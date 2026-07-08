# 01-startup-c - Bare-metal LED blink

Blink an LED on PB8 of the AK Embedded Base Kit (STM32L151CBT6). Everything is written by hand: vector table, Reset_Handler, linker script. No HAL, no CMSIS, no ST files.

Demo clip for the whole series lives in the [root README](../../README.md#demo).

## How it works

1. **Vector table** in `led_blink.c` — two words at `0x0800_0000`: the initial stack pointer and the address of `Reset_Handler`. On reset the CPU loads them into `SP` and `PC`.
2. **Reset_Handler** — zeroes `.bss`, copies `.data` from FLASH to RAM, then calls `main()`.
3. **Linker script** `stm32l151xx.ld` — declares FLASH (128 KB at `0x0800_0000`) and RAM (16 KB at `0x2000_0000`), puts `.isr_vector` at the start of FLASH, and exports the symbols the startup code needs (`_sbss`, `_ebss`, `_sdata`, `_edata`, `_sidata`).
4. **main()** — turns on the GPIOB clock in `RCC_AHBENR`, sets PB8 to output in `GPIOB_MODER`, toggles it with `GPIOB_BSRR` and a busy-loop delay.

Build / flash / debug:
```bash
make          # produce .elf and .bin
make flash    # flash by using ST-Link V2
make debug    # openocd + arm-none-eabi-gdb for debugging
```

## Meaning

The "hello world" of bare-metal. Covers what happens between reset and `main()`: vector table, stack setup, `.bss`/`.data` init, linker script, memory-mapped I/O. Every later example builds on this.
