# Hướng dẫn tạo âm thanh cho Zomwar Game

## Tổng quan

Buzzer trên board dùng PWM để phát âm thanh. Mỗi âm thanh là một mảng `Tone_TypeDef` gồm các cặp `{frequency, duration}`, kết thúc bằng `{0, 0}`.

```c
typedef struct {
    uint16_t frequency;  // Hz
    uint8_t  duration;   // đơn vị ~10ms (ví dụ: 9 ≈ 90ms)
} Tone_TypeDef;
```

---

## Bước 1 — Thiết kế melody

Dùng Web Audio Grid Sequencer (hoặc bất kỳ tool nào) để tạo giai điệu, sau đó ghi lại:
- Danh sách các **tên nốt** theo thứ tự (ví dụ: `NOTE_A4`, `NOTE_C5`, `0` cho khoảng lặng)
- **Thời lượng** tương ứng của từng nốt

Bảng nốt nhạc → tần số có trong file `notes.h`:

| Tên nốt | Tần số (Hz) |
|---------|------------|
| NOTE_C4 | 262 |
| NOTE_E4 | 330 |
| NOTE_A4 | 440 |
| NOTE_C5 | 523 |
| NOTE_G7 | 3136 |
| ... | ... |

---

## Bước 2 — Convert sang C bằng `melody2c`

Mở file `melody2c.c` và điền melody vào 2 mảng:

```c
int melody[]        = { NOTE_E5, 0, NOTE_C5, NOTE_G5, 0 };
int noteDurations[] = {       3, 1,       3,       3, 1 };
```

Đổi tên output trong `printf` cho phù hợp:

```c
printf("static const Tone_TypeDef tones_my_melody[] = {\n");
```

Sau đó build và chạy:

```bash
make run
```

Copy output vào `buzzer.h`.

---

## Bước 3 — Thêm vào `buzzer.h`

Dán array vừa tạo vào `application/sources/driver/buzzer/buzzer.h`:

```c
static const Tone_TypeDef tones_my_melody[] = {
    { 659, 3},
    {   0, 1},
    { 523, 3},
    { 784, 3},
    {   0, 1},
    {   0, 0}   // bắt buộc — đánh dấu kết thúc
};
```

---

## Bước 4 — Gọi trong game code

```c
#include "buzzer.h"

BUZZER_PlayTones(tones_my_melody);
```

---

## Các ràng buộc quan trọng

| Ràng buộc | Giá trị |
|-----------|---------|
| Tần số hợp lệ | 100 Hz – 8000 Hz |
| Nốt đầu tiên | **Không được là silence** (`frequency = 0`) — buzzer sẽ tắt ngay |
| Kết thúc mảng | Bắt buộc có `{0, 0}` |
| Duration | Mỗi đơn vị ≈ 10ms. Âm thanh ngắn dùng 2–9, dài dùng 12–27 |

> **Lỗi hay gặp:** Nếu melody bắt đầu bằng nhiều nốt `{0, duration}` thì sẽ không nghe được gì vì `BUZZER_Enable` disable buzzer ngay khi gặp `frequency < 100`.

---

## Reset melody để làm lại

Xóa nội dung 2 mảng về trống:

```bash
make reset
```

---

## Tóm tắt workflow

```
Grid Sequencer  →  melody2c.c  →  make run  →  buzzer.h  →  BUZZER_PlayTones()
```
