/*
 * Copyright (C) 2014 BlueKitchen GmbH
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

#define BTSTACK_FILE__ "nordic_dfu_demo_bootloader_only.c"

// *****************************************************************************
/* EXAMPLE_START(nordic_dfu): LE Nordic dfu example 
 *
 * @text All newer operating systems provide GATT Client functionality.
 * This example shows how to get a maximal throughput via BLE:
 * - send whenever possible,
 * - use the max ATT MTU.
 *
 * @text In theory, we should also update the connection parameters, but we already get
 * a connection interval of 30 ms and there's no public way to use a shorter 
 * interval with iOS (if we're not implementing an HID device).
 *
 * @text Note: To start the streaming, run the example.
 * On remote device use some GATT Explorer, e.g. LightBlue, BLExplr to enable notifications.
 */
 // *****************************************************************************

#define BTSTACK_FILE__ "nordic_dfu_demo_bootloader_only.c"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "btstack.h"
#include "nrf_dfu_ble.h"
#include "nordic_dfu_demo_bootloader_only.h"
#include "nrf_dfu.h"

uint8_t adv_data[] = {
    // Flags general discoverable, BR/EDR not supported
    2, BLUETOOTH_DATA_TYPE_FLAGS, 0x06,
    // Name
    15, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'D', 'f', 'u', 'b', 'l', 'o', 't', 'l', 'o', 'a', 'd', 'e', 'r', '0',
// UUID DFU
    3, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, 0x59, 0xfe,
    // Device name
    //8, BLUETOOTH_DATA_TYPE_LE_BLUETOOTH_DEVICE_ADDRESS, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};
const uint8_t adv_data_len = sizeof(adv_data);

static btstack_packet_callback_registration_t hci_event_callback_registration;
#if defined(MCUBOOT_IMG_APPLICATION1)
static uint8_t bootloader_adv_name[] = {'D', 'f', 'u', 'b', 'o', 'o', 't', 'l', 'o', 'a', 'd', 'e', 'r', '1'};
#elif defined(MCUBOOT_IMG_APPLICATION2)
static uint8_t bootloader_adv_name[] = {'D', 'f', 'u', 'b', 'o', 'o', 't', 'l', 'o', 'a', 'd', 'e', 'r', '2'};
#else
static uint8_t bootloader_adv_name[] = {'D', 'f', 'u', 'b', 'o', 'o', 't', 'l', 'o', 'a', 'd', 'e', 'r', '0'};
#endif

static void set_up_advertisement(uint8_t adv_data_length, uint8_t *adv_data)
{
    uint16_t adv_int_min = 0x0030;
    uint16_t adv_int_max = 0x0030;
    uint8_t adv_type = 0;
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
    gap_advertisements_set_data(adv_data_len, (uint8_t*) adv_data);
    gap_advertisements_enable(1);
}

/* LISTING_END(tracking): Tracking throughput */

/* 
 * @section HCI Packet Handler
 *
 * @text The packet handler prints the welcome message and requests a connection paramter update for LE Connections
 */

/* LISTING_START(packetHandler): Packet Handler */

static void hci_packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(channel);
    UNUSED(size);

    uint16_t conn_interval;
    hci_con_handle_t con_handle;

    if (packet_type != HCI_EVENT_PACKET) return;

    switch (hci_event_packet_get_type(packet)) {
        case BTSTACK_EVENT_STATE:
            // BTstack activated, get started
            if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
                memcpy(&adv_data[5], bootloader_adv_name, sizeof(bootloader_adv_name));
                set_up_advertisement(adv_data_len, adv_data);
            }
            break;
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            con_handle = HCI_CON_HANDLE_INVALID;
            break;
        case HCI_EVENT_COMMAND_COMPLETE:
            break;
        case HCI_EVENT_LE_META:
            switch (hci_event_le_meta_get_subevent_code(packet)) {
                case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
                    // print connection parameters (without using float operations)
                    con_handle    = hci_subevent_le_connection_complete_get_connection_handle(packet);
                    conn_interval = hci_subevent_le_connection_complete_get_conn_interval(packet);
                    printf("LE Connection - Connection Interval: %u.%02u ms\n", conn_interval * 125 / 100, 25 * (conn_interval & 3));
                    printf("LE Connection - Connection Latency: %u\n", hci_subevent_le_connection_complete_get_conn_latency(packet));

                    // request min con interval 15 ms for iOS 11+ 
                    printf("LE Connection - Request 15 ms connection interval\n");
                    gap_request_connection_parameter_update(con_handle, 12, 12, 0, 0x0048);
                    break;
                case HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE:
                    // print connection parameters (without using float operations)
                    con_handle    = hci_subevent_le_connection_update_complete_get_connection_handle(packet);
                    conn_interval = hci_subevent_le_connection_update_complete_get_conn_interval(packet);
                    printf("LE Connection - Connection Param update - connection interval %u.%02u ms, latency %u\n", conn_interval * 125 / 100,
                        25 * (conn_interval & 3), hci_subevent_le_connection_update_complete_get_conn_latency(packet));
                    break;
                default:
                    break;
            }
            break;  
        default:
            break;
    }
}
/* LISTING_END */

/* 
 * @section ATT Packet Handler
 *
 * @text The packet handler is used to setup and tear down the spp-over-gatt connection and its MTU
 */

/* LISTING_START(packetHandler): Packet Handler */
static void att_packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(channel);
    UNUSED(size);
    
    if (packet_type != HCI_EVENT_PACKET) return;

    switch (hci_event_packet_get_type(packet)) {
        case ATT_EVENT_CONNECTED:
            break;
        case ATT_EVENT_MTU_EXCHANGE_COMPLETE:
            break;
        case ATT_EVENT_CAN_SEND_NOW:
            break;
        case ATT_EVENT_DISCONNECTED:
            break;
        default:
            break;
    }
}
/* LISTING_END */

static void nordic_dfu_evt_observer(nrf_dfu_evt_type_t evt, uint8_t *packet, uint32_t size) {
    printf("%s evt:%d \n", __func__, evt);

    switch (evt) {
        case NRF_DFU_EVT_CHANGE_BOOTLOADER_NAME:
            break;
        case NRF_DFU_EVT_ENTER_BOOTLOADER_MODE:
            break;
        case NRF_DFU_EVT_DFU_COMPLETED:
#if defined(MCUBOOT_IMG_APPLICATION1) ||defined(MCUBOOT_IMG_APPLICATION2)
            boot_set_pending(1);
#endif
            hal_cpu_reset();
            break;
        default:
            break;
    }
}

int btstack_main(void);
int btstack_main(void){
    // register for HCI events
    hci_event_callback_registration.callback = &hci_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    l2cap_init();

    // setup LE device DB
    le_device_db_init();

    // setup SM: Display only
    sm_init();

    // setup ATT server
    att_server_init(profile_data, NULL, NULL);
    
    // setup nordic dfu server
    nrf_dfu_init(nordic_dfu_evt_observer);

    // register for ATT events
    att_server_register_packet_handler(att_packet_handler);

    // turn on!
	hci_power_control(HCI_POWER_ON);
	    
    return 0;
}
/* EXAMPLE_END */
