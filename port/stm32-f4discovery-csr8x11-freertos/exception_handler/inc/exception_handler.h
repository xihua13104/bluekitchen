#ifndef __EXCEPTION_HANDLER__
#define __EXCEPTION_HANDLER__

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "stdio.h"
#include "stdarg.h"
#include <stdint.h>
#include <string.h>

/* Public define -------------------------------------------------------------*/
#define DISABLE_MEMDUMP_MAGIC                           0xdeadbeef
#define DISABLE_WHILELOOP_MAGIC                         0xdeadaaaa

#define EXCEPTION_WDT_INTERRUPT                         0xdeadbbbb
#define EXCEPTION_WDT_RESET                             0xdeadcccc

#define EXCEPTION_MEMDUMP_NODUMP                        0
#define EXCEPTION_MEMDUMP_TEXT                          1
#define EXCEPTION_MEMDUMP_BINARY                        2
#define EXCEPTION_MEMDUMP_MINIDUMP                      4

#define EXCEPTION_MEMDUMP_MODE                          (EXCEPTION_MEMDUMP_TEXT)


#define platform_printf                                 printf
#define exception_printf                                printf
#define exception_msgid                                 printf

/* Public typedef ------------------------------------------------------------*/
typedef enum
{
    EXCEPTION_STATUS_ERROR = 0,
    EXCEPTION_STATUS_OK = 1
} exception_status_t;

typedef struct
{
    uint32_t is_valid;
    const char *expr;
    const char *file;
    uint32_t line;
    const char *string;
} assert_expr_t;

typedef struct
{
    char *region_name;
    unsigned int *start_address;
    unsigned int *end_address;
    unsigned int is_dumped;
} memory_region_type;

typedef void (*f_exception_callback_t)(void);

typedef struct
{
    f_exception_callback_t init_cb;
    f_exception_callback_t dump_cb;
} exception_config_type;

/* exception dump configuration area */
typedef union{
    uint32_t exception_mode;
    struct {
        uint32_t exception_nodump          : 1;
        uint32_t exception_fulldump_text   : 1;
        uint32_t exception_fulldump_binary : 1;
        uint32_t exception_minidump        : 1;
        uint32_t mask_irq_check_assert     : 1;
        uint32_t reset_after_dump          : 1;
        uint32_t wdt_reset_mode            : 1;
        uint32_t exception_dummy_dump_mode : 1;
        uint32_t magic_number              : 24;
    } exception_mode_t;
} exception_config_mode_t;

/* Public functions ----------------------------------------------------------*/
void exception_feed_wdt(void);
void exception_enable_wdt_reset(void);
void exception_enable_wdt_interrupt(void);
void exception_get_assert_expr(const char **expr, const char **file, int *line);
void platform_assert(const char *expr, const char *file, int line);
void light_assert(const char *expr, const char *file, int line);
int  exception_dump_config_init(void);
void exception_reboot(void);
exception_status_t exception_register_callbacks(exception_config_type *cb);
exception_status_t exception_register_regions(memory_region_type *region);

#endif /* #ifndef __EXCEPTION_HANDLER__ */
