/*
 * Copyright (C) 2019 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

/*
 *  Made for BlueKitchen by OneWave with <3
 *      Author: ftrefou@onewave.io
 */
#define BTSTACK_FILE__ "btstack_port.c"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "btstack_config.h"

#ifdef ENABLE_SEGGER_RTT
#include "hci_dump_segger_rtt_stdout.h"
#else
#include "hci_dump_embedded_stdout.h"
#endif

#include "bt_drv.h"
#include "FreeRTOS.h"
#include "task.h"


/********************************************
 *      hal_time_ms.h implementation
 *******************************************/
#include "hal_time_ms.h"
uint32_t hal_time_ms(void)
{
	return xTaskGetTickCount();
}


/*******************************************
 *       hal_cpu.h implementation
 ******************************************/
#include "hal_cpu.h"

void hal_cpu_disable_irqs(void){
    portDISABLE_INTERRUPTS();
}

void hal_cpu_enable_irqs(void){
    portENABLE_INTERRUPTS();
}

void hal_cpu_enable_irqs_and_sleep(void){
    portENABLE_INTERRUPTS();
    //__asm__("wfe"); // go to sleep if event flag isn't set. if set, just clear it. IRQs set event flag
}



/*******************************************
 *       transport implementation
 ******************************************/

// #define USE_SRAM_FLASH_BANK_EMU

#include "btstack.h"
#include "btstack_config.h"
#include "btstack_event.h"
#include "btstack_memory.h"
#include "btstack_run_loop.h"
#include "btstack_run_loop_freertos.h"
#include "btstack_tlv_flash_bank.h"
#include "le_device_db_tlv.h"
#include "hci.h"
#include "hci_dump.h"
#include "btstack_debug.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

typedef struct {
    unsigned char *p_buf;
    unsigned int length;
} bt_hci_rx_packet_t;

static void (*transport_packet_handler)(uint8_t packet_type, uint8_t *packet, uint16_t size);

static QueueHandle_t hci_rx_data_queue;
// data source for integration with BTstack Runloop
static btstack_data_source_t transport_data_source;
static btstack_tlv_flash_bank_t btstack_tlv_flash_bank_context;


// run from main thread
static void transport_send_hardware_error(uint8_t error_code){
    uint8_t event[] = { HCI_EVENT_HARDWARE_ERROR, 1, error_code};
    transport_packet_handler(HCI_EVENT_PACKET, &event[0], sizeof(event));
}

static void transport_notify_packet_send(void){
    // notify upper stack that it might be possible to send again
    uint8_t event[] = { HCI_EVENT_TRANSPORT_PACKET_SENT, 0};
    transport_packet_handler(HCI_EVENT_PACKET, &event[0], sizeof(event));
}

static void transport_notify_ready(void){
    // notify upper stack that it transport is ready
    uint8_t event[] = { HCI_EVENT_TRANSPORT_READY, 0};
    transport_packet_handler(HCI_EVENT_PACKET, &event[0], sizeof(event));
}

static void aml_bt_drv_recv_cb(unsigned char *p_buf, unsigned int length)
{
    bt_hci_rx_packet_t packet = {0};

    if (p_buf && length) {
        packet.p_buf = pvPortMalloc(length);
        memcpy(packet.p_buf, p_buf, length);
        packet.length = length;
    }
    xQueueSend(hci_rx_data_queue, (void*)&packet, portMAX_DELAY);
    btstack_run_loop_freertos_trigger();
}

/**
 * init transport
 * @param transport_config
 */
static void transport_init(const void *transport_config){
    printf("transport_init\n");
    aml_bt_drv_init(aml_bt_drv_recv_cb);
    hci_rx_data_queue = xQueueCreate(5, sizeof(bt_hci_rx_packet_t));
}

static void transport_deliver_hci_packets(void) {
    bt_hci_rx_packet_t hci_packet;

    // process hci packets
    while (xQueueReceive(hci_rx_data_queue, &hci_packet, 0) == pdTRUE)
    {
        switch (hci_packet.p_buf[0])
        {
            case HCI_EVENT_PACKET:
            case HCI_ACL_DATA_PACKET:
                /* Send buffer to upper stack */
                transport_packet_handler(hci_packet.p_buf[0],
                                        hci_packet.p_buf + 1,
                                        hci_packet.length - 1);
                vPortFree(hci_packet.p_buf);
                break;
       
            default:
                transport_send_hardware_error(0x01);  // invalid HCI packet
                break;
        }
    }
}

