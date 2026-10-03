#include "stm32l1_gpio.h"
#include "stm32l1xx.h"

#define STM32L1_GPIO_OPEN                       (0x4750494FU)    /* "GPIO" */
#define STM32L1_GPIO_CLOSED                     (0x00000000U)

#define STM32L1_GPIO_PRV_PORT_OFFSET    (8U)
#define STM32L1_GPIO_PRV_PORT_BITS      (0xFF00U)
#define STM32L1_GPIO_PRV_PIN_BITS       (0x00FFU)
#define STM32L1_GPIO_PRV_8BIT_MASK      (0xFFU)
#define STM32L1_GPIO_PRV_16BIT_MASK     (0xFFFFU)

#define STM32L1_GPIO_PRV_MODE_MASK      (0x00000003U)
#define STM32L1_GPIO_PRV_OTYPE_MASK     (0x00000004U)
#define STM32L1_GPIO_PRV_SPEED_MASK     (0x00000018U)
#define STM32L1_GPIO_PRV_PULL_MASK      (0x00000060U)
#define STM32L1_GPIO_PRV_OUTPUT_MASK    (0x00000080U)
#define STM32L1_GPIO_PRV_AF_MASK        (0x00000F00U)

#define STM32L1_GPIO_PRV_OTYPE_OFFSET   (2U)
#define STM32L1_GPIO_PRV_SPEED_OFFSET   (3U)
#define STM32L1_GPIO_PRV_PULL_OFFSET    (5U)
#define STM32L1_GPIO_PRV_OUTPUT_OFFSET  (7U)
#define STM32L1_GPIO_PRV_AF_OFFSET      (8U)

#define STM32L1_GPIO_PRV_FIELD_MASK     (0x3U)
#define STM32L1_GPIO_PRV_AF_FIELD_MASK  (0xFU)
#define STM32L1_GPIO_PRV_AFR_PIN_COUNT  (8U)

#define STM32L1_GPIO_PRV_PORT_ADDRESS(port_number) \
	((GPIO_TypeDef *) (GPIOA_BASE + ((GPIOB_BASE - GPIOA_BASE) * (port_number))))

static void stm32l1_gpio_pins_config(hal_gpio_cfg_t const * p_cfg);
static void stm32l1_gpio_pin_config(bsp_io_port_pin_t pin, uint32_t cfg);

const hal_gpio_api_t g_gpio_on_stm32l1_gpio =
{
	.open             = STM32L1_GPIO_Open,
	.close            = STM32L1_GPIO_Close,
	.pinsCfg          = STM32L1_GPIO_PinsCfg,
	.pinCfg           = STM32L1_GPIO_PinCfg,
	.pinRead          = STM32L1_GPIO_PinRead,
	.pinWrite         = STM32L1_GPIO_PinWrite,
	.portDirectionSet = STM32L1_GPIO_PortDirectionSet,
	.portRead         = STM32L1_GPIO_PortRead,
	.portWrite        = STM32L1_GPIO_PortWrite
};

