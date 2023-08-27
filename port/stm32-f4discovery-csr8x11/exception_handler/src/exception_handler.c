/* Includes ------------------------------------------------------------------*/
#include "exception_handler.h"
#include "stm32f4xx_hal.h"
#include "core_cm4.h"

#ifdef WDT_ENABLE
#include "hal_wdt.h"
#endif

/* Private define ------------------------------------------------------------*/
#if  defined ( __GNUC__ )
#ifndef __weak
#define __weak   __attribute__((weak))
#endif /* __weak */
#endif /* __GNUC__ */

#define EXCEPTION_CONFIGURATIONS_MAX                    6

#define EXCEPTION_STACK_WORDS                           768

/* Private typedef -----------------------------------------------------------*/
typedef enum {
    EXCEPTION_RESET     = 1,
    EXCEPTION_NMI       = 2,
    EXCEPTION_HARD_FAULT = 3,
    EXCEPTION_MEMMANAGE_FAULT = 4,
    EXCEPTION_BUS_FAULT = 5,
    EXCEPTION_USAGE_FAULT = 6,
    EXCEPTION_DEBUGMON_FAULT = 12,
} exception_type_t;

typedef struct {
    int items;
    exception_config_type configs[EXCEPTION_CONFIGURATIONS_MAX];
} exception_config_t;

typedef struct {
    uint32_t items;
    memory_region_type regions[EXCEPTION_CONFIGURATIONS_MAX];
} exception_user_regions_t;

typedef struct {
    uint32_t count;
    uint32_t timestamp;
    uint32_t timestamp_32k;
    uint32_t reason;
    assert_expr_t *assert_expr;
} exception_info_t;

enum { r0, r1, r2, r3, r12, lr, pc, psr,
       s0, s1, s2, s3, s4, s5, s6, s7,
       s8, s9, s10, s11, s12, s13, s14, s15,
       fpscr
     };

typedef struct {
    unsigned int r0;
    unsigned int r1;
    unsigned int r2;
    unsigned int r3;
    unsigned int r4;
    unsigned int r5;
    unsigned int r6;
    unsigned int r7;
    unsigned int r8;
    unsigned int r9;
    unsigned int r10;
    unsigned int r11;
    unsigned int r12;
    unsigned int sp;              /* after pop r0-r3, lr, pc, xpsr                   */
    unsigned int lr;              /* lr before exception                             */
    unsigned int pc;              /* pc before exception                             */
    unsigned int psr;             /* xpsr before exeption                            */
    unsigned int control;         /* nPRIV bit & FPCA bit meaningful, SPSEL bit = 0  */
    unsigned int exc_return;      /* current lr                                      */
    unsigned int msp;             /* msp                                             */
    unsigned int psp;             /* psp                                             */
    unsigned int basepri;         /* basepri                                         */
    unsigned int primask;         /* primask                                         */
    unsigned int faultmask;       /* faultmask                                       */
    unsigned int fpscr;
    unsigned int s0;
    unsigned int s1;
    unsigned int s2;
    unsigned int s3;
    unsigned int s4;
    unsigned int s5;
    unsigned int s6;
    unsigned int s7;
    unsigned int s8;
    unsigned int s9;
    unsigned int s10;
    unsigned int s11;
    unsigned int s12;
    unsigned int s13;
    unsigned int s14;
    unsigned int s15;
    unsigned int s16;
    unsigned int s17;
    unsigned int s18;
    unsigned int s19;
    unsigned int s20;
    unsigned int s21;
    unsigned int s22;
    unsigned int s23;
    unsigned int s24;
    unsigned int s25;
    unsigned int s26;
    unsigned int s27;
    unsigned int s28;
    unsigned int s29;
    unsigned int s30;
    unsigned int s31;
} exception_context_t;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* exception handler's stack */
static unsigned int exception_stack[EXCEPTION_STACK_WORDS] = {0};
unsigned int *exception_stack_pointer = &exception_stack[EXCEPTION_STACK_WORDS - 1];

/* exception user configuration area */
static exception_config_t exception_config = {0};

/* avoid other global value corrupt exception_user_regions */
static exception_user_regions_t exception_user_regions;

/* exception reboot configuration area */
static int reboot_flag = 0;

/* assert information area */
static assert_expr_t assert_expr = {0};
static const char *assert_string = "CM4 assert : %s, file: %s, line: %d\r\n";

