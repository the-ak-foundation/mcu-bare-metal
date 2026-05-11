# Hướng dẫn tạo âm thanh cho Zomwar Game

## Tổng quan

Buzzer trên board dùng PWM để phát âm thanh. Mỗi âm thanh là một mảng `Tone_TypeDef` gồm các cặp `{frequency, duration}`, kết thúc bằng `{0, 0}`.

```c
typedef struct {
    uint16_t frequency;  // tần số Hz
    uint8_t  duration;   // thời lượng, mỗi đơn vị ≈ 10ms
} Tone_TypeDef;
```

---

## Bước 1 — Thiết kế melody

Dùng Web Audio Grid Sequencer để tạo giai điệu, sau đó ghi lại:
- Danh sách các **tên nốt** theo thứ tự (dùng `0` cho khoảng lặng)
- **Thời lượng** tương ứng của từng nốt

Toàn bộ tên nốt từ `NOTE_B0` đến `NOTE_DS8` có trong `notes.h`. Một số nốt thường dùng:

| Tên nốt  | Tần số (Hz) | Tên nốt  | Tần số (Hz) |
|----------|------------|----------|------------|
| NOTE_C4  | 262        | NOTE_C5  | 523        |
| NOTE_E4  | 330        | NOTE_E5  | 659        |
| NOTE_G4  | 392        | NOTE_G5  | 784        |
| NOTE_A4  | 440        | NOTE_A5  | 880        |
| NOTE_B4  | 494        | NOTE_G7  | 3136       |

---

## Bước 2 — Convert sang C bằng `melody2c`

Mở `melody2c.c`, điền melody vào 2 mảng và đổi tên array:

```c
int melody[]        = { NOTE_A5, NOTE_G5, 0, NOTE_E5, 0 };
int noteDurations[] = {       2,       2, 1,       3, 1 };

// đổi tên cho phù hợp
printf("static const Tone_TypeDef tones_shoot[] = {\n");
```

Build và chạy:

```bash
make run
```

Copy output in ra rồi dán vào `buzzer.h`.

---

## Bước 3 — Thêm vào `buzzer.h`

Dán array vào `application/sources/driver/buzzer/buzzer.h`.

Ví dụ — âm thanh bắn đạn kiểu laser:

```c
// Laser "pew" — giảm dần tần số
static const Tone_TypeDef tones_shoot[] = {
    {6000, 1},
    {5000, 1},
    {4000, 1},
    {3000, 1},
    {   0, 0}   // bắt buộc — đánh dấu kết thúc
};
```

---

## Bước 4 — Gọi trong game code

```c
#include "buzzer.h"

BUZZER_PlayTones(tones_shoot);
```

Ví dụ — âm thanh bắn đạn trong `zw_game_bullet.cpp`:

```c
// ZW_GAME_BULLET_SHOOT macro
BUZZER_PlayTones(tones_shoot);
```

---

## Các ràng buộc quan trọng

| Ràng buộc | Chi tiết |
|-----------|---------|
| Tần số hợp lệ | **100 Hz – 8000 Hz** |
| Nốt đầu tiên | **Không được là silence** (`frequency = 0`) — buzzer tắt ngay lập tức |
| Kết thúc mảng | Bắt buộc có `{0, 0}` |
| Duration ngắn | 1–3 cho âm thanh game (click, shoot) |
| Duration dài | 9–27 cho nhạc nền, melody |

> **Lỗi hay gặp:** Nếu nốt đầu tiên có `frequency = 0` thì sẽ không nghe được gì. Luôn bắt đầu mảng bằng một nốt có tần số thực.

---

## Các âm thanh hiện có trong game

| Tên array           | Dùng ở đâu                        |
|---------------------|-----------------------------------|
| `tones_startup`     | Khởi động máy                     |
| `tones_3beep`       | Game over, idle                   |
| `tones_cc`          | Bắn đạn, điều hướng menu          |
| `tones_BUM`         | Zombie bị tiêu diệt               |
| `tones_USB_con`     | Cắm USB                           |
| `tones_USB_dis`     | Rút USB                           |
| `tones_SMB`         | Super Mario Bros. (demo)          |
| `tones_merryChristmas` | Merry Christmas (demo)         |

---

## Tóm tắt workflow

```
Grid Sequencer  →  melody2c.c  →  make run  →  buzzer.h  →  BUZZER_PlayTones()
```
