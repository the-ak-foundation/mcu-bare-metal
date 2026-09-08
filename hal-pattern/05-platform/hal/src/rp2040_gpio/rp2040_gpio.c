#include "rp2040_gpio.h"
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
#define RP2040_GPIO_PRV_INPUT_EN_MASK           (0x00002000U)

/* Per-pin register address helpers. */
#define RP2040_GPIO_PRV_IO_CTRL_ADDR(pin)       (IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + ((uint32_t)(pin) * 8U))
#define RP2040_GPIO_PRV_PAD_ADDR(pin)           (PADS_BANK0_BASE + PADS_BANK0_GPIO0_OFFSET + ((uint32_t)(pin) * 4U))

static void rp2040_gpio_pins_config(hal_gpio_cfg_t const * p_cfg);
static void rp2040_gpio_pin_config(bsp_io_port_pin_t pin, uint32_t cfg);

const hal_gpio_api_t g_gpio_on_rp2040_gpio =
{
    .open             = RP2040_GPIO_Open,
    .close            = RP2040_GPIO_Close,
    .pinsCfg          = RP2040_GPIO_PinsCfg,
    .pinCfg           = RP2040_GPIO_PinCfg,
    .pinRead          = RP2040_GPIO_PinRead,
    .pinWrite         = RP2040_GPIO_PinWrite,
    .portDirectionSet = RP2040_GPIO_PortDirectionSet,
    .portRead         = RP2040_GPIO_PortRead,
    .portWrite        = RP2040_GPIO_PortWrite
};

hal_err_t RP2040_GPIO_Open(hal_gpio_ctrl_t * const p_ctrl, hal_gpio_cfg_t const * p_cfg)
{
    rp2040_gpio_instance_ctrl_t * p_instance_ctrl = (rp2040_gpio_instance_ctrl_t *) p_ctrl;

#if (1 == RP2040_GPIO_CFG_PARAM_CHECKING_ENABLE)
    HAL_ASSERT(NULL != p_instance_ctrl);
    HAL_ASSERT(NULL != p_cfg);
    HAL_ASSERT(NULL != p_cfg->p_pin_cfg_data || 0 == p_cfg->number_of_pins);
    HAL_ERROR_RETURN(RP2040_GPIO_OPEN != p_instance_ctrl->open, HAL_ERR_ALREADY_OPEN);
#else
    HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

    p_instance_ctrl->open = RP2040_GPIO_OPEN;

    rp2040_gpio_pins_config(p_cfg);

    return HAL_SUCCESS;
}

