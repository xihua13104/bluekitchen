#ifndef __EXCEPTION_HANDLER__
#define __EXCEPTION_HANDLER__

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "stdio.h"
#include "stdarg.h"
#include <stdint.h>
#include <string.h>

/* Public define -------------------------------------------------------------*/
#define exception_printf                                printf

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

/* Public functions ----------------------------------------------------------*/
void exception_feed_wdt(void);
void exception_enable_wdt_reset(void);
void exception_enable_wdt_interrupt(void);
void exception_get_assert_expr(const char **expr, const char **file, int *line);
void platform_assert(const char *expr, const char *file, int line);
int  exception_dump_config_init(void);
void exception_reboot(void);
exception_status_t exception_register_callbacks(exception_config_type *cb);
exception_status_t exception_register_regions(memory_region_type *region);

#endif /* #ifndef __EXCEPTION_HANDLER__ */