static void transport_process(btstack_data_source_t *ds, btstack_data_source_callback_type_t callback_type) {
    switch (callback_type){
        case DATA_SOURCE_CALLBACK_POLL:
            // process hci packets
            transport_deliver_hci_packets();
            break;
        default:
            break;
    }
}

/**
 * open transport connection
 */
static int transport_open(void){
    printf("transport_open\n");
    // set up polling data_source
    btstack_run_loop_set_data_source_handler(&transport_data_source, &transport_process);
    btstack_run_loop_enable_data_source_callbacks(&transport_data_source, DATA_SOURCE_CALLBACK_POLL);
    btstack_run_loop_add_data_source(&transport_data_source);
    aml_bt_drv_power_on();
    return 0;
}

/**
 * close transport connection
 */
static int transport_close(void){
    printf("transport_close\n");
    // remove data source
    btstack_run_loop_disable_data_source_callbacks(&transport_data_source, DATA_SOURCE_CALLBACK_POLL);
    btstack_run_loop_remove_data_source(&transport_data_source);
    aml_bt_drv_power_off();
    return 0;
}

/**
 * register packet handler for HCI packets: ACL and Events
 */
static void transport_register_packet_handler(void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size)){
    printf("transport_register_packet_handler\n");
    transport_packet_handler = handler;
}

/**
 * support async transport layers, e.g. IRQ driven without buffers
 */
static int transport_can_send_packet_now(uint8_t packet_type) {
    switch (packet_type)
    {
        case HCI_COMMAND_DATA_PACKET:
            return 1;

        case HCI_ACL_DATA_PACKET:
            return 1;
    }
    return 1;
}

/**
 * send packet
 */
static int transport_send_packet(uint8_t packet_type, uint8_t *packet, int size){
    switch (packet_type){
        case HCI_COMMAND_DATA_PACKET:
        case HCI_ACL_DATA_PACKET:
            aml_bt_drv_data_send(packet_type, packet, size);
            transport_notify_packet_send();
            break;

        default:
            transport_send_hardware_error(0x01);  // invalid HCI packet
            break;
    }
    return 0;  
}

static const hci_transport_t transport = {
    "amlogic-hci-h4",
    &transport_init,
    &transport_open,
    &transport_close,
    &transport_register_packet_handler,
    &transport_can_send_packet_now,
    &transport_send_packet,
    NULL, // set baud rate
    NULL, // reset link
    NULL, // set SCO config
};


static const hci_transport_t * transport_get_instance(void){
    return &transport;
}

static btstack_packet_callback_registration_t hci_event_callback_registration;


static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    if (packet_type != HCI_EVENT_PACKET) return;
    switch(hci_event_packet_get_type(packet)){
        // wait with TLV init (which uses HW semaphores to coordinate with CPU2) until CPU2 was started
        case HCI_EVENT_TRANSPORT_READY:
            break;
        default:
            break;
    }
}

extern int btstack_main(int argc, const char * argv[]);

void port_thread(void* args){
    // enable packet logger
#ifdef ENABLE_HCI_DUMP
#ifdef ENABLE_SEGGER_RTT
    // hci_dump_init(hci_dump_segger_rtt_stdout_get_instance());
#else
    hci_dump_init(hci_dump_embedded_stdout_get_instance());
#endif
#endif

    /// GET STARTED with BTstack ///
    btstack_memory_init();
    btstack_run_loop_init(btstack_run_loop_freertos_get_instance());

    // init HCI
    hci_init(transport_get_instance(), NULL);
    
    // inform about BTstack state
    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    btstack_main(0, NULL);

    printf("btstack executing run loop...\n");
    btstack_run_loop_execute();
}

static TaskHandle_t hbtstack_task;

void btstack_go(void)
{
    xTaskCreate(port_thread, "btstack_thread", 2048, NULL, 7, &hbtstack_task);
}