/* exception information area */
static exception_info_t exception_info = {0};
exception_info_t *exception_info_pointer = &exception_info;

/* exception context area */
exception_context_t exception_context = {0};
exception_context_t *exception_context_pointer = &exception_context;

static uint32_t primask_backup_assert = 0;

static uint32_t exception_wdt_mode = 0;

static uint32_t double_assert_flag = 0;
static uint32_t double_assert_lr = 0;

volatile exception_config_mode_t exception_config_mode = {EXCEPTION_MEMDUMP_TEXT};

/******************************************************************************/
/*            Memory Regions Definition                                       */
/******************************************************************************/
#if defined(__GNUC__)
extern unsigned int _sdata[];
extern unsigned int _edata[];
extern unsigned int _srodata[];
extern unsigned int _erodata[];
extern unsigned int _suser_heap[];
extern unsigned int _euser_heap[];
extern unsigned int _sbss[];
extern unsigned int _ebss[];

const memory_region_type memory_regions[] = {
    {"data", _sdata, _edata, 1},
    {"rodata", _srodata, _erodata, 1},
    {"user_heap", _suser_heap, _euser_heap, 1},
    {"bss", _sbss, _ebss, 1},
    {0}
};
#endif /* __GNUC__ */

/* Private functions ---------------------------------------------------------*/
/******************************************************************************/
/*            Exception's wdt timeout Functions                               */
/******************************************************************************/
void exception_feed_wdt(void)
{
#ifdef WDT_ENABLE
#endif
}

void exception_enable_wdt_reset(void)
{
#ifdef WDT_ENABLE
#endif
    exception_wdt_mode = EXCEPTION_WDT_RESET;
}

void exception_disable_wdt_reset(void)
{
#ifdef WDT_ENABLE
#endif
}

void exception_enable_wdt_interrupt(void)
{
    if ((exception_wdt_mode != EXCEPTION_WDT_INTERRUPT) && (exception_info.reason != EXCEPTION_NMI)) {
        exception_wdt_mode = EXCEPTION_WDT_INTERRUPT;
    }
}

/******************************************************************************/
/*            Exception's assert Functions                                    */
/******************************************************************************/
void abort(void)
{
    static uint32_t primask_backup_abort = 0;

    primask_backup_abort = __get_PRIMASK();
    __disable_irq();

    SCB->CCR |=  SCB_CCR_UNALIGN_TRP_Msk;
    *((volatile unsigned int *) 0xFFFFFFF1) = 1;
    for (;;);

    /* Just to avoid compiler warnings. */
    (void) primask_backup_abort;
}

void platform_assert(const char *expr, const char *file, int line)
{
    primask_backup_assert = __get_PRIMASK();
    __disable_irq();

    SCB->CCR |=  SCB_CCR_UNALIGN_TRP_Msk;
    assert_expr.is_valid = 1;
    assert_expr.expr = expr;
    assert_expr.file = file;
    assert_expr.line = line;
    *((volatile unsigned int *) 0xFFFFFFF1) = 1;

    while(1);

    /* Just to avoid compiler warnings. */
    (void) primask_backup_assert;
}

void exception_get_assert_expr(const char **expr, const char **file, int *line)
{
    if (assert_expr.is_valid) {
        *expr = assert_expr.expr;
        *file = assert_expr.file;
        *line = assert_expr.line;
    } else {
        *expr = NULL;
        *file = NULL;
        *line = 0;
    }
}

void exception_print_assert_info(void)
{
    if (assert_expr.is_valid == 1) {
        exception_printf("CM4 assert failed: %s, file: %s, line: %d\r\n",
                         assert_expr.expr,
                         assert_expr.file,
                         (int)assert_expr.line);
    } else if (assert_expr.is_valid == 2) {

    }
}

void exception_dump_config(int flag)
{
    reboot_flag = flag;
}

static int reboot_check(void)
{
    return reboot_flag;
}

void exception_reboot(void)
{
    NVIC_SystemReset();
}

/******************************************************************************/
/*            Exception's Dump Mode Functions                        */
/******************************************************************************/
int exception_dump_config_init(void)
{
    return 0;
}

