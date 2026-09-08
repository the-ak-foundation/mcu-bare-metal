/**
 * RP2040 startup file: second-stage boot (.boot2 = 256 B precomputed for
 * generic 03h QSPI flash), Cortex-M0+ vector table, Reset_Handler.
 *
 * The .boot2 blob is copied verbatim from the rp-rs/rp2040-boot2 project
 * (BSD-3-Clause). It configures QSPI XIP mode then jumps to Reset_Handler
 * via the vector table at 0x10000100.
 */

    .syntax unified
    .cpu cortex-m0plus
    .thumb

.global g_pfnVectors
.global Default_Handler

.word _sidata
.word _sdata
.word _edata
.word _sbss
.word _ebss

/* =========================================================================
 * .boot2 : 256 B second-stage bootloader (generic 03h serial-read flash).
 * The bootrom loads this to SRAM, checks CRC32, executes it. This code
 * programs the SSI/XIP hardware then jumps to the vector table.
 * ========================================================================= */
    .section .boot2, "ax"
    .byte 0x00, 0xb5, 0x0c, 0x4b, 0x00, 0x21, 0x99, 0x60
    .byte 0x02, 0x21, 0x59, 0x61, 0x0a, 0x49, 0x19, 0x60
    .byte 0x0a, 0x49, 0x0b, 0x48, 0x01, 0x60, 0x00, 0x21
    .byte 0x59, 0x60, 0x01, 0x21, 0x99, 0x60, 0x01, 0xbc
    .byte 0x00, 0x28, 0x00, 0xd0, 0x00, 0x47, 0x07, 0x48
    .byte 0x07, 0x49, 0x08, 0x60, 0x03, 0xc8, 0x80, 0xf3
    .byte 0x08, 0x88, 0x08, 0x47, 0x00, 0x00, 0x00, 0x18
    .byte 0x00, 0x03, 0x1f, 0x00, 0x18, 0x02, 0x00, 0x03
    .byte 0xf4, 0x00, 0x00, 0x18, 0x00, 0x01, 0x00, 0x10
    .byte 0x08, 0xed, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    .byte 0x00, 0x00, 0x00, 0x00, 0xea, 0x22, 0x4c, 0x5d

/* =========================================================================
 * Reset_Handler : run after boot2 sets VTOR and jumps here.
 * ========================================================================= */
    .section .text.Reset_Handler
    .weak Reset_Handler
    .type Reset_Handler, %function
Reset_Handler:

/* Copy the data segment initializers from flash to SRAM */
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
    movs r3, #0
    b LoopCopyDataInit

CopyDataInit:
    ldr r4, [r2, r3]
    str r4, [r0, r3]
    adds r3, r3, #4

LoopCopyDataInit:
    adds r4, r0, r3
    cmp r4, r1
    bcc CopyDataInit

/* Zero fill the bss segment. */
    ldr r2, =_sbss
    ldr r4, =_ebss
    movs r3, #0
    b LoopFillZerobss

FillZerobss:
    str  r3, [r2]
    adds r2, r2, #4

LoopFillZerobss:
    cmp r2, r4
    bcc FillZerobss

/* Call SystemInit if present (weak; empty by default on RP2040). */
    bl SystemInit

/* Call static constructors. */
    bl __libc_init_array

/* Call the board initialization function. */
    movs r0, #0
    bl bsp_init

/* Call the application's entry point. */
    bl main
    b .
.size Reset_Handler, .-Reset_Handler

/* =========================================================================
 * SystemInit : weak default (no-op). Board BSP may override.
 * ========================================================================= */
    .section .text.SystemInit
    .weak SystemInit
    .type SystemInit, %function
SystemInit:
    bx lr
.size SystemInit, .-SystemInit

/* =========================================================================
 * Default_Handler : infinite loop trap for unused vectors.
 * ========================================================================= */
    .section .text.Default_Handler, "ax", %progbits
Default_Handler:
Infinite_Loop:
    b Infinite_Loop
.size Default_Handler, .-Default_Handler

