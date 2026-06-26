# 01-blink

Hi anh em, nay mình sẽ giới thiệu tới anh em chương trình bare-metal đầu tiên — nháy LED trên AK Embedded Base Kit (STM32L151).

## Mục tiêu

Chương trình này nhằm 2 việc:

1. Chứng minh chip sống và toolchain hoạt động:

- Toolchain build được
- Linker đặt vector table đúng addr 0x08000000
- CPU reset → đọc SP + Reset_Handler → vào được main()

2. Điều khiển peripheral đầu tiên — GPIO:

- Bật clock cho GPIOB qua RCC
- Cấu hình PB8 thành output
- Toggle LED bằng BSRR

## Phần cứng

Chip: STM32L151 (128KB Flash, 16KB SRAM, Cortex-M3, no FPU)
LED: PB8 (LED_LIFE trên AK Base Kit, active-low — pin LOW thì đèn sáng)

## File

| File | Vai trò |
| bm_led_blink.h | Khai báo struct register (GPIO, RCC), base address, bit defines, board mapping (LED_PORT/LED_PIN) |
| main.c | Startup code (vector table + Reset_Handler), delay, main() — logic blink |
| stm32l151xx.ld | Đây là linker script với kích thước Flash là 128KB (0x08000000) và SRAM là 16KB (0x20000000) |
| Makefile | File build và flash firmware |

## Build và flash

Build file:

```bash
make
```
Nạp lên kit:

```bash
make flash
```

Xóa file build:

```bash
make clean
```

## Kết quả

LED_LIFE (PB8) trên kit nháy chu kỳ ~1Hz là thành công.

LED nháy được = chứng minh đồng thời rất nhiều thứ:

- Vector table OK → CPU reset đúng vào Reset_Handler
- Reset handler OK → .bss zero, .data copy, vào main()
- Memory map OK → register access không hard fault
- GPIO/RCC config OK → PB8 thực sự toggle

<table align="center">
  <tr>
    <td align="center"><img src="" alt="blink's result" width="1000"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 1:</em></strong> LED PB8 nháy ~1Hz</p>






