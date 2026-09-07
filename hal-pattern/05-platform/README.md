# 05-platform - The HAL project

This is the HAL. Folders 00 to 04 walked through the driver-access techniques on the same LED blink. Here we stop iterating on one file and start building the real thing: a layered HAL project with an API per peripheral, one instance driver per MCU peripheral, a BSP layer, and per-project config. GPIO, UART, and Timer are the three working modules under `examples/`.

The application code (`hal_entry.c`) does not depend on the MCU. Porting to a new chip is done by adding a new instance driver and binding it in `hal_data.c`; the application stays the same.

Demo clip for every learning example lives in the [root README](../../README.md#demo). The HAL project builds a real firmware and behaves the same way when it runs.

## Layout

```text
05-platform/
├── Makefile                             # build one example and target
├── README.md
├── board/
│   └── ak_base_kit/                     # board init, LED table, board.h
├── cmsis/                               # Cortex M and MCU register definitions
├── examples/
│   └── ak_base_kit/
│       └── gpio/
│           └── led_blink/
│               ├── hal_cfg/
│               │   ├── bsp/
│               │   │   ├── bsp_cfg.h         # project BSP config (assert, param check)
│               │   │   └── bsp_clock_cfg.h   # project clock tree config
│               │   └── driver/
│               │       └── stm32l1_gpio_cfg.h
│               ├── hal_gen/             # generated: hal_data.[ch], pin_data.[ch]
│               └── src/
│                   └── hal_entry.c      # application entry
├── hal/
│   ├── inc/
│   │   ├── api/                         # interfaces shared by all MCUs
│   │   └── instances/                   # peripheral instances used by applications
│   └── src/
│       ├── bsp/
│       │   └── mcu/
│       │       ├── all/                 # bsp_common, bsp_delay, bsp_io, bsp_compiler_support
│       │       └── stm32l1/             # STM32L1 clock initialization
│       └── stm32l1_gpio/                # STM32L1 GPIO driver instance
└── script/
    └── stm32l151cbtx_flash.ld           # STM32L151CB Flash and RAM layout
```

Each peripheral instance lives in its own folder under `hal/src/`, named `<mcu>_<peripheral>/`. Application code sees only the API in `hal/inc/api/`; the instance vtable in `hal_data.c` is what glues the two together.

A folder is added with its first working module. No empty placeholders.

## First target

```text
MCU     : STM32L151CBT6
Board   : AK Embedded Base Kit
LED     : PB8
Clocks  : HSE 8 MHz, PLL x12 / 3, SYSCLK 32 MHz
```

The first example uses GPIO and the BSP software delay to blink `PB8`. A new peripheral is added only with an example that can be built and tested.

## Examples

Each folder under `examples/ak_base_kit/` is one working firmware. Pick which to build with `NAME_MODULE` and `PROJECT_DIR` in the next section.

| Folder | Peripheral | Main loop |
|---|---|---|
| `gpio/led_blink` | GPIO | Toggles the LED and calls the software delay between steps. |
| `uart/hello` | UART | Writes a line over USART1 and waits between writes. |
| `timer/blink` | Timer | Empty. The TIM2 overflow interrupt toggles the LED through a callback. |

## Build

Needs the Arm GNU Toolchain (arm-none-eabi-gcc, tested with GCC 10.3). The Makefile expects it at `GCC_PATH`. Override on the command line if installed elsewhere.

Build the default LED blink firmware:

```sh
make
```

Build a different example by pointing `NAME_MODULE` and `PROJECT_DIR` at it:

```sh
make NAME_MODULE=led_blink   PROJECT_DIR=examples/ak_base_kit/gpio/led_blink
make NAME_MODULE=echo        PROJECT_DIR=examples/ak_base_kit/uart/echo
```

Output goes to `build_<NAME_MODULE>/<NAME_MODULE>.{elf,map,bin}`.

Clean:

```sh
make clean
```

## Flash

The firmware in `build_<NAME_MODULE>/<NAME_MODULE>.bin` is a standalone image linked at `0x08000000`.

> **Warning:** the AK Embedded Base Kit ships with an AK bootloader at the same flash origin. Flashing this image with SWD overwrites the bootloader. Save the bootloader image first if you want to restore it later.

Uses STM32CubeProgrammer over SWD. Default path is `$(HOME)/workspace/tools/STM32CubeProgrammer/bin`. Override with `PROGRAMER_PATH` if installed elsewhere.

```sh
make flash
make flash PROGRAMER_PATH=/opt/st/stm32cubeprog/bin
make flash APP_START_ADDR=0x08003000     # skip the AK bootloader region
```

## Debug

```sh
make debug
```

## Porting

**Add a new board on STM32L1:**

1. Create `board/<board_name>/` with `board.h`, `board_init.[ch]`, `board_leds.[ch]`.
2. Define `BOARD_<NAME>` in `board.h`.
3. Add a new example under `examples/<board_name>/<peripheral>/<example>/` with its own `hal_cfg/` and `hal_gen/`.
4. Build with `make PROJECT_DIR=examples/<board_name>/...`.

**Add a new MCU:**

1. Put the vendor CMSIS headers under `cmsis/` (or a per-MCU subfolder).
2. Add `hal/src/bsp/mcu/<mcu>/bsp_clocks.[ch]` for clock init.
3. For each peripheral you use, add `hal/src/<mcu>_<peripheral>/<mcu>_<peripheral>.c` that implements the API vtable `g_<mod>_on_<mcu>_<peripheral>`.
4. Add a linker script under `script/` for the new MCU memory map.
5. Add a startup file that calls `SystemInit` and then `main`. `main` is defined weakly in `bsp_common.c`.
6. Application code (`hal_entry.c`) does not change. Only `hal_data.c` rebinds the instance vtable to the new driver.

## References

1. [STM32L151 reference manual (RM0038)](https://www.st.com/resource/en/reference_manual/rm0038-stm32l100xx-stm32l151xx-stm32l152xx-and-stm32l162xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) - register map and peripheral behaviour.
2. [Cortex-M3 Devices Generic User Guide](https://developer.arm.com/documentation/dui0552/a/) - SysTick, NVIC, memory model.
