# 00-minimal

Hi anh em, nay mình sẽ giới thiệu tới anh em một chương trình bare-metal đơn giản nhất được thực hiện trên AK Embedded Base Kit (STM32L151).

## Mục tiêu

Mục đích của chương trình này nhằm chứng minh chip sống và toolchain đang hoạt động:
- Toolchain debug được
- Linker đã đặt vector table đúng addr 0x08000000 (Các bạn có thể xem trong file linker script)
- CPU reset -> đọc được SP và Reset_Handler -> trỏ tới main() được

## File

| File | Vai trò |
|-------|----------|
| `main.c` | Đây là startup code gồm có Vector table, Reset_Handler và main() |
| `stm32l151xx.ld` | Đây là linker script với kích thước Flash là 128KB (0x08000000) và SRAM là 16KB (0x20000000) |
| `Makefile` | File build và flash firmware |

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

Nếu CPU dừng lại tại main() là thành công.

```bash
st-util &
arm-none-eabi-gdb 00-minimal.elf
(gdb) target extended-remote :4242
(gdb) load
(gdb) break main
(gdb) continue
```

<table align="center">
  <tr>
    <td align="center"><img src="" alt="minimal's result" width="1000"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 1:</em></strong> Result</p>

## Contact & Support

<p style="font-size: 20px;"><strong>Cao Trong Phuoc</strong> - Software Engineer - Embedded Systems</p>

``` Note
Thank you for visiting this repository.
If you have any questions, suggestions, or feedback about this project or firmware development, feel free to contact me directly.
```

<a href="https://github.com/caotrongphuoc">
  <img src="https://img.shields.io/badge/GitHub-caotrongphuoc-181717?style=for-the-badge&logo=github&logoColor=white"/>
</a>

<a href="https://www.linkedin.com/in/cao-trong-phuoc/">
  <img src="https://img.shields.io/badge/LinkedIn-Cao%20Trong%20Phuoc-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white"/>
</a>
