// THIS HEADER FILE IS AUTOMATICALLY GENERATED -- DO NOT EDIT

/**
 * Copyright (c) 2021 Raspberry Pi Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
// =============================================================================
// Register block : TIMER
// Version        : 1
// Bus type       : apb
// Description    : Free-running 64-bit microsecond counter with four 32-bit
//                  match alarms. Fed by the watchdog tick (1 MHz nominal).
//                  Subset used by the HAL: latched read, per-alarm arm,
//                  interrupt enable and status.
// =============================================================================
#ifndef _HARDWARE_REGS_TIMER_H
#define _HARDWARE_REGS_TIMER_H
// =============================================================================
// Register    : TIMER_TIMEHW
// Description : Write to bits 63:32 of time (buffer for TIMELW).
#define TIMER_TIMEHW_OFFSET  _u(0x00000000)
// Register    : TIMER_TIMELW
// Description : Write low 32 bits; commits TIMEHW too.
#define TIMER_TIMELW_OFFSET  _u(0x00000004)
// Register    : TIMER_TIMEHR
// Description : Read bits 63:32 (latched by TIMELR read).
#define TIMER_TIMEHR_OFFSET  _u(0x00000008)
// Register    : TIMER_TIMELR
// Description : Latched read of bits 31:0.
#define TIMER_TIMELR_OFFSET  _u(0x0000000c)
// Register    : TIMER_ALARM0..3
// Description : Match value for alarm n. Writing arms it.
#define TIMER_ALARM0_OFFSET  _u(0x00000010)
#define TIMER_ALARM1_OFFSET  _u(0x00000014)
#define TIMER_ALARM2_OFFSET  _u(0x00000018)
#define TIMER_ALARM3_OFFSET  _u(0x0000001c)
// Register    : TIMER_ARMED
// Description : 1 bit per alarm. Write 1 to force disarm without a match.
#define TIMER_ARMED_OFFSET   _u(0x00000020)
// Register    : TIMER_TIMERAWH / TIMERAWL
// Description : Unlatched read of upper / lower half (no side effects).
#define TIMER_TIMERAWH_OFFSET _u(0x00000024)
#define TIMER_TIMERAWL_OFFSET _u(0x00000028)
// Register    : TIMER_INTR / INTE / INTF / INTS
// Description : Interrupt raw status, enable, force, masked status. 1 bit per alarm.
#define TIMER_INTR_OFFSET    _u(0x00000034)
#define TIMER_INTE_OFFSET    _u(0x00000038)
#define TIMER_INTF_OFFSET    _u(0x0000003c)
#define TIMER_INTS_OFFSET    _u(0x00000040)
#define TIMER_ALARM_0_BITS   _u(0x00000001)
#define TIMER_ALARM_1_BITS   _u(0x00000002)
#define TIMER_ALARM_2_BITS   _u(0x00000004)
#define TIMER_ALARM_3_BITS   _u(0x00000008)
// =============================================================================
#endif // _HARDWARE_REGS_TIMER_H