void exception_dump_config_check(void)
{
	uint8_t mode = 0;

	/* check mode */
	if (exception_config_mode.exception_mode == 0) {
		/* error status, exception_dump_mode should not be 0 */
		exception_config_mode.exception_mode_t.reset_after_dump = 1;
		exception_config_mode.exception_mode_t.exception_fulldump_binary = 1;
	}

	mode = (uint8_t)(exception_config_mode.exception_mode & 0xff);
	if ((mode != (uint8_t)((exception_config_mode.exception_mode >>  8) & 0xff)) ||
			(mode != (uint8_t)((exception_config_mode.exception_mode >> 16) & 0xff)) ||
			(mode != (uint8_t)((exception_config_mode.exception_mode >> 24) & 0xff))) {
		/* error status, exception_dump_mode should be a 4 Byets-repeated data*/
		exception_config_mode.exception_mode_t.reset_after_dump = 1;
		exception_config_mode.exception_mode_t.exception_fulldump_binary = 1;
	}
}

/******************************************************************************/
/*            Exception's regitser callbacks Functions                        */
/******************************************************************************/
exception_status_t exception_register_callbacks(exception_config_type *cb)
{
    int i;

    if (exception_config.items >= EXCEPTION_CONFIGURATIONS_MAX) {
        return EXCEPTION_STATUS_ERROR;
    } else {
        /* check if it is already registered */
        for (i = 0; i < exception_config.items; i++) {
            if (exception_config.configs[i].init_cb == cb->init_cb
                    && exception_config.configs[i].dump_cb == cb->dump_cb) {
                return EXCEPTION_STATUS_ERROR;
            }
        }
        exception_config.configs[exception_config.items].init_cb = cb->init_cb;
        exception_config.configs[exception_config.items].dump_cb = cb->dump_cb;
        exception_config.items++;
        return EXCEPTION_STATUS_OK;
    }
}

exception_status_t exception_register_regions(memory_region_type *region)
{
    if (exception_user_regions.items >= EXCEPTION_CONFIGURATIONS_MAX) {
        return EXCEPTION_STATUS_ERROR;
    } else {
        exception_user_regions.regions[exception_user_regions.items].region_name = region->region_name;
        exception_user_regions.regions[exception_user_regions.items].start_address = region->start_address;
        exception_user_regions.regions[exception_user_regions.items].end_address = region->end_address;
        exception_user_regions.regions[exception_user_regions.items].is_dumped = region->is_dumped;
        exception_user_regions.items++;
        return EXCEPTION_STATUS_OK;
    }
}

/******************************************************************************/
/*            Exception's init Functions                                      */
/******************************************************************************/
void exception_init(void)
{
    /*enable unalign access,it will not trap when align access occurs */
    SCB->CCR &= ~SCB_CCR_UNALIGN_TRP_Msk;

    /* enable wdt reset mode for prevent exception flow hang */
    exception_enable_wdt_reset();

    // /* enable exception log service */
    // exception_log_service_init();

    /* show exception mode */
    exception_printf("exception_mode:0x%x\r\n", exception_config_mode.exception_mode);
    /* show pc */
    exception_printf("cm4 pc:0x%x\r\n", exception_context.pc);
    /* show lr */
    exception_printf("cm4 lr:0x%x\r\n", exception_context.lr);

#if 0
    /* check exception dump configuration if is ok */
    exception_dump_config_check();
#endif

    /* Update Exception Number */
    exception_info.count += 1;

    /* Get current time stamp */
    exception_info.timestamp = HAL_GetTick();

    if (exception_config_mode.exception_mode_t.reset_after_dump == 1) {
        exception_dump_config(DISABLE_WHILELOOP_MAGIC);
    }

}

