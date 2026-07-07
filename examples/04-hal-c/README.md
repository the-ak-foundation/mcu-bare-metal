# 04-hal-c - HAL layer

Blink LED PB8 with SysTick timing.

This example keeps the same behavior as [`03-cmsis-c`](../03-cmsis-c/). Direct register access is replaced by HAL function calls from ST.

No ST startup files.

Demo clip for the whole series lives in the [root README](../../README.md#demo).

## Diff From 03-CMSIS-C

Clock enable, GPIO config, and SysTick setup are replaced by HAL calls:

```diff
-RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
-GPIOB->MODER |= (1U << (LED_PIN * 2));
-SysTick_Config(SYSCLK_HZ / TICK_HZ);
+HAL_Init();
+__HAL_RCC_GPIOB_CLK_ENABLE();
+GPIO_InitTypeDef gpio_init = {
+    .Pin   = LED_PIN,
+    .Mode  = GPIO_MODE_OUTPUT_PP,
+    .Pull  = GPIO_NOPULL,
+    .Speed = GPIO_SPEED_FREQ_LOW,
+};
+HAL_GPIO_Init(LED_PORT, &gpio_init);
```

`LED_PIN` is now the bitmask `GPIO_PIN_8` (`1U << 8`), not the pin index.

The main loop is replaced by `HAL_Delay`:

```diff
-uint32_t last_tick = g_tick;
-for (;;)
-{
-    if (g_tick - last_tick >= 100)
-    {
-        last_tick = g_tick;
-        GPIOB->ODR ^= (1U << LED_PIN);
-    }
-}
+for (;;)
+{
+    HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
+    HAL_Delay(TICK_MS);
+}
```

`SysTick_Handler` calls `HAL_IncTick` instead of `g_tick++`.

The Makefile adds the HAL include path and the HAL enable macro:

```diff
-CFLAGS = ... -I./cmsis -DSTM32L151xB
+CFLAGS = ... -I./cmsis -I./hal -DSTM32L151xB -DUSE_HAL_DRIVER
```

## HAL Files

The HAL sources are vendored in [`hal/`](./hal/), copied from ST's [`stm32l1xx_hal_driver`](https://github.com/STMicroelectronics/stm32l1xx_hal_driver) and [`cmsis_device_l1`](https://github.com/STMicroelectronics/cmsis_device_l1) repositories. Five `.c` files are compiled:

| File | Provides |
|------|----------|
| `stm32l1xx_hal.c` | `HAL_Init`, `HAL_Delay`, `HAL_IncTick`, `uwTick` |
| `stm32l1xx_hal_rcc.c` | RCC setup called by `HAL_Init` |
| `stm32l1xx_hal_gpio.c` | `HAL_GPIO_Init`, `HAL_GPIO_TogglePin` |
| `stm32l1xx_hal_cortex.c` | `HAL_NVIC_SetPriority` called by `HAL_Init` |
| `system_stm32l1xx.c` | `SystemCoreClock` (2 097 000 Hz, MSI default) |

Their headers, plus a few included by the RCC code (`flash`, `pwr`, `Legacy/`), are vendored next to them. `stm32l1xx_hal_conf.h` is the ST template with 25 unused modules commented out.

The CMSIS files under [`cmsis/`](./cmsis/) are the same as in `03-cmsis-c`.

## How It Works

`HAL_Init` sets the NVIC priority group and calls `HAL_InitTick`, which programs SysTick for a 1 ms tick.

`SysTick_Handler` calls `HAL_IncTick`, which increments `uwTick` inside `stm32l1xx_hal.c`.

`HAL_Delay(TICK_MS)` waits until `uwTick` has advanced `TICK_MS` ms.

## Build / Flash / Debug

```bash
make
make flash
make debug
```

## Meaning

HAL replaces direct register access with function calls.

The register writes still happen inside the HAL functions. HAL adds about 2 KB of code compared to `03-cmsis-c`.
