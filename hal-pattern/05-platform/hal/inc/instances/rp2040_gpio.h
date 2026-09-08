#ifndef __RP2040_GPIO_H__
#define __RP2040_GPIO_H__

#include "hal_gpio_api.h"
#if __has_include("rp2040_gpio_cfg.h")
#include "rp2040_gpio_cfg.h"
#endif

HAL_HEADER

/** RP2040 GPIO private control block. */
typedef struct st_rp2040_gpio_instance_ctrl
{
    uint32_t open;
    void   * p_context;
} rp2040_gpio_instance_ctrl_t;

/** RP2040 GPIO extended configuration. Assign to hal_gpio_cfg_t::p_extend
 *  when using RP2040-specific features. Empty placeholder for now. */
typedef struct st_rp2040_gpio_extended_cfg
{
    uint32_t reserved;
} rp2040_gpio_extended_cfg_t;

/** RP2040 GPIO pin configuration options. Or these together in hal_gpio_pin_cfg_t::pin_cfg. */
typedef enum e_rp2040_gpio_cfg_options
{
    /* Function select (IO_BANK0 GPIOn_CTRL.FUNCSEL, bits 0-4). */
    RP2040_GPIO_CFG_FUNC_XIP           = 0x00000000,
    RP2040_GPIO_CFG_FUNC_SPI           = 0x00000001,
    RP2040_GPIO_CFG_FUNC_UART          = 0x00000002,
    RP2040_GPIO_CFG_FUNC_I2C           = 0x00000003,
    RP2040_GPIO_CFG_FUNC_PWM           = 0x00000004,
    RP2040_GPIO_CFG_FUNC_SIO           = 0x00000005,
    RP2040_GPIO_CFG_FUNC_PIO0          = 0x00000006,
    RP2040_GPIO_CFG_FUNC_PIO1          = 0x00000007,
    RP2040_GPIO_CFG_FUNC_CLK           = 0x00000008,
    RP2040_GPIO_CFG_FUNC_USB           = 0x00000009,
    RP2040_GPIO_CFG_FUNC_NULL          = 0x0000001F,

    /* Direction (SIO GPIO_OE, only relevant when FUNC=SIO). */
    RP2040_GPIO_CFG_DIR_INPUT          = 0x00000000,
    RP2040_GPIO_CFG_DIR_OUTPUT         = 0x00000020,

    /* Initial output level (SIO GPIO_OUT). */
    RP2040_GPIO_CFG_OUTPUT_LOW         = 0x00000000,
    RP2040_GPIO_CFG_OUTPUT_HIGH        = 0x00000040,

    /* Pull direction (PADS_BANK0 GPIOn.PUE / PDE). */
    RP2040_GPIO_CFG_PULL_NONE          = 0x00000000,
    RP2040_GPIO_CFG_PULL_UP            = 0x00000080,
    RP2040_GPIO_CFG_PULL_DOWN          = 0x00000100,

    /* Output drive strength (PADS_BANK0 GPIOn.DRIVE). */
    RP2040_GPIO_CFG_DRIVE_2MA          = 0x00000000,
    RP2040_GPIO_CFG_DRIVE_4MA          = 0x00000200,
    RP2040_GPIO_CFG_DRIVE_8MA          = 0x00000400,
    RP2040_GPIO_CFG_DRIVE_12MA         = 0x00000600,

    /* Input schmitt trigger (PADS_BANK0 GPIOn.SCHMITT). */
    RP2040_GPIO_CFG_SCHMITT_OFF        = 0x00000000,
    RP2040_GPIO_CFG_SCHMITT_ON         = 0x00000800,

    /* Slew rate (PADS_BANK0 GPIOn.SLEWFAST). */
    RP2040_GPIO_CFG_SLEW_SLOW          = 0x00000000,
    RP2040_GPIO_CFG_SLEW_FAST          = 0x00001000,

    /* Input enable (PADS_BANK0 GPIOn.IE). Required for read-back and input use. */
    RP2040_GPIO_CFG_INPUT_DISABLE      = 0x00000000,
    RP2040_GPIO_CFG_INPUT_ENABLE       = 0x00002000
} rp2040_gpio_cfg_options_t;

/** GPIO implementation for RP2040. */
extern const hal_gpio_api_t g_gpio_on_rp2040_gpio;

hal_err_t RP2040_GPIO_Open(hal_gpio_ctrl_t * const p_ctrl, hal_gpio_cfg_t const * p_cfg);
hal_err_t RP2040_GPIO_Close(hal_gpio_ctrl_t * const p_ctrl);
hal_err_t RP2040_GPIO_PinsCfg(hal_gpio_ctrl_t * const p_ctrl, hal_gpio_cfg_t const * p_cfg);
hal_err_t RP2040_GPIO_PinCfg(hal_gpio_ctrl_t * const p_ctrl, bsp_io_port_pin_t pin, uint32_t cfg);
hal_err_t RP2040_GPIO_PinRead(hal_gpio_ctrl_t * const p_ctrl,
                              bsp_io_port_pin_t       pin,
                              bsp_io_level_t        * p_pin_value);
hal_err_t RP2040_GPIO_PinWrite(hal_gpio_ctrl_t * const p_ctrl,
                               bsp_io_port_pin_t       pin,
                               bsp_io_level_t          level);
hal_err_t RP2040_GPIO_PortDirectionSet(hal_gpio_ctrl_t * const p_ctrl,
                                       bsp_io_port_t           port,
                                       hal_gpio_size_t         direction_values,
                                       hal_gpio_size_t         mask);
hal_err_t RP2040_GPIO_PortRead(hal_gpio_ctrl_t * const p_ctrl,
                               bsp_io_port_t           port,
                               hal_gpio_size_t       * p_port_value);
hal_err_t RP2040_GPIO_PortWrite(hal_gpio_ctrl_t * const p_ctrl,
                                bsp_io_port_t           port,
                                hal_gpio_size_t         value,
                                hal_gpio_size_t         mask);

HAL_FOOTER

#endif // __RP2040_GPIO_H__
