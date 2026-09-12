/*
 * Minimal shim to satisfy pico-sdk hardware regs headers expectation of
 * hardware/platform_defs.h. Only exposes the _u() literal-suffix macro.
 *
 * pico-sdk regs headers write constants as _u(0x1234) so the same header
 * can compile as C (u suffix) or as assembly (bare literal).
 */

#ifndef _HARDWARE_PLATFORM_DEFS_H
#define _HARDWARE_PLATFORM_DEFS_H

#ifndef _u
#if defined(__ASSEMBLER__)
#define _u(x)    x
#else
#define _u(x)    x ## u
#endif
#endif

#endif // _HARDWARE_PLATFORM_DEFS_H
