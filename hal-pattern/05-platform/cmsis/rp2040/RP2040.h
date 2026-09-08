/*
 * RP2040 top-level device header.
 *
 * Register maps under hardware/regs/ are copied verbatim from the
 * raspberrypi/pico-sdk project and are covered by their BSD-3-Clause
 * license. See each file's header comment for the copyright notice.
 *
 * hardware/platform_defs.h in this folder is a local shim that only
 * exposes the _u() macro the regs headers expect.
 */

#ifndef __RP2040_H__
#define __RP2040_H__

#include <stdint.h>

#include "hardware/platform_defs.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sio.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/pads_bank0.h"
#include "hardware/regs/resets.h"
#include "hardware/regs/clocks.h"
#include "hardware/regs/pll.h"
#include "hardware/regs/xosc.h"
#include "hardware/regs/watchdog.h"
#include "hardware/regs/m0plus.h"

/* Raw memory-mapped register access helper. */
#define RP2040_REG(addr)    (*(volatile uint32_t *)(addr))

#endif // __RP2040_H__
