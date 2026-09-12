# 05-platform - The HAL project

This is the HAL. Folders 00 to 04 walked through the driver-access techniques on the same LED blink. Here we stop iterating on one file and start building the real thing: a layered HAL project with an API per peripheral, one instance driver per MCU peripheral, a BSP layer, and per-project config. LED blink is its first working module. Every new peripheral (UART, ADC, and so on) will come as a new example under `examples/`.

The application code (`hal_entry.c`) does not depend on the MCU. Porting to a new chip is done by adding a new instance driver and binding it in `hal_data.c`; the application stays the same.

Demo clip for every learning example lives in the [root README](../../README.md#demo). The HAL project builds a real firmware and behaves the same way when it runs.

## Layout

```text
05-platform/
├── Makefile                             # build one example and target
├── README.md
├── board/
│   ├── ak_base_kit/                     # board init, LED table, board.h (STM32L1)
│   └── raspberry_pi_pico/               # board init, LED table, board.h (RP2040)
├── cmsis/                               # Cortex M and MCU register definitions
│   └── rp2040/                          # RP2040 register + pico-sdk header subset
├── examples/
│   ├── ak_base_kit/
│   │   └── gpio/
│   │       └── led_blink/
│   │           ├── hal_cfg/
│   │           │   ├── bsp/
│   │           │   │   ├── bsp_cfg.h         # project BSP config (assert, param check)
│   │           │   │   └── bsp_clock_cfg.h   # project clock tree config
│   │           │   └── driver/
│   │           │       └── stm32l1_gpio_cfg.h
│   │           ├── hal_gen/             # generated: hal_data.[ch], pin_data.[ch]
│   │           └── src/
│   │               └── hal_entry.c      # application entry
│   └── raspberry_pi_pico/
│       └── gpio/
│           └── led_blink/               # same layout, byte-identical hal_entry.c
├── hal/
│   ├── inc/
│   │   ├── api/                         # interfaces shared by all MCUs
│   │   └── instances/                   # peripheral instances used by applications
│   └── src/
│       ├── bsp/
│       │   └── mcu/
│       │       ├── all/                 # bsp_common, bsp_delay, bsp_io, bsp_compiler_support
│       │       ├── stm32l1/             # STM32L1 clock initialization
│       │       └── rp2040/              # RP2040 clock initialization
│       ├── stm32l1_gpio/                # STM32L1 GPIO driver instance
│       └── rp2040_gpio/                 # RP2040 GPIO driver instance
└── script/
    ├── stm32l151cbtx_flash.ld           # STM32L151CB Flash and RAM layout
    └── rp2040_flash.ld                  # RP2040 external QSPI Flash and SRAM layout
```

Each peripheral instance lives in its own folder under `hal/src/`, named `<mcu>_<peripheral>/`. Application code sees only the API in `hal/inc/api/`; the instance vtable in `hal_data.c` is what glues the two together.

A folder is added with its first working module. No empty placeholders.

## Targets

```text
Target 1
  MCU     : STM32L151CBT6         (Cortex-M3)
  Board   : AK Embedded Base Kit
  LED     : PB8
  Clocks  : HSE 8 MHz, PLL x12 / 3, SYSCLK 32 MHz

Target 2
  MCU     : RP2040                (dual Cortex-M0+, core 0 only)
  Board   : Raspberry Pi Pico
  LED     : GPIO25
  Clocks  : XOSC 12 MHz, PLL_SYS x125 / 6 / 2, SYSCLK 125 MHz
```

Both targets run the same `hal_entry.c` byte-for-byte. Adding a target means adding an MCU folder, a board folder, a clock init, and an example — never touching the application source.

## Examples

Each folder under `examples/<board>/` is one working firmware. Pick which to build with `MCU`, `NAME_MODULE`, and `PROJECT_DIR` in the next section.

| Board | Folder | Peripheral | Main loop |
|---|---|---|---|
| AK Base Kit | `gpio/led_blink` | GPIO | Toggles the LED and calls the software delay between steps. |
| AK Base Kit | `uart/hello` | UART | Writes a line over USART1 and waits between writes. |
| AK Base Kit | `timer/blink` | Timer | Empty. The TIM2 overflow interrupt toggles the LED through a callback. |
| Raspberry Pi Pico | `gpio/led_blink` | GPIO | Same source as the STM32 blink, built for RP2040. |

## Build

Needs the Arm GNU Toolchain (arm-none-eabi-gcc, tested with GCC 10.3). The Makefile expects it at `GCC_PATH`. Override on the command line if installed elsewhere.

Pick the target with `MCU=`. Default is `stm32l1`.

```sh
make                       # stm32l1, led_blink for AK Base Kit
make MCU=rp2040 NAME_MODULE=led_blink PROJECT_DIR=examples/raspberry_pi_pico/gpio/led_blink
```

Build a different STM32L1 example by pointing `NAME_MODULE` and `PROJECT_DIR` at it:

```sh
make NAME_MODULE=led_blink   PROJECT_DIR=examples/ak_base_kit/gpio/led_blink
make NAME_MODULE=hello       PROJECT_DIR=examples/ak_base_kit/uart/hello
make NAME_MODULE=blink       PROJECT_DIR=examples/ak_base_kit/timer/blink
```

Output goes to `build_<NAME_MODULE>/<NAME_MODULE>.{elf,map,bin}`. For RP2040, `make uf2` also produces `<NAME_MODULE>.uf2`.

Clean:

```sh
make clean
make MCU=rp2040 NAME_MODULE=led_blink PROJECT_DIR=examples/raspberry_pi_pico/gpio/led_blink clean
```

## Flash

### STM32L1 (SWD, STM32CubeProgrammer)

The firmware in `build_<NAME_MODULE>/<NAME_MODULE>.bin` is a standalone image linked at `0x08000000`.

> **Warning:** the AK Embedded Base Kit ships with an AK bootloader at the same flash origin. Flashing this image with SWD overwrites the bootloader. Save the bootloader image first if you want to restore it later.

Default programmer path is `$(HOME)/workspace/tools/STM32CubeProgrammer/bin`. Override with `PROGRAMER_PATH` if installed elsewhere.

```sh
make flash
make flash PROGRAMER_PATH=/opt/st/stm32cubeprog/bin
make flash APP_START_ADDR=0x08003000     # skip the AK bootloader region
```

### RP2040 (UF2 mass-storage)

Hold `BOOTSEL` on the Pico while plugging in USB. The board mounts as `RPI-RP2`. Build and drop the UF2 onto it:

```sh
make MCU=rp2040 NAME_MODULE=led_blink PROJECT_DIR=examples/raspberry_pi_pico/gpio/led_blink uf2
cp build_led_blink/led_blink.uf2 /media/$USER/RPI-RP2/
```

Needs `picotool` on `PATH`. Override with `PICOTOOL=/path/to/picotool` if installed elsewhere.

### RP2040 (SWD, openocd)

With a CMSIS-DAP probe (e.g. Raspberry Pi Debug Probe) wired to `SWCLK`/`SWDIO`/`GND`:

```sh
make MCU=rp2040 NAME_MODULE=led_blink PROJECT_DIR=examples/raspberry_pi_pico/gpio/led_blink flash
make MCU=rp2040 ... flash PICO_SWD_ADAPTER=interface/raspberrypi-swd.cfg   # Pico Probe over GPIO
make MCU=rp2040 ... flash PICO_SWD_SPEED_HZ=1000                            # slower link
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
