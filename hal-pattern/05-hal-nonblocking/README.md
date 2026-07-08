# 05-hal-nonblocking - HAL with non-blocking tick

Blink LED PB8 with `HAL_GetTick` instead of `HAL_Delay`.

This example keeps the same behavior as [`04-hal-blocking`](../04-hal-blocking/). The main loop is non-blocking, so it can do other work between toggles.

Demo clip for the whole series lives in the [root README](../../README.md#demo).

## Diff From 04-Hal-Blocking

Only the main loop changes:

```diff
-for (;;)
-{
-    HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
-    HAL_Delay(TICK_MS);
-}
+uint32_t last_tick = HAL_GetTick();
+
+for (;;)
+{
+    if (HAL_GetTick() - last_tick >= TICK_MS)
+    {
+        last_tick = HAL_GetTick();
+        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
+    }
+}
```

`HAL_GetTick` returns the HAL-internal `uwTick` — the same counter `HAL_Delay` reads.

## How It Works

`HAL_GetTick` reads `uwTick` and returns it.

Subtracting `last_tick` handles the 32-bit counter wrap around: unsigned subtraction stays correct across the rollover point.

The loop can do other work when the 100 ms window has not elapsed. `HAL_Delay` cannot — it spins inside HAL until the time is up.

## Build / Flash / Debug

```bash
make
make flash
make debug
```

## Meaning

`HAL_Delay` stops the CPU for 100 ms.

`HAL_GetTick` plus subtract lets other code run in the same window.

Same clock, same LED, same tick source. Different loop shape.