/* =========================================================================
 * Vector table : Cortex-M0+ core exceptions + RP2040 IRQs (26 external).
 * ========================================================================= */
    .section .isr_vector, "a", %progbits
    .type g_pfnVectors, %object
    .size g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word 0
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler

    /* External IRQs 0-25 */
    .word TIMER_IRQ_0_Handler
    .word TIMER_IRQ_1_Handler
    .word TIMER_IRQ_2_Handler
    .word TIMER_IRQ_3_Handler
    .word PWM_IRQ_WRAP_Handler
    .word USBCTRL_IRQ_Handler
    .word XIP_IRQ_Handler
    .word PIO0_IRQ_0_Handler
    .word PIO0_IRQ_1_Handler
    .word PIO1_IRQ_0_Handler
    .word PIO1_IRQ_1_Handler
    .word DMA_IRQ_0_Handler
    .word DMA_IRQ_1_Handler
    .word IO_IRQ_BANK0_Handler
    .word IO_IRQ_QSPI_Handler
    .word SIO_IRQ_PROC0_Handler
    .word SIO_IRQ_PROC1_Handler
    .word CLOCKS_IRQ_Handler
    .word SPI0_IRQ_Handler
    .word SPI1_IRQ_Handler
    .word UART0_IRQ_Handler
    .word UART1_IRQ_Handler
    .word ADC_IRQ_FIFO_Handler
    .word I2C0_IRQ_Handler
    .word I2C1_IRQ_Handler
    .word RTC_IRQ_Handler

/* =========================================================================
 * Weak aliases for all handlers so unused ones fall through to Default_Handler.
 * ========================================================================= */
    .weak NMI_Handler
    .thumb_set NMI_Handler, Default_Handler
    .weak HardFault_Handler
    .thumb_set HardFault_Handler, Default_Handler
    .weak SVC_Handler
    .thumb_set SVC_Handler, Default_Handler
    .weak PendSV_Handler
    .thumb_set PendSV_Handler, Default_Handler
    .weak SysTick_Handler
    .thumb_set SysTick_Handler, Default_Handler

    .weak TIMER_IRQ_0_Handler
    .thumb_set TIMER_IRQ_0_Handler, Default_Handler
    .weak TIMER_IRQ_1_Handler
    .thumb_set TIMER_IRQ_1_Handler, Default_Handler
    .weak TIMER_IRQ_2_Handler
    .thumb_set TIMER_IRQ_2_Handler, Default_Handler
    .weak TIMER_IRQ_3_Handler
    .thumb_set TIMER_IRQ_3_Handler, Default_Handler
    .weak PWM_IRQ_WRAP_Handler
    .thumb_set PWM_IRQ_WRAP_Handler, Default_Handler
    .weak USBCTRL_IRQ_Handler
    .thumb_set USBCTRL_IRQ_Handler, Default_Handler
    .weak XIP_IRQ_Handler
    .thumb_set XIP_IRQ_Handler, Default_Handler
    .weak PIO0_IRQ_0_Handler
    .thumb_set PIO0_IRQ_0_Handler, Default_Handler
    .weak PIO0_IRQ_1_Handler
    .thumb_set PIO0_IRQ_1_Handler, Default_Handler
    .weak PIO1_IRQ_0_Handler
    .thumb_set PIO1_IRQ_0_Handler, Default_Handler
    .weak PIO1_IRQ_1_Handler
    .thumb_set PIO1_IRQ_1_Handler, Default_Handler
    .weak DMA_IRQ_0_Handler
    .thumb_set DMA_IRQ_0_Handler, Default_Handler
    .weak DMA_IRQ_1_Handler
    .thumb_set DMA_IRQ_1_Handler, Default_Handler
    .weak IO_IRQ_BANK0_Handler
    .thumb_set IO_IRQ_BANK0_Handler, Default_Handler
    .weak IO_IRQ_QSPI_Handler
    .thumb_set IO_IRQ_QSPI_Handler, Default_Handler
    .weak SIO_IRQ_PROC0_Handler
    .thumb_set SIO_IRQ_PROC0_Handler, Default_Handler
    .weak SIO_IRQ_PROC1_Handler
    .thumb_set SIO_IRQ_PROC1_Handler, Default_Handler
    .weak CLOCKS_IRQ_Handler
    .thumb_set CLOCKS_IRQ_Handler, Default_Handler
    .weak SPI0_IRQ_Handler
    .thumb_set SPI0_IRQ_Handler, Default_Handler
    .weak SPI1_IRQ_Handler
    .thumb_set SPI1_IRQ_Handler, Default_Handler
    .weak UART0_IRQ_Handler
    .thumb_set UART0_IRQ_Handler, Default_Handler
    .weak UART1_IRQ_Handler
    .thumb_set UART1_IRQ_Handler, Default_Handler
    .weak ADC_IRQ_FIFO_Handler
    .thumb_set ADC_IRQ_FIFO_Handler, Default_Handler
    .weak I2C0_IRQ_Handler
    .thumb_set I2C0_IRQ_Handler, Default_Handler
    .weak I2C1_IRQ_Handler
    .thumb_set I2C1_IRQ_Handler, Default_Handler
    .weak RTC_IRQ_Handler
    .thumb_set RTC_IRQ_Handler, Default_Handler
