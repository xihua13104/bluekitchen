.syntax unified
.cpu cortex-m4
.thumb

.global  exception_stack_pointer
.global  exception_context_pointer
.global  exception_cm4_fault_handler
.global  exception_test_handler
#if defined(MTK_MEMORY_MONITOR_ENABLE)
.global  memorymonitor_exception_enter_trace
#endif

.section  .text
.global  CommonFault_Handler
.type  CommonFault_Handler, %function
CommonFault_Handler:
    mrs r2, primask                   /* move primask to r2          */
    cpsid i                           /* disable irq                 */
    ldr r0, =exception_context_pointer
    ldr r0, [r0]                      /* r0 := exception_context_pointer*/
    tst r12, #4                       /* thread or handler mode?     */
    ite eq
    mrseq r1, msp
    mrsne r1, psp
    ldr r3, [r1, #0]                  /* read r0 from stack          */
    str r3, [r0, #0]                  /* store r0                    */
    ldr r3, [r1, #4]                  /* read r1 from stack          */
    str r3, [r0, #4]                  /* store r1                    */
    ldr r3, [r1, #8]                  /* read r2 from stack          */
    str r3, [r0, #8]                  /* store r2                    */
    ldr r3, [r1, #12]                 /* read r3 from stack          */
    str r3, [r0, #12]                 /* store r3                    */
    ldr r3, [r1, #16]                 /* read r12 from stack         */
    str r3, [r0, #48]                 /* store r12                   */
    ldr r3, [r1, #20]                 /* read lr from stack          */
    str r3, [r0, #56]                 /* store lr                    */
    ldr r3, [r1, #24]                 /* read pc from stack          */
    str r3, [r0, #60]                 /* store pc                    */
    ldr r3, [r1, #28]                 /* read xpsr from stack        */
    str r3, [r0, #64]                 /* store xpsr                  */
    add r0, r0, #16                   /* point to context.r4         */
    stmia r0!, {r4-r11}               /* store r4-r11                */
    mov r5, r12                       /* r5 := EXC_RETURN            */
    add r0, r0, #20                   /* point to context.control    */
    mrs r1, control                   /* move CONTROL to r1          */
    str r1, [r0], #4                  /* store CONTROL               */
    str r5, [r0], #4                  /* store EXC_RETURN            */
    mrs r4, msp                       /* r4 := MSP                   */
    str r4, [r0], #4                  /* store MSP                   */
    mrs r1, psp                       /* move PSP to r1              */
    str r1, [r0], #4                  /* store PSP                   */
    mrs r1, basepri                   /* move basepri to r1          */
    str r1, [r0], #4                  /* store basepri               */
    mov r1, r2                        /* move primask to r1          */
    str r1, [r0], #4                  /* store primask               */
    mrs r1, faultmask                 /* move faultmask to r1        */
    str r1, [r0]                      /* store faultmask             */
    tst r5, #0x10                     /* FPU context?                */
    itt eq
    addeq r0, r0, #68                 /* point to contex.s16         */
    vstmeq r0, {s16-s31}              /* store s16-s31               */
    ldr r3, =exception_stack_pointer
    ldr r3, [r3]                      /* r3 := exception_stack_pointer*/
    cmp r3, #0                        /* if (!exception_stack_pointer)*/
    it ne
    movne sp, r3                      /* update msp                  */
    push {lr}
    bl exception_init
    pop {lr}
    tst r5, #4                        /* thread or handler mode?     */
    ite eq
    moveq r0, r4
    mrsne r0, psp
    bx lr
.size  CommonFault_Handler, .-CommonFault_Handler

.section  .text
.weak  NMI_Handler
.type  NMI_Handler, %function
NMI_Handler:
    mov r12, lr
    bl CommonFault_Handler
    mov r1, #2
    ldr r3, =exception_cm4_fault_handler
    blx r3
.size  NMI_Handler, .-NMI_Handler

.section  .text
.global  HardFault_Handler
.type  HardFault_Handler, %function
HardFault_Handler:
    mov r12, lr
    bl CommonFault_Handler
    //bl exception_test_handler
    mov r1, #3
    ldr r3, =exception_cm4_fault_handler
    blx r3
.size  HardFault_Handler, .-HardFault_Handler

.section  .text
.weak  MemManage_Handler
.type  MemManage_Handler, %function
MemManage_Handler:
    mov r12, lr
    bl CommonFault_Handler
    mov r1, #4
    ldr r3, =exception_cm4_fault_handler
    blx r3
.size  MemManage_Handler, .-MemManage_Handler

.section  .text
.weak  BusFault_Handler
.type  BusFault_Handler, %function
BusFault_Handler:
    mov r12, lr
    bl CommonFault_Handler
    mov r1, #5
    ldr r3, =exception_cm4_fault_handler
    blx r3
.size  BusFault_Handler, .-BusFault_Handler

.section  .text
.weak  UsageFault_Handler
.type  UsageFault_Handler, %function
UsageFault_Handler:
    mov r12, lr
    bl CommonFault_Handler
    mov r1, #6
    ldr r3, =exception_cm4_fault_handler
    blx r3
.size  UsageFault_Handler, .-UsageFault_Handler

.section  .text
.weak  DebugMon_Handler
.type  DebugMon_Handler, %function
DebugMon_Handler:
#if !defined(MTK_MEMORY_MONITOR_ENABLE)
    mov r12, lr
    bl CommonFault_Handler
    mov r1, #12
    ldr r3, =exception_cm4_fault_handler
    blx r3
#else
    push {r0-r12, r14}
    tst lr, #4
    ite eq
    mrseq r0, msp
    mrsne r0, psp
    mov r1, sp
    ldr r3, =memorymonitor_exception_enter_trace
    blx r3
    cbnz r0, 1f
    pop {r0-r12, r14}
    bx r14
1:
    pop {r0-r12, r14}
    mov r12, lr
    bl CommonFault_Handler
    mov r1, #12
    ldr r3, =exception_cm4_fault_handler
    blx r3
#endif
.size  DebugMon_Handler, .-DebugMon_Handler
