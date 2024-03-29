    
    .global  OSTCBCurPtr
    .global  OSTCBHighRdyPtr

    .global  _estack
                                    
    .global  OS_CPU_PendSVHandler

    .cpu    cortex-m3
    .fpu    softvfp
    .syntax unified
    .thumb
    .text

    .equ NVIC_INT_CTRL,0xE000ED04
    .equ NVIC_SYSPRI14,0xE000ED22
    .equ NVIC_PENDSV_PRI,0xFF
    .equ NVIC_PENDSVSET,0x10000000

    .global OSStartHighRdy
    .type OSStartHighRdy, %function
OSStartHighRdy:
    LDR     R0, =NVIC_SYSPRI14             /*; Set the PendSV exception priority*/
    LDR     R1, =NVIC_PENDSV_PRI
    STRB    R1, [R0];

    MOVS    R0, #0                         /* Set the PSP to 0 for initial context switch call*/
    MSR     PSP, R0

    LDR     R0, =NVIC_INT_CTRL             /*; Trigger the PendSV exception (causes context switch)*/
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]

    CPSIE   I                              /*; Enable interrupts at processor level*/
	
OSStartHang:
    B       OSStartHang                    /*; Should never get here*/

    .global OS_CPU_PendSVHandler
    .type OS_CPU_PendSVHandler, %function

/**
 * context switch function, this function should reconize three status:
 * 1. status start: OS just boot to last step
 * 2. OSTCBCurPtr == NULL: a task was just deleted or some status ask for 
 *    necessary context switch
 * 3. normal status: save one context and recover another context
 *
 * status 1 and 2 should jump to Handler_nosave immediately, status 3 
 * should excute all code
 */
OS_CPU_PendSVHandler:
    CPSID   I                                                  /* ; Prevent interruption during context switch*/

    MRS     R0, PSP                                            /* ; PSP is process stack pointer*/
    CBZ     R0, OS_CPU_PendSVHandler_nosave                    /* ; Skip register save the first time*/
    LDR     R1, =OSTCBCurPtr  /* ; OSTCBCurPtr->OSTCBStkPtr = SP;*/
    LDR     R1, [R1]
    CBZ     R1, OS_CPU_PendSVHandler_nosave
    LDR     R2, [R1, #4]					/* get user interrupt fun */
    CBNZ    R2, OS_CLEAR_USER_INTERRUPT_FUN

    SUBS    R0, R0, #0x20                                       /*; Save remaining regs r4-11 on process stack*/
    STM     R0, {R4-R11}

    STR     R0, [R1]   /* ; R0 is SP of process being switched out*/
                                                               
OS_CPU_PendSVHandler_nosave:

    LDR     R0, =OSTCBCurPtr
    LDR     R1, =OSTCBHighRdyPtr
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2] /*; R0 is new process SP; SP = OSTCBHighRdyPtr->StkPtr;*/
    LDM     R0, {R4-R11}                                        
    ADDS    R0, R0, #0x20

    MSR     PSP, R0
    ORR     LR, LR, #0x04
    CPSIE   I

    BX      LR

OS_CLEAR_USER_INTERRUPT_FUN:
    MOV     R2, #0
    STR	    R2, [R1, #4]
    B	    OS_CPU_PendSVHandler_nosave

    /*END*/