hal_err_t STM32L1_GPIO_Open(hal_gpio_ctrl_t * const p_ctrl, hal_gpio_cfg_t const * p_cfg)
{
	stm32l1_gpio_instance_ctrl_t * p_instance_ctrl = (stm32l1_gpio_instance_ctrl_t *) p_ctrl;

#if (1 == STM32L1_GPIO_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ASSERT(NULL != p_cfg);
	HAL_ASSERT(NULL != p_cfg->p_pin_cfg_data || 0 == p_cfg->number_of_pins);
	HAL_ERROR_RETURN(STM32L1_GPIO_OPEN != p_instance_ctrl->open, HAL_ERR_ALREADY_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	p_instance_ctrl->open = STM32L1_GPIO_OPEN;

	stm32l1_gpio_pins_config(p_cfg);

	return HAL_SUCCESS;
}

hal_err_t STM32L1_GPIO_Close(hal_gpio_ctrl_t * const p_ctrl)
{
	stm32l1_gpio_instance_ctrl_t * p_instance_ctrl = (stm32l1_gpio_instance_ctrl_t *) p_ctrl;

#if (1 == STM32L1_GPIO_CFG_PARAM_CHECKING_ENABLE)
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(STM32L1_GPIO_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	p_instance_ctrl->open = STM32L1_GPIO_CLOSED;

	return HAL_SUCCESS;
}

hal_err_t STM32L1_GPIO_PinsCfg(hal_gpio_ctrl_t * const p_ctrl, hal_gpio_cfg_t const * p_cfg)
{
#if (1 == STM32L1_GPIO_CFG_PARAM_CHECKING_ENABLE)
	stm32l1_gpio_instance_ctrl_t * p_instance_ctrl = (stm32l1_gpio_instance_ctrl_t *) p_ctrl;
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(STM32L1_GPIO_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
	HAL_ASSERT(NULL != p_cfg);
	HAL_ASSERT(NULL != p_cfg->p_pin_cfg_data);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	stm32l1_gpio_pins_config(p_cfg);

	return HAL_SUCCESS;
}

hal_err_t STM32L1_GPIO_PinCfg(hal_gpio_ctrl_t * const p_ctrl, bsp_io_port_pin_t pin, uint32_t cfg)
{
#if (1 == STM32L1_GPIO_CFG_PARAM_CHECKING_ENABLE)
	stm32l1_gpio_instance_ctrl_t * p_instance_ctrl = (stm32l1_gpio_instance_ctrl_t *) p_ctrl;
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(STM32L1_GPIO_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	stm32l1_gpio_pin_config(pin, cfg);

	return HAL_SUCCESS;
}

hal_err_t STM32L1_GPIO_PinRead(hal_gpio_ctrl_t * const p_ctrl,
							   bsp_io_port_pin_t         pin,
							   bsp_io_level_t     * p_pin_value)
{
#if (1 == STM32L1_GPIO_CFG_PARAM_CHECKING_ENABLE)
	stm32l1_gpio_instance_ctrl_t * p_instance_ctrl = (stm32l1_gpio_instance_ctrl_t *) p_ctrl;
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(STM32L1_GPIO_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
	HAL_ASSERT(NULL != p_pin_value);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	uint32_t port_number = ((uint32_t) pin >> STM32L1_GPIO_PRV_PORT_OFFSET) & STM32L1_GPIO_PRV_8BIT_MASK;
	uint32_t pin_number  = (uint32_t) pin & STM32L1_GPIO_PRV_PIN_BITS;

	GPIO_TypeDef * p_gpio_regs = STM32L1_GPIO_PRV_PORT_ADDRESS(port_number);

	*p_pin_value = (bsp_io_level_t) ((p_gpio_regs->IDR >> pin_number) & 1U);

	return HAL_SUCCESS;
}

hal_err_t STM32L1_GPIO_PortRead(hal_gpio_ctrl_t * const p_ctrl,
								bsp_io_port_t         port,
								hal_gpio_size_t       * p_port_value)
{
#if (1 == STM32L1_GPIO_CFG_PARAM_CHECKING_ENABLE)
	stm32l1_gpio_instance_ctrl_t * p_instance_ctrl = (stm32l1_gpio_instance_ctrl_t *) p_ctrl;
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(STM32L1_GPIO_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
	HAL_ASSERT(NULL != p_port_value);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	uint32_t port_number = ((uint32_t) port >> STM32L1_GPIO_PRV_PORT_OFFSET) & STM32L1_GPIO_PRV_8BIT_MASK;

	GPIO_TypeDef * p_gpio_regs = STM32L1_GPIO_PRV_PORT_ADDRESS(port_number);

	*p_port_value = (hal_gpio_size_t) (p_gpio_regs->IDR & STM32L1_GPIO_PRV_16BIT_MASK);

	return HAL_SUCCESS;
}

hal_err_t STM32L1_GPIO_PortWrite(hal_gpio_ctrl_t * const p_ctrl,
								 bsp_io_port_t         port,
								 hal_gpio_size_t         value,
								 hal_gpio_size_t         mask)
{
#if (1 == STM32L1_GPIO_CFG_PARAM_CHECKING_ENABLE)
	stm32l1_gpio_instance_ctrl_t * p_instance_ctrl = (stm32l1_gpio_instance_ctrl_t *) p_ctrl;
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(STM32L1_GPIO_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	hal_gpio_size_t setbits = value & mask;
	hal_gpio_size_t clrbits = (hal_gpio_size_t) (~value & mask);

	uint32_t port_number = ((uint32_t) port >> STM32L1_GPIO_PRV_PORT_OFFSET) & STM32L1_GPIO_PRV_8BIT_MASK;

	GPIO_TypeDef * p_gpio_regs = STM32L1_GPIO_PRV_PORT_ADDRESS(port_number);

	p_gpio_regs->BSRR = ((uint32_t) clrbits << 16U) | setbits;

	return HAL_SUCCESS;
}

hal_err_t STM32L1_GPIO_PinWrite(hal_gpio_ctrl_t * const p_ctrl,
								bsp_io_port_pin_t         pin,
								bsp_io_level_t       level)
{
#if (1 == STM32L1_GPIO_CFG_PARAM_CHECKING_ENABLE)
	stm32l1_gpio_instance_ctrl_t * p_instance_ctrl = (stm32l1_gpio_instance_ctrl_t *) p_ctrl;
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(STM32L1_GPIO_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
	HAL_ERROR_RETURN(level <= BSP_IO_LEVEL_HIGH, HAL_ERR_INVALID_ARGUMENT);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	hal_gpio_size_t setbits = 0U;
	hal_gpio_size_t clrbits = 0U;
	bsp_io_port_t port    = (bsp_io_port_t) (STM32L1_GPIO_PRV_PORT_BITS & (hal_gpio_size_t) pin);

	hal_gpio_size_t shift    = STM32L1_GPIO_PRV_PIN_BITS & (hal_gpio_size_t) pin;
	hal_gpio_size_t pin_mask = (hal_gpio_size_t) (1U << shift);

	if (BSP_IO_LEVEL_LOW == level)
	{
		clrbits = pin_mask;
	}
	else
	{
		setbits = pin_mask;
	}

	uint32_t port_number = ((uint32_t) port >> STM32L1_GPIO_PRV_PORT_OFFSET) & STM32L1_GPIO_PRV_8BIT_MASK;

	GPIO_TypeDef * p_gpio_regs = STM32L1_GPIO_PRV_PORT_ADDRESS(port_number);

	p_gpio_regs->BSRR = ((uint32_t) clrbits << 16U) | setbits;

	return HAL_SUCCESS;
}

hal_err_t STM32L1_GPIO_PortDirectionSet(hal_gpio_ctrl_t * const p_ctrl,
										bsp_io_port_t         port,
										hal_gpio_size_t         direction_values,
										hal_gpio_size_t         mask)
{
#if (1 == STM32L1_GPIO_CFG_PARAM_CHECKING_ENABLE)
	stm32l1_gpio_instance_ctrl_t * p_instance_ctrl = (stm32l1_gpio_instance_ctrl_t *) p_ctrl;
	HAL_ASSERT(NULL != p_instance_ctrl);
	HAL_ERROR_RETURN(STM32L1_GPIO_OPEN == p_instance_ctrl->open, HAL_ERR_NOT_OPEN);
#else
	HAL_PARAMETER_NOT_USED(p_ctrl);
#endif

	uint32_t port_number = ((uint32_t) port >> STM32L1_GPIO_PRV_PORT_OFFSET) & STM32L1_GPIO_PRV_8BIT_MASK;

	GPIO_TypeDef * p_gpio_regs = STM32L1_GPIO_PRV_PORT_ADDRESS(port_number);

	uint32_t write_value = p_gpio_regs->MODER;

	for (uint32_t pin_number = 0U; pin_number < 16U; pin_number++)
	{
		uint32_t pin_mask = 1U << pin_number;

		if (0U != (mask & pin_mask))
		{
			uint32_t field = pin_number * 2U;

			write_value &= ~(STM32L1_GPIO_PRV_FIELD_MASK << field);
			write_value |= ((direction_values & pin_mask) >> pin_number) << field;
		}
	}

	p_gpio_regs->MODER = write_value;

	return HAL_SUCCESS;
}

static void stm32l1_gpio_pins_config(hal_gpio_cfg_t const * p_cfg)
{
	for (uint16_t pin_count = 0U; pin_count < p_cfg->number_of_pins; pin_count++)
	{
		stm32l1_gpio_pin_config(p_cfg->p_pin_cfg_data[pin_count].pin,
								p_cfg->p_pin_cfg_data[pin_count].pin_cfg);
	}
}

static void stm32l1_gpio_pin_config(bsp_io_port_pin_t pin, uint32_t cfg)
{
	uint32_t port_number = ((uint32_t) pin >> STM32L1_GPIO_PRV_PORT_OFFSET) & STM32L1_GPIO_PRV_8BIT_MASK;
	uint32_t pin_number  = (uint32_t) pin & STM32L1_GPIO_PRV_PIN_BITS;

	GPIO_TypeDef * p_gpio_regs = STM32L1_GPIO_PRV_PORT_ADDRESS(port_number);

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN << port_number;
	(void) RCC->AHBENR;

	uint32_t pin_mask   = 1U << pin_number;
	uint32_t field_mask = STM32L1_GPIO_PRV_FIELD_MASK << (pin_number * 2U);
	uint32_t field      = pin_number * 2U;

	if (0U != ((cfg & STM32L1_GPIO_PRV_OUTPUT_MASK) >> STM32L1_GPIO_PRV_OUTPUT_OFFSET))
	{
		p_gpio_regs->BSRR = pin_mask;
	}
	else
	{
		p_gpio_regs->BSRR = pin_mask << 16U;
	}

	p_gpio_regs->OTYPER =
		(p_gpio_regs->OTYPER & ~pin_mask) |
		(((cfg & STM32L1_GPIO_PRV_OTYPE_MASK) >> STM32L1_GPIO_PRV_OTYPE_OFFSET) << pin_number);

	p_gpio_regs->OSPEEDR =
		(p_gpio_regs->OSPEEDR & ~field_mask) |
		(((cfg & STM32L1_GPIO_PRV_SPEED_MASK) >> STM32L1_GPIO_PRV_SPEED_OFFSET) << field);

	p_gpio_regs->PUPDR =
		(p_gpio_regs->PUPDR & ~field_mask) |
		(((cfg & STM32L1_GPIO_PRV_PULL_MASK) >> STM32L1_GPIO_PRV_PULL_OFFSET) << field);

	uint32_t af_register = pin_number / STM32L1_GPIO_PRV_AFR_PIN_COUNT;
	uint32_t af_shift    = (pin_number % STM32L1_GPIO_PRV_AFR_PIN_COUNT) * 4U;
	uint32_t af_mask     = STM32L1_GPIO_PRV_AF_FIELD_MASK << af_shift;

	p_gpio_regs->AFR[af_register] =
		(p_gpio_regs->AFR[af_register] & ~af_mask) |
		(((cfg & STM32L1_GPIO_PRV_AF_MASK) >> STM32L1_GPIO_PRV_AF_OFFSET) << af_shift);

	p_gpio_regs->MODER =
		(p_gpio_regs->MODER & ~field_mask) |
		((cfg & STM32L1_GPIO_PRV_MODE_MASK) << field);
}