/******************************************************************************/
/*            Exception's context dump Functions                              */
/******************************************************************************/
static void exception_dump_context(uint32_t stack[])
{
    /* Context Dump */
    exception_context.sp   = ((uint32_t)stack) + 0x20;
    /* FPU context? */
    if ((exception_context.exc_return & 0x10) == 0) {
        exception_context.control |= 0x4; /* CONTROL.FPCA */
        exception_context.s0 = stack[s0];
        exception_context.s1 = stack[s1];
        exception_context.s2 = stack[s2];
        exception_context.s3 = stack[s3];
        exception_context.s4 = stack[s4];
        exception_context.s5 = stack[s5];
        exception_context.s6 = stack[s6];
        exception_context.s7 = stack[s7];
        exception_context.s8 = stack[s8];
        exception_context.s9 = stack[s9];
        exception_context.s10 = stack[s10];
        exception_context.s11 = stack[s11];
        exception_context.s12 = stack[s12];
        exception_context.s13 = stack[s13];
        exception_context.s14 = stack[s14];
        exception_context.s15 = stack[s15];
        exception_context.fpscr = stack[fpscr];
        exception_context.sp += 72; /* s0-s15, fpsr, reserved */
    }

    /* if CCR.STKALIGN=1, check PSR[9] to know if there is forced stack alignment */
    if ((SCB->CCR & SCB_CCR_STKALIGN_Msk) && (exception_context.psr & 0x200)) {
        exception_context.sp += 4;
    }

    /* update CONTROL.SPSEL and psp if returning to thread mode */
    if (exception_context.exc_return & 0x4) {
        exception_context.control |= 0x2; /* CONTROL.SPSel */
        exception_context.psp = exception_context.sp;
    } else { /* update msp if returning to handler mode */
        exception_context.msp = exception_context.sp;
    }

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT)
    if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT) {
        /* feed wdt to keep time for context dump */
        exception_feed_wdt();

        exception_printf("\r\nCM4 Register dump begin:\r\n");
        exception_printf("r0  = 0x%08x\r\n", exception_context.r0);
        exception_printf("r1  = 0x%08x\r\n", exception_context.r1);
        exception_printf("r2  = 0x%08x\r\n", exception_context.r2);
        exception_printf("r3  = 0x%08x\r\n", exception_context.r3);
        exception_printf("r4  = 0x%08x\r\n", exception_context.r4);
        exception_printf("r5  = 0x%08x\r\n", exception_context.r5);
        exception_printf("r6  = 0x%08x\r\n", exception_context.r6);
        exception_printf("r7  = 0x%08x\r\n", exception_context.r7);
        exception_printf("r8  = 0x%08x\r\n", exception_context.r8);
        exception_printf("r9  = 0x%08x\r\n", exception_context.r9);
        exception_printf("r10 = 0x%08x\r\n", exception_context.r10);
        exception_printf("r11 = 0x%08x\r\n", exception_context.r11);
        exception_printf("r12 = 0x%08x\r\n", exception_context.r12);
        exception_printf("lr  = 0x%08x\r\n", exception_context.lr);
        exception_printf("pc  = 0x%08x\r\n", exception_context.pc);
        exception_printf("xpsr = 0x%08x\r\n", exception_context.psr);
        exception_printf("EXC_RET = 0x%08x\r\n", exception_context.exc_return);

        /* FPU context? */
        if ((exception_context.exc_return & 0x10) == 0) {
            exception_printf("s0  = 0x%08x\r\n", exception_context.s0);
            exception_printf("s1  = 0x%08x\r\n", exception_context.s1);
            exception_printf("s2  = 0x%08x\r\n", exception_context.s2);
            exception_printf("s3  = 0x%08x\r\n", exception_context.s3);
            exception_printf("s4  = 0x%08x\r\n", exception_context.s4);
            exception_printf("s5  = 0x%08x\r\n", exception_context.s5);
            exception_printf("s6  = 0x%08x\r\n", exception_context.s6);
            exception_printf("s7  = 0x%08x\r\n", exception_context.s7);
            exception_printf("s8  = 0x%08x\r\n", exception_context.s8);
            exception_printf("s9  = 0x%08x\r\n", exception_context.s9);
            exception_printf("s10 = 0x%08x\r\n", exception_context.s10);
            exception_printf("s11 = 0x%08x\r\n", exception_context.s11);
            exception_printf("s12 = 0x%08x\r\n", exception_context.s12);
            exception_printf("s13 = 0x%08x\r\n", exception_context.s13);
            exception_printf("s14 = 0x%08x\r\n", exception_context.s14);
            exception_printf("s15 = 0x%08x\r\n", exception_context.s15);
            exception_printf("s16 = 0x%08x\r\n", exception_context.s16);
            exception_printf("s17 = 0x%08x\r\n", exception_context.s17);
            exception_printf("s18 = 0x%08x\r\n", exception_context.s18);
            exception_printf("s19 = 0x%08x\r\n", exception_context.s19);
            exception_printf("s20 = 0x%08x\r\n", exception_context.s20);
            exception_printf("s21 = 0x%08x\r\n", exception_context.s21);
            exception_printf("s22 = 0x%08x\r\n", exception_context.s22);
            exception_printf("s23 = 0x%08x\r\n", exception_context.s23);
            exception_printf("s24 = 0x%08x\r\n", exception_context.s24);
            exception_printf("s25 = 0x%08x\r\n", exception_context.s25);
            exception_printf("s26 = 0x%08x\r\n", exception_context.s26);
            exception_printf("s27 = 0x%08x\r\n", exception_context.s27);
            exception_printf("s28 = 0x%08x\r\n", exception_context.s28);
            exception_printf("s29 = 0x%08x\r\n", exception_context.s29);
            exception_printf("s30 = 0x%08x\r\n", exception_context.s30);
            exception_printf("s31 = 0x%08x\r\n", exception_context.s31);
            exception_printf("fpscr = 0x%08x\r\n", exception_context.fpscr);
        }

        exception_printf("CONTROL = 0x%08x\r\n", exception_context.control);
        exception_printf("MSP     = 0x%08x\r\n", exception_context.msp);
        exception_printf("PSP     = 0x%08x\r\n", exception_context.psp);
        exception_printf("sp      = 0x%08x\r\n", exception_context.sp);
        exception_printf("basepri = 0x%08x\r\n", exception_context.basepri);
        exception_printf("primask = 0x%08x\r\n", exception_context.primask);
        exception_printf("faultmask = 0x%08x\r\n", exception_context.faultmask);
        exception_printf("\r\nCM4 Register dump end:\r\n");
    }
