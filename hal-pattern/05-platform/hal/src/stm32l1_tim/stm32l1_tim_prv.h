#ifndef __STM32L1_TIM_PRV_H__
#define __STM32L1_TIM_PRV_H__

#define STM32L1_TIM_OPEN               (0x54494D45U)    /* "TIME" */
#define STM32L1_TIM_CLOSED             (0x00000000U)

#define STM32L1_TIM_PRV_CHANNEL_MAX    (3U)
#define STM32L1_TIM_PRV_ARR_MAX        (65535U)
#define STM32L1_TIM_PRV_PSC_MAX        (65535U)
#define STM32L1_TIM_PRV_US_PER_SEC     (1000000U)

#endif // __STM32L1_TIM_PRV_H__
