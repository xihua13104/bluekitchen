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

#define BTSTACK_FILE__ "nordic_dfu_demo_bonding.c"

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

#define BTSTACK_FILE__ "nordic_dfu_demo_bonding.c"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "btstack.h"
#include "nrf_dfu_ble.h"
#include "nordic_dfu_demo_bonding.h"
#include "nrf_dfu.h"

typedef struct {
	uint16_t start_att_handle;
	uint16_t end_att_handle;
} gatt_service_change_range_t;

typedef struct {
	uint16_t att_handle;
	uint8_t is_indicate_en;
    hci_con_handle_t con_handle;
    gatt_service_change_range_t range;
} gatt_service_change_context_t;

uint8_t adv_data[] = {
    // Flags general discoverable, BR/EDR not supported
    2, BLUETOOTH_DATA_TYPE_FLAGS, 0x06,
    // Name
    9, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'D', 'f', 'u', 'b', 'o', 'n', 'd', '0',
// UUID DFU
    3, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, 0x59, 0xfe,
    // Device name
    //8, BLUETOOTH_DATA_TYPE_LE_BLUETOOTH_DEVICE_ADDRESS, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};
const uint8_t adv_data_len = sizeof(adv_data);

const uint8_t bootloader_profile_data[] =
{
    // ATT DB Version
    1,

    // 0x0001 PRIMARY_SERVICE-GAP_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x00, 0x18, 
    // 0x0002 CHARACTERISTIC-GAP_DEVICE_NAME-READ
    0x0d, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x02, 0x03, 0x00, 0x00, 0x2a, 
    // 0x0003 VALUE-GAP_DEVICE_NAME-READ-'nRF DFU Target'
    // READ_ANYBODY
    0x16, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00, 0x2a, 0x6e, 0x52, 0x46, 0x20, 0x44, 0x46, 0x55, 0x20, 0x54, 0x61, 0x72, 0x67, 0x65, 0x74, 
    // add Battery Service
    //#import <battery_service.gatt>

    // 0x0004 PRIMARY_SERVICE-GATT_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x28, 0x01, 0x18, 
    // 0x0005 CHARACTERISTIC-GATT_SERVICE_CHANGED-READ | INDICATE
    0x0d, 0x00, 0x02, 0x00, 0x05, 0x00, 0x03, 0x28, 0x22, 0x06, 0x00, 0x05, 0x2a, 
    // 0x0006 VALUE-GATT_SERVICE_CHANGED-READ | INDICATE-'00 00 0F 00'
    // READ_ANYBODY
    0x0c, 0x00, 0x02, 0x00, 0x06, 0x00, 0x05, 0x2a, 0x00, 0x00, 0x0F, 0x00, 
    // 0x0007 CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0e, 0x01, 0x07, 0x00, 0x02, 0x29, 0x00, 0x00, 
    //Nordic DFU service

    // 0x0008 PRIMARY_SERVICE-FE59
    0x0a, 0x00, 0x02, 0x00, 0x08, 0x00, 0x00, 0x28, 0x59, 0xfe, 
    // 0x0009 CHARACTERISTIC-8EC90001-F315-4F60-9FB8-838830DAEA50-READ | WRITE | NOTIFY | DYNAMIC
    0x1b, 0x00, 0x02, 0x00, 0x09, 0x00, 0x03, 0x28, 0x1a, 0x0a, 0x00, 0x50, 0xea, 0xda, 0x30, 0x88, 0x83, 0xb8, 0x9f, 0x60, 0x4f, 0x15, 0xf3, 0x01, 0x00, 0xc9, 0x8e, 
    // 0x000a VALUE-8EC90001-F315-4F60-9FB8-838830DAEA50-READ | WRITE | NOTIFY | DYNAMIC-''
    // READ_ANYBODY, WRITE_ANYBODY
    0x16, 0x00, 0x0a, 0x03, 0x0a, 0x00, 0x50, 0xea, 0xda, 0x30, 0x88, 0x83, 0xb8, 0x9f, 0x60, 0x4f, 0x15, 0xf3, 0x01, 0x00, 0xc9, 0x8e, 
    // 0x000b CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0e, 0x01, 0x0b, 0x00, 0x02, 0x29, 0x00, 0x00, 
    // 0x000c CHARACTERISTIC-8EC90002-F315-4F60-9FB8-838830DAEA50-READ | WRITE | WRITE_WITHOUT_RESPONSE |DYNAMIC
    0x1b, 0x00, 0x02, 0x00, 0x0c, 0x00, 0x03, 0x28, 0x0e, 0x0d, 0x00, 0x50, 0xea, 0xda, 0x30, 0x88, 0x83, 0xb8, 0x9f, 0x60, 0x4f, 0x15, 0xf3, 0x02, 0x00, 0xc9, 0x8e, 
    // 0x000d VALUE-8EC90002-F315-4F60-9FB8-838830DAEA50-READ | WRITE | WRITE_WITHOUT_RESPONSE |DYNAMIC-''
    // READ_ANYBODY, WRITE_ANYBODY
    0x16, 0x00, 0x0e, 0x03, 0x0d, 0x00, 0x50, 0xea, 0xda, 0x30, 0x88, 0x83, 0xb8, 0x9f, 0x60, 0x4f, 0x15, 0xf3, 0x02, 0x00, 0xc9, 0x8e, 
    //CHARACTERISTIC,  8EC90003-F315-4F60-9FB8-838830DAEA50, READ | WRITE | INDICATE | DYNAMIC

    // END
    0x00, 0x00, 
}; // total size 112 bytes 

static btstack_packet_callback_registration_t hci_event_callback_registration;
static btstack_packet_callback_registration_t sm_event_callback_registration;
#if defined(MCUBOOT_IMG_APPLICATION1)
static uint8_t bootloader_adv_name[8] = {'D', 'f', 'u', 'b', 'o', 'n', 'd', '1'};
#elif defined(MCUBOOT_IMG_APPLICATION2)
static uint8_t bootloader_adv_name[8] = {'D', 'f', 'u', 'b', 'o', 'n', 'd', '2'};
#else
static uint8_t bootloader_adv_name[8] = {'D', 'f', 'u', 'b', 'o', 'n', 'd', '0'};
#endif

static uint8_t is_enter_bootloader = 0;

static gatt_service_change_context_t gatt_service_change_ctx;

static void set_up_advertisement(uint8_t adv_data_length, uint8_t *adv_data)
{
    uint16_t adv_int_min = 0x0030;
    uint16_t adv_int_max = 0x0030;
    uint8_t adv_type = 0;
    bd_addr_t null_addr;
    uint8_t adv_filter_policy = 0;
    memset(null_addr, 0, 6);
    /* Advertising_Filter_Policy 
     0x00: Disable the whitelist mechanism, allowing any device to connect and scan.
     0x01: Allow any device to connect, but only allow scanning of devices in the whitelist.
     0x02: Allow any device to scan, but only allow devices in the whitelist to connect.
     0x03: Only devices in the whitelist are allowed to scan and connect.
    */
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, adv_filter_policy);
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
    bd_addr_t addr;
    bd_addr_type_t addr_type;
    uint8_t status;

    if (packet_type != HCI_EVENT_PACKET) return;

    switch (hci_event_packet_get_type(packet)) {
        case BTSTACK_EVENT_STATE:
            // BTstack activated, get started
            if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
                memcpy(&adv_data[5], bootloader_adv_name, sizeof(bootloader_adv_name));
                set_up_advertisement(adv_data_len, adv_data);
            } else if (btstack_event_state_get_state(packet) == HCI_STATE_OFF) {
                hci_power_control(HCI_POWER_ON);
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
                    // print peer address
                    hci_subevent_le_connection_complete_get_peer_address(packet, addr);
                    printf("LE Connection - Connection peer addr:%s\n", bd_addr_to_str(addr));
                    //if (!is_enter_bootloader) {
                    //    gap_whitelist_add(BD_ADDR_TYPE_LE_PUBLIC, addr);
                    //}
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

                    if (is_enter_bootloader) {
                        if (gatt_service_change_ctx.is_indicate_en)
                            att_server_request_can_send_now_event(gatt_service_change_ctx.con_handle);
                        break;
                    }
                    // for testing, choose one of the following actions
                    // manually start pairing
                    sm_request_pairing(con_handle);

                    // gatt client request to authenticated characteristic in sm_pairing_central (short cut, uses hard-coded value handle)
                    // gatt_client_read_value_of_characteristic_using_value_handle(&packet_handler, con_handle, 0x0009);

                    // general gatt client request to trigger mandatory authentication
                    // gatt_client_discover_primary_services(&packet_handler, con_handle);
                    break;
                default:
                    break;
            }
            break;
        case SM_EVENT_JUST_WORKS_REQUEST:
            printf("Just Works requested\n");
            sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
            break;
        case SM_EVENT_NUMERIC_COMPARISON_REQUEST:
            printf("Confirming numeric comparison: %"PRIu32"\n", sm_event_numeric_comparison_request_get_passkey(packet));
            sm_numeric_comparison_confirm(sm_event_passkey_display_number_get_handle(packet));
            break;
        case SM_EVENT_PASSKEY_DISPLAY_NUMBER:
            printf("Display Passkey: %"PRIu32"\n", sm_event_passkey_display_number_get_passkey(packet));
            break;
        case SM_EVENT_IDENTITY_CREATED:
            sm_event_identity_created_get_identity_address(packet, addr);
            printf("Identity created: type %u address %s\n", sm_event_identity_created_get_identity_addr_type(packet), bd_addr_to_str(addr));
            break;
        case SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED:
            sm_event_identity_resolving_succeeded_get_identity_address(packet, addr);
            printf("Identity resolved: type %u address %s\n", sm_event_identity_resolving_succeeded_get_identity_addr_type(packet), bd_addr_to_str(addr));
            break;
        case SM_EVENT_IDENTITY_RESOLVING_FAILED:
            sm_event_identity_created_get_address(packet, addr);
            printf("Identity resolving failed\n");
            break;
        case SM_EVENT_PAIRING_STARTED:
            printf("Pairing started\n");
            break;
        case SM_EVENT_PAIRING_COMPLETE:
            switch (sm_event_pairing_complete_get_status(packet)){
                case ERROR_CODE_SUCCESS:
                    printf("Pairing complete, success\n");
                    break;
                case ERROR_CODE_CONNECTION_TIMEOUT:
                    printf("Pairing failed, timeout\n");
                    break;
                case ERROR_CODE_REMOTE_USER_TERMINATED_CONNECTION:
                    printf("Pairing failed, disconnected\n");
                    break;
                case ERROR_CODE_AUTHENTICATION_FAILURE:
                    printf("Pairing failed, authentication failure with reason = %u\n", sm_event_pairing_complete_get_reason(packet));
                    break;
                default:
                    break;
            }
            break;
        case SM_EVENT_REENCRYPTION_STARTED:
            sm_event_reencryption_complete_get_address(packet, addr);
            printf("Bonding information exists for addr type %u, identity addr %s -> re-encryption started\n",
                   sm_event_reencryption_started_get_addr_type(packet), bd_addr_to_str(addr));
            break;
        case SM_EVENT_REENCRYPTION_COMPLETE:
            switch (sm_event_reencryption_complete_get_status(packet)){
                case ERROR_CODE_SUCCESS:
                    printf("Re-encryption complete, success\n");
                    break;
                case ERROR_CODE_CONNECTION_TIMEOUT:
                    printf("Re-encryption failed, timeout\n");
                    break;
                case ERROR_CODE_REMOTE_USER_TERMINATED_CONNECTION:
                    printf("Re-encryption failed, disconnected\n");
                    break;
                case ERROR_CODE_PIN_OR_KEY_MISSING:
                    printf("Re-encryption failed, bonding information missing\n\n");
                    printf("Assuming remote lost bonding information\n");
                    printf("Deleting local bonding information to allow for new pairing...\n");
                    sm_event_reencryption_complete_get_address(packet, addr);
                    addr_type = sm_event_reencryption_started_get_addr_type(packet);
                    gap_delete_bonding(addr_type, addr);
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
            if (is_enter_bootloader) {
                gatt_service_change_ctx.range.start_att_handle = 0x00;
                gatt_service_change_ctx.range.end_att_handle = 0x0F;
                att_server_indicate(gatt_service_change_ctx.con_handle,
                                ATT_CHARACTERISTIC_GATT_SERVICE_CHANGED_01_VALUE_HANDLE,
                                &gatt_service_change_ctx.range,
                                sizeof(gatt_service_change_ctx.range));
            }
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
            printf("new bootloader name:%s \n", (char *)packet);
            memcpy(&bootloader_adv_name, packet, size);
            break;
        case NRF_DFU_EVT_ENTER_BOOTLOADER_MODE:
            hci_power_control(HCI_POWER_OFF);
            att_server_init(bootloader_profile_data, NULL, NULL);
            is_enter_bootloader = 1;
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

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size){
    UNUSED(connection_handle);
    return 0;
}

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size){
    UNUSED(transaction_mode);
    UNUSED(offset);
    UNUSED(buffer_size);
    
    if (att_handle == ATT_CHARACTERISTIC_GATT_SERVICE_CHANGED_01_CLIENT_CONFIGURATION_HANDLE) {
        gatt_service_change_ctx.is_indicate_en = little_endian_read_16(buffer, 0) == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION;
        gatt_service_change_ctx.con_handle = connection_handle;
    }
    return 0;
}

