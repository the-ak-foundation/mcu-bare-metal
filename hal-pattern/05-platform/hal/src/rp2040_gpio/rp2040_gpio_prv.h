#ifndef __RP2040_GPIO_PRV_H__
#define __RP2040_GPIO_PRV_H__

#include "RP2040.h"

#define RP2040_GPIO_OPEN                        (0x4750494FU)    /* "GPIO" */
#define RP2040_GPIO_CLOSED                      (0x00000000U)

#define RP2040_GPIO_PRV_PIN_MAX                 (30U)

/* Bit masks for the encoded pin_cfg u32 field. */
#define RP2040_GPIO_PRV_FUNC_MASK               (0x0000001FU)
#define RP2040_GPIO_PRV_DIR_MASK                (0x00000020U)
#define RP2040_GPIO_PRV_OUTPUT_INIT_MASK        (0x00000040U)
#define RP2040_GPIO_PRV_PULL_UP_MASK            (0x00000080U)
#define RP2040_GPIO_PRV_PULL_DOWN_MASK          (0x00000100U)
#define RP2040_GPIO_PRV_DRIVE_MASK              (0x00000600U)
#define RP2040_GPIO_PRV_DRIVE_SHIFT             (9U)
#define RP2040_GPIO_PRV_SCHMITT_MASK            (0x00000800U)
#define RP2040_GPIO_PRV_SLEW_MASK               (0x00001000U)

/* Per-pin register address helpers. */
#define RP2040_GPIO_PRV_IO_CTRL_ADDR(pin)       (IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + ((uint32_t)(pin) * 8U))
#define RP2040_GPIO_PRV_PAD_ADDR(pin)           (PADS_BANK0_BASE + PADS_BANK0_GPIO0_OFFSET + ((uint32_t)(pin) * 4U))

#endif // __RP2040_GPIO_PRV_H__