#endif /* EXCEPTION_MEMDUMP_MODE */
}

/******************************************************************************/
/*            Exception's memory dump Functions                               */
/******************************************************************************/
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT)
static void exception_dump_region_info(const memory_region_type *static_regions, exception_user_regions_t *user_regions)
{
    uint32_t i = 0;
    unsigned int *current, *end;

    /* static regions */
    for (i = 0; ; i++) {
        if (!static_regions[i].region_name) {
            break;
        }
        if (!static_regions[i].is_dumped) {
            continue;
        }
        current = (unsigned int *)((uint32_t)(static_regions[i].start_address) & 0xfffffffc);
        end     = (unsigned int *)(static_regions[i].end_address);
        if (current < end) {
            /* feed wdt to keep time for output data */
            exception_feed_wdt();
            /* output region's information */
            exception_printf("Region-%s: start_address = 0x%x, end_address = 0x%x\r\n", static_regions[i].region_name, (unsigned int)current, (unsigned int)end);
        }
    }

    /* dynamic regions */
    for (i = 0; ((i < user_regions->items) && (i < EXCEPTION_CONFIGURATIONS_MAX)); i++) {
        if ((user_regions->regions)[i].is_dumped) {
            current = (unsigned int *)((uint32_t)((user_regions->regions)[i].start_address) & 0xfffffffc);
            end     = (unsigned int *)((user_regions->regions)[i].end_address);
            if (current < end) {
                /* feed wdt to keep time for output data */
                exception_feed_wdt();
                /* output region's information */
                exception_printf("Region-%s: start_address = 0x%x, end_address = 0x%x\r\n", (user_regions->regions)[i].region_name, (unsigned int)current, (unsigned int)end);
            }
        }
    }
}
#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT)
static void exception_dump_region_data_text(const memory_region_type *static_regions, exception_user_regions_t *user_regions)
{
    uint32_t i = 0;
    unsigned int *current, *end;

    /* static regions */
    for (i = 0; ; i++) {
        if (!static_regions[i].region_name) {
            break;
        }
        if (!static_regions[i].is_dumped) {
            continue;
        }
        current = (unsigned int *)((uint32_t)(static_regions[i].start_address) & 0xfffffffc);
        end     = (unsigned int *)(static_regions[i].end_address);
        for (; current < end; current += 4) {
            /*if (*(current + 0) == 0 &&
                    *(current + 1) == 0 &&
                    *(current + 2) == 0 &&
                    *(current + 3) == 0) {
                continue;
            }*/
            /* feed wdt to keep time for output data */
            exception_feed_wdt();
            /* output data */
            exception_printf("0x%08x: %08x %08x %08x %08x\r\n",
                             (unsigned int)current,
                             *(current + 0),
                             *(current + 1),
                             *(current + 2),
                             *(current + 3));
        }
    }

    /* dynamic regions */
    for (i = 0; ((i < user_regions->items) && (i < EXCEPTION_CONFIGURATIONS_MAX)); i++) {
        if ((user_regions->regions)[i].is_dumped) {
            current = (unsigned int *)((uint32_t)((user_regions->regions)[i].start_address) & 0xfffffffc);
            end     = (unsigned int *)((user_regions->regions)[i].end_address);
            for (; current < end; current += 4) {
                /*if (*(current + 0) == 0 &&
                        *(current + 1) == 0 &&
                        *(current + 2) == 0 &&
                        *(current + 3) == 0) {
                    continue;
                }*/
                /* feed wdt to keep time for output data */
                exception_feed_wdt();
                /* output data */
                exception_printf("0x%08x: %08x %08x %08x %08x\r\n",
                                 (unsigned int)current,
                                 *(current + 0),
                                 *(current + 1),
                                 *(current + 2),
                                 *(current + 3));
            }
        }
    }
}
#endif /* EXCEPTION_MEMDUMP_MODE */

