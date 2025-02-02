//
// btstack_config.h for STM32 F4 Discovery + csr8311 port
//
// Documentation: https://bluekitchen-gmbh.com/btstack/#how_to/
//

#ifndef BTSTACK_CONFIG_H
#define BTSTACK_CONFIG_H

// Port related features
#define HAVE_BTSTACK_STDIN
#define HAVE_EMBEDDED_TIME_MS
#define ENABLE_BTSTACK_ASSERT
// BTstack features that can be enabled

#ifdef BLE_ENABLE
    #define ENABLE_BLE
#endif

#ifdef BR_EDR_ENABLE
    #define ENABLE_CLASSIC
#endif
//#define ENABLE_BTSTACK_ASSERT

#ifdef ENABLE_BLE
    #define ENABLE_LE_CENTRAL
    #define ENABLE_LE_DATA_CHANNELS
    #define ENABLE_LE_PERIPHERAL
    //#define ENABLE_LE_SECURE_CONNECTIONS
    #ifdef ENABLE_LE_SECURE_CONNECTIONS
        #define ENABLE_MICRO_ECC_FOR_LE_SECURE_CONNECTIONS
        #define ENABLE_MICRO_ECC_P256
    #endif
#endif

#define ENABLE_LOG_ERROR
#define ENABLE_LOG_INFO
#define ENABLE_LOG_DEBUG
#define ENABLE_PRINTF_HEXDUMP
//#define ENABLE_SEGGER_RTT

#ifdef ENABLE_CLASSIC
    #define HAVE_HAL_AUDIO
    #define HAVE_HAL_AUDIO_SINK_STEREO_ONLY

    #define ENABLE_HFP_WIDE_BAND_SPEECH
    #define ENABLE_GATT_OVER_CLASSIC
    #define MAX_NR_AVDTP_CONNECTIONS 1
    #define MAX_NR_AVDTP_STREAM_ENDPOINTS 1
    #define MAX_NR_AVRCP_CONNECTIONS 2
    #define MAX_NR_BNEP_CHANNELS 1
    #define MAX_NR_BNEP_SERVICES 1
    #define MAX_NR_HFP_CONNECTIONS 1
    #define MAX_NR_RFCOMM_CHANNELS 1
    #define MAX_NR_RFCOMM_MULTIPLEXERS 1
    #define MAX_NR_RFCOMM_SERVICES 1
#endif

// BTstack configuration. buffers, sizes, ...
#define HCI_ACL_PAYLOAD_SIZE (1691 + 4)
#define MAX_NR_BTSTACK_LINK_KEY_DB_MEMORY_ENTRIES  2
#define MAX_NR_GATT_CLIENTS 1
#define MAX_NR_HCI_CONNECTIONS 2
#define MAX_NR_L2CAP_CHANNELS  5
#define MAX_NR_L2CAP_SERVICES  5
#define MAX_NR_SERVICE_RECORD_ITEMS 4
#define MAX_NR_SM_LOOKUP_ENTRIES 3
#define MAX_NR_WHITELIST_ENTRIES 1

// Link Key DB and LE Device DB using TLV on top of Flash Sector interface
#define NVM_NUM_DEVICE_DB_ENTRIES 16
#define NVM_NUM_LINK_KEYS 16

#ifdef BTSTACK_FREERTOS_ENABLE
    #define HAVE_FREERTOS_TASK_NOTIFICATIONS
#endif

#endif