int btstack_main(void);
int btstack_main(void){
    // register for HCI events
    hci_event_callback_registration.callback = &hci_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    sm_event_callback_registration.callback = &hci_packet_handler;
    sm_add_event_handler(&sm_event_callback_registration);

    l2cap_init();

    // setup LE device DB
    le_device_db_init();

    // setup SM: Display only
    sm_init();

    // setup ATT server
    att_server_init(profile_data, att_read_callback, att_write_callback);
    
    // setup nordic dfu server
    nrf_dfu_init(nordic_dfu_evt_observer);

    // register for ATT events
    att_server_register_packet_handler(att_packet_handler);

    // Configuration

    // Enable mandatory authentication for GATT Client
    // - if un-encrypted connections are not supported, e.g. when connecting to own device, this enforces authentication
    // gatt_client_set_required_security_level(LEVEL_2);

    /**
     * Choose ONE of the following configurations
     * Bonding is disabled to allow for repeated testing. It can be enabled by or'ing
     * SM_AUTHREQ_BONDING to the authentication requirements like this:
     * sm_set_authentication_requirements( X | SM_AUTHREQ_BONDING)
     */

#ifndef ENABLE_LE_SECURE_CONNECTIONS
    // LE Legacy Pairing, Just Works
    //sm_set_io_capabilities(IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
    //sm_set_authentication_requirements(SM_AUTHREQ_BONDING);

    // LE Legacy Pairing, Passkey entry initiator enter, responder (us) displays
    sm_set_io_capabilities(IO_CAPABILITY_DISPLAY_ONLY);
    sm_set_authentication_requirements(SM_AUTHREQ_MITM_PROTECTION|SM_AUTHREQ_BONDING);
    sm_use_fixed_passkey_in_display_role(123456);

#else
    // enable LE Secure Connections Only mode - disables Legacy pairing
    sm_set_secure_connections_only_mode(true);

    // LE Secure Connections, Just Works
    // sm_set_io_capabilities(IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
    // sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION);

    // LE Secure Connections, Numeric Comparison
    sm_set_io_capabilities(IO_CAPABILITY_DISPLAY_YES_NO);
    sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION|SM_AUTHREQ_MITM_PROTECTION|SM_AUTHREQ_BONDING);

    // LE Secure Pairing, Passkey entry initiator enter, responder (us) displays
    // sm_set_io_capabilities(IO_CAPABILITY_DISPLAY_ONLY);
    // sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION|SM_AUTHREQ_MITM_PROTECTION);
    // sm_use_fixed_passkey_in_display_role(123456);

    // LE Secure Pairing, Passkey entry initiator displays, responder (us) enter
    // sm_set_io_capabilities(IO_CAPABILITY_KEYBOARD_ONLY);
    // sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION|SM_AUTHREQ_MITM_PROTECTION);
#endif

    // turn on!
	hci_power_control(HCI_POWER_ON);
	    
    return 0;
}
/* EXAMPLE_END */