static void exception_dump_memory(void)
{
    /* Memory Dump */
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT)
    if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT) {

        /* feed wdt to keep time for memory dump */
        exception_feed_wdt();

        /* Print Regions' information */
        exception_printf("CM4 Regions Information:\r\n");
        exception_dump_region_info(memory_regions, &exception_user_regions);

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT)
        if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT)
            /* Print Regions' data */
        {
            exception_dump_region_data_text(memory_regions, &exception_user_regions);
        }
#endif /* EXCEPTION_MEMDUMP_MODE */

        /* dump cm4 end log */
        exception_printf("\r\nmemory dump completed.\r\n");
    }
#endif /* EXCEPTION_MEMDUMP_MODE */
}

/******************************************************************************/
/*            Exception's print fault type Functions                          */
/******************************************************************************/
static void printUsageErrorMsg(uint32_t CFSRValue)
{
    exception_printf("Usage fault: ");
    CFSRValue >>= 16; /* right shift to lsb */
    if ((CFSRValue & (1 << 9)) != 0) {
        exception_printf("Divide by zero\r\n");
    }
    if ((CFSRValue & (1 << 8)) != 0) {
        exception_printf("Unaligned access\r\n");
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        exception_printf("Coprocessor error\r\n");
    }
    if ((CFSRValue & (1 << 2)) != 0) {
        exception_printf("Invalid EXC_RETURN\r\n");
    }
    if ((CFSRValue & (1 << 1)) != 0) {
        exception_printf("Invalid state\r\n");
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        exception_printf("Undefined instruction\r\n");
    }
}

static void printMemoryManagementErrorMsg(uint32_t CFSRValue)
{
    exception_printf("Memory Management fault: ");
    CFSRValue &= 0x000000FF; /* mask mem faults */
    if ((CFSRValue & (1 << 5)) != 0) {
        exception_printf("A MemManage fault occurred during FP lazy state preservation\r\n");
    }
    if ((CFSRValue & (1 << 4)) != 0) {
        exception_printf("A derived MemManage fault occurred on exception entry\r\n");
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        exception_printf("A derived MemManage fault occurred on exception return\r\n");
    }
    if ((CFSRValue & (1 << 1)) != 0) { /* Need to check valid bit (bit 7 of CFSR)? */
        exception_printf("Data access violation @0x%08x\r\n", (unsigned int)SCB->MMFAR);
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        exception_printf("MPU or Execute Never (XN) default memory map access violation\r\n");
    }
    if ((CFSRValue & (1 << 7)) != 0) { /* To review: remove this if redundant */
        exception_printf("SCB->MMFAR = 0x%08x\r\n", (unsigned int)SCB->MMFAR);
    }
}

