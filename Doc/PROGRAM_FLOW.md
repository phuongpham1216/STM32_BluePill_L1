# PROGRAM FLOW - STM32 BluePill L1

## 1. Muc tieu chuong trinh

Chuong trinh tao xung PWM va cho phep nguoi dung dieu chinh thong so bang Rotary Encoder EC11.

Man hinh GLCD ST7920 hien thi cac thong so cua xung PWM cho nguoi dung.
Du lieu trang thai duoc in ra UART de debug.

## 2. Ket noi phan cung

### 2.1 UART debug

- `PA9  -> USART1_TX`
- `PA10 -> USART1_RX`
- Baudrate: `115200`

### 2.2 PWM output (TIM1)

- `PA8 -> TIM1_CH1`
- `PA7 -> TIM1_CH1N`

Gioi han thong so:

- Frequency: `1 kHz -> 50 kHz`
- Duty: `5% -> 95%`

Gia tri mac dinh khi khoi tao:

- Frequency: `20 kHz`
- Duty: `50%`
- Trang thai output: `STOP` (chua phat PWM)

### 2.3 Rotary Encoder EC11 (TIM2 encoder)

- `PA0 -> TIM2_CH1 (CHA)`
- `PA1 -> TIM2_CH2 (CHB)`
- `PA2 -> Nut nhan tren encoder (mode)`

### 2.4 Nut Run/Stop rieng

- `PA3 -> RunSW`

### 2.5 GLCD ST7920 (SPI2)

- `PB15 -> SPI2_MOSI (DI)`
- `PB13 -> SPI2_SCK  (SCLK)`
- `PB12 -> SPI2_NSS  (CS)`
- `PB14 -> RST`

## 3. Mo hinh trang thai

He thong co 2 mode logic: thay doi bang nut nhan tren encoder PA2.

1. `MODE_FREQUENCY`: xoay encoder de doi tan so.
2. `MODE_DUTY`: xoay encoder de doi duty.

Nut nhan `PA3` cho muc tieu phat (Start), dung (Stop) xung PWM.

## 4. Luong xu ly tong the

1. Khoi tao HAL, Clock, GPIO, UART, TIM1, TIM2, SPI.
2. Khoi tao module: pwm, encoder, button, scheduler.
3. Dang ky task dinh ky:
   - `Button_Task`: 50 ms
   - `Encoder_Task`: 10 ms
   - `Debug_Task`: 50 ms
4. Vong lap vo han:
   - Goi `Scheduler_Run()`
   - Task den chu ky thi thuc thi.

## 5. Hanh vi tung task

### 5.1 Button_Task

- Doc nut mode (PA2) va nut run (PA3).
- Co debounce cho tung nut.
- Moi lan nhan PA2 hop le thi chuyen mode theo vong:
  - `FREQUENCY <-> DUTY`
- Moi lan nhan PA3 hop le thi dao trang thai:
  - `STOP -> RUN`
  - `RUN -> STOP`

### 5.2 Encoder_Task

- Doc counter TIM2 (wrap-safe 16-bit).
- Chuan hoa buoc theo EC11 (`counts per step`) de tranh nhay 2.
- Theo mode hien tai:
  - Frequency mode: `App_AdjustFrequency(step)`
  - Duty mode: `App_AdjustDuty(step)`

### 5.3 Debug_Task

- Dinh ky ~500 ms in log:
  - Mode
  - Frequency
  - Duty
  - Run state

## 6. Rule cap nhat PWM

- Khi doi frequency:
  - Tu dong tinh lai `PSC/ARR` de tranh vuot 16-bit.
  - Giu nguyen duty phan tram.
- Khi doi duty:
  - Gioi han duty trong [5..95].

## 7. Tieu chi kiem thu nhanh

1. Boot len: log dung gia tri mac dinh.
2. Bam nut mode PA2: mode chuyen dung `FREQ <-> DUTY`.
3. Xoay encoder 1 nac:
   - Frequency tang/giam 1 step dung ky vong.
   - Duty tang/giam 1% (khong nhay 2).
4. Bam PA3:
   - Lan 1: Start PWM
   - Lan 2: Stop PWM
5. Gioi han bien:
   - Frequency khong nho hon 1 kHz, khong lon hon 50 kHz.
   - Duty khong nho hon 5%, khong lon hon 95%.

## 8. Ghi chu

- Yeu cau GLCD ST7920 da duoc khai bao ket noi SPI2.
- Phan driver/GUI chi tiet cho GLCD can bo sung theo thu vien ban dang dung.
