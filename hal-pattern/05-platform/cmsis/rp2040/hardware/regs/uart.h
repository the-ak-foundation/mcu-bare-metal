// THIS HEADER FILE IS AUTOMATICALLY GENERATED -- DO NOT EDIT

/**
 * Copyright (c) 2021 Raspberry Pi Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
// =============================================================================
// Register block : UART
// Version        : 1
// Bus type       : apb
// Description    : ARM PrimeCell UART (PL011) register subset used by the HAL:
//                  data, flags, baud dividers, line control, control, and
//                  the minimum interrupt mask/status/clear set.
// =============================================================================
#ifndef _HARDWARE_REGS_UART_H
#define _HARDWARE_REGS_UART_H
// =============================================================================
// Register    : UARTDR
// Description : Data Register (8 data bits + error flags)
#define UART_UARTDR_OFFSET _u(0x00000000)
#define UART_UARTDR_DATA_BITS _u(0x000000ff)
// =============================================================================
// Register    : UARTFR
// Description : Flag Register
#define UART_UARTFR_OFFSET _u(0x00000018)
#define UART_UARTFR_TXFE_BITS _u(0x00000080)
#define UART_UARTFR_RXFF_BITS _u(0x00000040)
#define UART_UARTFR_TXFF_BITS _u(0x00000020)
#define UART_UARTFR_RXFE_BITS _u(0x00000010)
#define UART_UARTFR_BUSY_BITS _u(0x00000008)
// =============================================================================
// Register    : UARTIBRD
// Description : Integer Baud Rate Register (16-bit divisor integer part)
#define UART_UARTIBRD_OFFSET _u(0x00000024)
#define UART_UARTIBRD_BAUD_DIVINT_BITS _u(0x0000ffff)
// =============================================================================
// Register    : UARTFBRD
// Description : Fractional Baud Rate Register (6-bit divisor fractional part)
#define UART_UARTFBRD_OFFSET _u(0x00000028)
#define UART_UARTFBRD_BAUD_DIVFRAC_BITS _u(0x0000003f)
// =============================================================================
// Register    : UARTLCR_H
// Description : Line Control Register (word length, FIFO, parity, stop)
#define UART_UARTLCR_H_OFFSET _u(0x0000002c)
#define UART_UARTLCR_H_WLEN_BITS  _u(0x00000060)
#define UART_UARTLCR_H_WLEN_LSB   _u(5)
#define UART_UARTLCR_H_FEN_BITS   _u(0x00000010)
#define UART_UARTLCR_H_STP2_BITS  _u(0x00000008)
#define UART_UARTLCR_H_EPS_BITS   _u(0x00000004)
#define UART_UARTLCR_H_PEN_BITS   _u(0x00000002)
// =============================================================================
// Register    : UARTCR
// Description : Control Register
#define UART_UARTCR_OFFSET _u(0x00000030)
#define UART_UARTCR_RXE_BITS    _u(0x00000200)
#define UART_UARTCR_TXE_BITS    _u(0x00000100)
#define UART_UARTCR_UARTEN_BITS _u(0x00000001)
// =============================================================================
// Register    : UARTIMSC
// Description : Interrupt Mask Set/Clear Register
#define UART_UARTIMSC_OFFSET _u(0x00000038)
#define UART_UARTIMSC_RXIM_BITS _u(0x00000010)
#define UART_UARTIMSC_TXIM_BITS _u(0x00000020)
// =============================================================================
// Register    : UARTMIS
// Description : Masked Interrupt Status Register
#define UART_UARTMIS_OFFSET _u(0x00000040)
#define UART_UARTMIS_RXMIS_BITS _u(0x00000010)
#define UART_UARTMIS_TXMIS_BITS _u(0x00000020)
// =============================================================================
// Register    : UARTICR
// Description : Interrupt Clear Register
#define UART_UARTICR_OFFSET _u(0x00000044)
#define UART_UARTICR_RXIC_BITS _u(0x00000010)
#define UART_UARTICR_TXIC_BITS _u(0x00000020)
// =============================================================================
#endif // _HARDWARE_REGS_UART_H