hal_err_t RP2040_GPIO_Close(hal_gpio_ctrl_t * const p_ctrl)
{
    rp2040_gpio_instance_ctrl_t * p_instance_ctrl = (rp2040_gpio_instance_ctrl_t *) p_ctrl;

#if (1 == RP2040_GPIO_CFG_PARAM_CHECKING_ENABLE)
    HAL_ASSERT(NULL != p_instance_ctrl);
    HAL_ERROR_RETURN(RP2040_GPIO_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#else
    HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

    p_instance_ctrl->open = RP2040_GPIO_CLOSED;

    return HAL_SUCCESS;
}

hal_err_t RP2040_GPIO_PinsCfg(hal_gpio_ctrl_t * const p_ctrl, hal_gpio_cfg_t const * p_cfg)
{
#if (1 == RP2040_GPIO_CFG_PARAM_CHECKING_ENABLE)
    rp2040_gpio_instance_ctrl_t * p_instance_ctrl = (rp2040_gpio_instance_ctrl_t *) p_ctrl;
    HAL_ASSERT(NULL != p_instance_ctrl);
    HAL_ERROR_RETURN(RP2040_GPIO_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
    HAL_ASSERT(NULL != p_cfg);
    HAL_ASSERT(NULL != p_cfg->p_pin_cfg_data);
#else
    HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

    rp2040_gpio_pins_config(p_cfg);

    return HAL_SUCCESS;
}

hal_err_t RP2040_GPIO_PinCfg(hal_gpio_ctrl_t * const p_ctrl, bsp_io_port_pin_t pin, uint32_t cfg)
{
#if (1 == RP2040_GPIO_CFG_PARAM_CHECKING_ENABLE)
    rp2040_gpio_instance_ctrl_t * p_instance_ctrl = (rp2040_gpio_instance_ctrl_t *) p_ctrl;
    HAL_ASSERT(NULL != p_instance_ctrl);
    HAL_ERROR_RETURN(RP2040_GPIO_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
    HAL_ERROR_RETURN((uint32_t) pin < RP2040_GPIO_PRV_PIN_MAX, HAL_ERR_INVALID_ARGUMENT);
#else
    HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

    rp2040_gpio_pin_config(pin, cfg);

    return HAL_SUCCESS;
}

hal_err_t RP2040_GPIO_PinRead(hal_gpio_ctrl_t * const p_ctrl,
                              bsp_io_port_pin_t       pin,
                              bsp_io_level_t        * p_pin_value)
{
    HAL_PARAMETER_NOT_USED(p_ctrl);
    HAL_PARAMETER_NOT_USED(pin);
    HAL_PARAMETER_NOT_USED(p_pin_value);
    return HAL_ERR_UNSUPPORTED;
}

hal_err_t RP2040_GPIO_PinWrite(hal_gpio_ctrl_t * const p_ctrl,
                               bsp_io_port_pin_t       pin,
                               bsp_io_level_t          level)
{
    HAL_PARAMETER_NOT_USED(p_ctrl);
    HAL_PARAMETER_NOT_USED(pin);
    HAL_PARAMETER_NOT_USED(level);
    return HAL_ERR_UNSUPPORTED;
}

hal_err_t RP2040_GPIO_PortDirectionSet(hal_gpio_ctrl_t * const p_ctrl,
                                       bsp_io_port_t           port,
                                       hal_gpio_size_t         direction_values,
                                       hal_gpio_size_t         mask)
{
    HAL_PARAMETER_NOT_USED(p_ctrl);
    HAL_PARAMETER_NOT_USED(port);
    HAL_PARAMETER_NOT_USED(direction_values);
    HAL_PARAMETER_NOT_USED(mask);
    return HAL_ERR_UNSUPPORTED;
}

hal_err_t RP2040_GPIO_PortRead(hal_gpio_ctrl_t * const p_ctrl,
                               bsp_io_port_t           port,
                               hal_gpio_size_t       * p_port_value)
{
    HAL_PARAMETER_NOT_USED(p_ctrl);
    HAL_PARAMETER_NOT_USED(port);
    HAL_PARAMETER_NOT_USED(p_port_value);
    return HAL_ERR_UNSUPPORTED;
}

hal_err_t RP2040_GPIO_PortWrite(hal_gpio_ctrl_t * const p_ctrl,
                                bsp_io_port_t           port,
                                hal_gpio_size_t         value,
                                hal_gpio_size_t         mask)
{
    HAL_PARAMETER_NOT_USED(p_ctrl);
    HAL_PARAMETER_NOT_USED(port);
    HAL_PARAMETER_NOT_USED(value);
    HAL_PARAMETER_NOT_USED(mask);
    return HAL_ERR_UNSUPPORTED;
}

static void rp2040_gpio_pins_config(hal_gpio_cfg_t const * p_cfg)
{
    for (uint16_t i = 0U; i < p_cfg->number_of_pins; i++)
    {
        rp2040_gpio_pin_config(p_cfg->p_pin_cfg_data[i].pin,
                               p_cfg->p_pin_cfg_data[i].pin_cfg);
    }
}

static void rp2040_gpio_pin_config(bsp_io_port_pin_t pin, uint32_t cfg)
{
    uint32_t func    = cfg & RP2040_GPIO_PRV_FUNC_MASK;
    uint32_t is_out  = (0U != (cfg & RP2040_GPIO_PRV_DIR_MASK));
    uint32_t init_hi = (0U != (cfg & RP2040_GPIO_PRV_OUTPUT_INIT_MASK));

    /* Pad control assembly. IE kept on so read-back works even for outputs. */
    uint32_t pad = PADS_BANK0_GPIO0_IE_BITS;

    if (0U != (cfg & RP2040_GPIO_PRV_PULL_UP_MASK))
    {
        pad |= PADS_BANK0_GPIO0_PUE_BITS;
    }
    if (0U != (cfg & RP2040_GPIO_PRV_PULL_DOWN_MASK))
    {
        pad |= PADS_BANK0_GPIO0_PDE_BITS;
    }
    pad |= ((cfg & RP2040_GPIO_PRV_DRIVE_MASK) >> RP2040_GPIO_PRV_DRIVE_SHIFT) << PADS_BANK0_GPIO0_DRIVE_LSB;
    if (0U != (cfg & RP2040_GPIO_PRV_SCHMITT_MASK))
    {
        pad |= PADS_BANK0_GPIO0_SCHMITT_BITS;
    }
    if (0U != (cfg & RP2040_GPIO_PRV_SLEW_MASK))
    {
        pad |= PADS_BANK0_GPIO0_SLEWFAST_BITS;
    }

    RP2040_REG(RP2040_GPIO_PRV_PAD_ADDR(pin)) = pad;

    /* Function select: routes pad to the chosen peripheral (SIO, UART, ...). */
    RP2040_REG(RP2040_GPIO_PRV_IO_CTRL_ADDR(pin)) = func;

    /* For SIO usage, set OE and initial output level here so the pin is ready. */
    if (RP2040_GPIO_CFG_FUNC_SIO == func)
    {
        uint32_t mask = 1U << (uint32_t) pin;

        if (is_out)
        {
            if (init_hi)
            {
                RP2040_REG(SIO_BASE + SIO_GPIO_OUT_SET_OFFSET) = mask;
            }
            else
            {
                RP2040_REG(SIO_BASE + SIO_GPIO_OUT_CLR_OFFSET) = mask;
            }
            RP2040_REG(SIO_BASE + SIO_GPIO_OE_SET_OFFSET) = mask;
        }
        else
        {
            RP2040_REG(SIO_BASE + SIO_GPIO_OE_CLR_OFFSET) = mask;
        }
    }
}