static void printBusFaultErrorMsg(uint32_t CFSRValue)
{
    exception_printf("Bus fault: ");
    CFSRValue &= 0x0000FF00; /* mask bus faults */
    CFSRValue >>= 8;
    if ((CFSRValue & (1 << 5)) != 0) {
        exception_printf("A bus fault occurred during FP lazy state preservation\r\n");
    }
    if ((CFSRValue & (1 << 4)) != 0) {
        exception_printf("A derived bus fault has occurred on exception entry\r\n");
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        exception_printf("A derived bus fault has occurred on exception return\r\n");
    }
    if ((CFSRValue & (1 << 2)) != 0) {
        exception_printf("Imprecise data access error has occurred\r\n");
    }
    if ((CFSRValue & (1 << 1)) != 0) { /* Need to check valid bit (bit 7 of CFSR)? */
        exception_printf("A precise data access error has occurred @x%08x\r\n", (unsigned int)SCB->BFAR);
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        exception_printf("A bus fault on an instruction prefetch has occurred\r\n");
    }
    if ((CFSRValue & (1 << 7)) != 0) { /* To review: remove this if redundant */
        exception_printf("SCB->BFAR = 0x%08x\r\n", (unsigned int)SCB->BFAR);
    }
}
/******************************************************************************/
/*            Exception's dump processor Functions                            */
/******************************************************************************/
void exception_dump_preprocess(uint32_t fault_type)
{
    exception_info.reason = fault_type;
    exception_info.assert_expr = &assert_expr;

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT)
    if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT) {

        /* Genie start message */
        exception_printf("<<<<<<<< LOG START LOG START LOG START LOG START LOG START <<<<<<<<\r\n");
    }
#endif
    /* feed wdt to keep time for preprocess */
    exception_feed_wdt();

    exception_printf("\r\nCM4 Fault Dump:\r\n");
    exception_print_assert_info();
    exception_printf("Exception Count = 0x%08x\r\n", (unsigned int)exception_info.count);
    exception_printf("Exception Time = 0x%08x\r\n", (unsigned int)exception_info.timestamp_32k);

    switch (exception_info.reason) {
        case EXCEPTION_NMI:
            exception_printf("\r\nIn NMI Fault Handler\r\n");
            break;
        case EXCEPTION_HARD_FAULT:
            exception_printf("\r\nIn Hard Fault Handler\r\n");
            break;
        case EXCEPTION_MEMMANAGE_FAULT:
            exception_printf("\r\nIn MemManage Fault Handler\r\n");
            break;
        case EXCEPTION_BUS_FAULT:
            exception_printf("\r\nIn Bus Fault Handler\r\n");
            break;
        case EXCEPTION_USAGE_FAULT:
            exception_printf("\r\nIn Usage Fault Handler\r\n");
            break;
        case EXCEPTION_DEBUGMON_FAULT:
            exception_printf("\r\nIn Debug Monitor Fault Handler\r\n");
            break;
        default:
            exception_printf("\r\nIn Unknow Fault Handler\r\n");
            break;
    }

    exception_printf("SCB->HFSR = 0x%08x\r\n", (unsigned int)SCB->HFSR);
    exception_printf("SCB->CFSR = 0x%08x\r\n", (unsigned int)SCB->CFSR);
    if ((SCB->HFSR & (1 << 30)) != 0) {
        exception_printf("Forced Hard Fault\r\n");
    }
    if ((SCB->CFSR & 0xFFFF0000) != 0) {
        printUsageErrorMsg(SCB->CFSR);
    }
    if ((SCB->CFSR & 0x0000FF00) != 0) {
        printBusFaultErrorMsg(SCB->CFSR);
    }
    if ((SCB->CFSR & 0x000000FF) != 0) {
        printMemoryManagementErrorMsg(SCB->CFSR);
    }
}

void exception_dump_postprocess(void)
{
    uint32_t i = 0;

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT)
    if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT) {
        for (i = 0; i < exception_config.items; i++) {
            if (exception_config.configs[i].init_cb) {

                /* feed wdt to keep time for init callback */
                exception_feed_wdt();
                /* run init callback */
                exception_config.configs[i].init_cb();
            }
        }
    }
#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT)
    if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT) {
        /* Genie complete message */
        exception_printf("<<<<<<<< LOG END LOG END LOG END LOG END LOG END <<<<<<<<\r\n");
    }
#endif

}

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/
void exception_cm4_fault_handler(uint32_t stack[], uint32_t fault_type)
{
    /* dump exception time, fault type, etc */
    exception_dump_preprocess(fault_type);

    /* dump the context when the exception happens */
    exception_dump_context(stack);

    /* dump the memory */
    exception_dump_memory();

    /* finish the dump */
    exception_dump_postprocess();

    /* check if reboot */
    if (reboot_check() == DISABLE_WHILELOOP_MAGIC) {
        exception_reboot();
    } else {

        /* disable wdt reset mode for entering while loop */
        /* maybe wdt has been changed to interrupt mode for triggering the NMI interrupt */
        exception_disable_wdt_reset();

        while (1);
    }
}

