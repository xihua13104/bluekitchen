#ifndef __EXCEPTION_CONFIG__
#define __EXCEPTION_CONFIG__

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Public define -------------------------------------------------------------*/
#define EXCEPTION_MEMDUMP_DEFAULT                       (EXCEPTION_MEMDUMP_BINARY)

#define configUSE_FLASH_SUSPEND                         1

#define EXCEPTION_SLAVES_TOTAL                          1
#define EXCEPTION_SLAVES_TIMEOUT                        10000000

#define MINIDUMP_FIXED_STRING_SIZE                     (20 * 1024)

#define MINIDUMP_ADDRESS_OFFSET_CM4                     (0)
#define MINIDUMP_HEADER_LENGTH                          (512)
#define MINIDUMP_CONTEXT_LENGTH                         (512)
#define MINIDUMP_MSPSTACK_LENGTH                        (2 * 1024)
#define MINIDUMP_PSPSTACK_LENGTH                        (3 * 1024)
#define MINIDUMP_DATA_SIZE_CM4                          (6144)

#define EXCEPTION_MINIDUMP_LATEST_INDEX_OFFSET          0x4

#define EXCEPTION_CPU_ID_MASTER                         0
#define EXCEPTION_CPU_ID_SLAVE                          1

extern int log_print_exception_log(const char *message, ...);
extern void log_dump_exception_data(const uint8_t *data, uint32_t size);
#define EXCEPTION_PRINT_DEFAULT                         log_print_exception_log
#define EXCEPTION_MSGID_DEFAULT                         LOG_EXCEPTION_MSGID


/* Public typedef ------------------------------------------------------------*/
/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/


#endif /* #ifndef __EXCEPTION_CONFIG__ */
