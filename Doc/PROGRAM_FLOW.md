# PROGRAM FLOW - STM32 BluePill L1

## 1. Muc tieu chuong trinh

Chuong trinh tao xung PWM va cho phep nguoi dung dieu chinh thong so bang Rotary Encoder EC11.

Man hinh GLCD ST7920 hien thi thong tin:
- Line 1: Frequency
- Line 2: Duty
- Line 3: Mode
- Line 4: PWM ON/OFF

Trang thai he thong duoc in ra UART de debug.

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

Gia tri mac dinh:
- Frequency: `20 kHz`
- Duty: `50%`
- Run state: `STOP` (khong phat xung khi boot)

### 2.3 Rotary Encoder EC11 (TIM2 encoder)

- `PA0 -> TIM2_CH1 (CHA)`
- `PA1 -> TIM2_CH2 (CHB)`
- `PA2 -> Nut nhan mode`

### 2.4 Nut Run/Stop rieng

- `PA3 -> RunSW`

### 2.5 GLCD ST7920 (SPI2)

- `PB15 -> SPI2_MOSI (DI)`
- `PB13 -> SPI2_SCK  (SCLK)`
- `PB12 -> SPI2_NSS  (CS)`
- `PB14 -> RST`

## 3. Mo hinh trang thai

He thong co 2 mode:
1. `MODE_FREQUENCY`: xoay encoder de doi frequency.
2. `MODE_DUTY`: xoay encoder de doi duty.

Nut `PA3` dung de toggle RUN/STOP PWM.

## 4. Flow chart tong the

```text
+-----------------------+
| Power on / Reset MCU  |
+-----------+-----------+
            |
            v
+-----------------------+
| HAL_Init + Clock      |
| GPIO/UART/TIM/SPI init|
+-----------+-----------+
            |
            v
+-----------------------+
| App/Driver/Module init|
| - PWM_Init            |
| - Encoder_Init        |
| - Button_Init         |
| - Display_Init        |
| - Scheduler_AddTask   |
+-----------+-----------+
            |
            v
+-----------------------+
| while(1)              |
|   Scheduler_Run()     |
+-----------+-----------+
            |
            v
+-----------------------+
| Task den chu ky thi    |
| duoc goi thuc thi      |
| - Button_Task          |
| - Encoder_Task         |
| - Debug_Task           |
| - Display_Task         |
+-----------------------+
```

## 5. Flow chi tiet theo task

### 5.1 Button_Task (chu ky 50 ms)

```text
Doc PA2, PA3 -> detect canh nhan + debounce
  |
  +-- PA2 nhan hop le -> doi mode FREQ <-> DUTY
  |
  +-- PA3 nhan hop le -> App_SetRun(!app.pwm_running)
                          -> PWM_Start hoac PWM_Stop
```

### 5.2 Encoder_Task (chu ky 10 ms)

```text
Doc TIM2 counter (16-bit wrap-safe)
  |
  v
Tinh raw_delta = now - last
  |
  v
Cong don accum_delta
  |
  v
step = accum_delta / ENCODER_COUNTS_PER_STEP (hien tai = 4)
  |
  +-- mode FREQ -> App_AdjustFrequency(step) -> PWM_SetFrequency()
  |
  +-- mode DUTY -> App_AdjustDuty(step)      -> PWM_SetDuty()
```

### 5.3 Debug_Task (duoc scheduler goi 50 ms, tu gioi han 500 ms)

```text
Neu chua du 500 ms -> return
Neu du 500 ms -> in UART:
Mode | Frequency | Duty | Run
```

### 5.4 Display_Task (chu ky 100 ms)

```text
Neu state khong doi (mode/freq/duty/run) -> return
Neu co doi:
  tao 4 line text
  ST7920_SetCursor + ST7920_WriteString cho 4 dong
```

## 6. Luong du lieu (data flow)

```text
Nut PA2/PA3 ------> Button_Task ------> app.mode / app.pwm_running
Encoder TIM2 -----> Encoder_Task -----> app.freq / app.duty
app.* ------------> PWM driver  ------> TIM1 PWM output
app.* ------------> Display_Task ------> ST7920 (SPI2)
app.* ------------> Debug_Task --------> UART log
```

## 7. Giai thich tung file trong project (den hien tai)

## 7.1 Nhom app

- `Core/app/app.h`
  - Khai bao `AppMode_t`, `App_t`, bien global `app`.
  - API muc ung dung: `App_AdjustFrequency`, `App_AdjustDuty`, `App_SetRun`.
- `Core/app/app.c`
  - Chua state trung tam cua he thong (`app`).
  - Ap gioi han freq/duty.
  - Dieu khien start/stop PWM qua driver.

## 7.2 Nhom system

- `Core/system/scheduler.h`
  - Kieu task, cau truc task, API scheduler.
- `Core/system/scheduler.c`
  - Scheduler cooperative, quet task theo chu ky.
  - Co xu ly cap nhat `last_run` theo boi so interval de giam drift.
- `Core/system/log.h`
  - Macro `LOG(...)` in ra UART qua `printf`.

## 7.3 Nhom module (logic theo tinh nang)

- `Core/modules/button_module.h/.c`
  - Poll 2 nut PA2/PA3.
  - Debounce bang `HAL_GetTick`.
  - Chuyen mode va toggle run/stop.
- `Core/modules/encoder_module.h/.c`
  - Doc delta encoder tu driver.
  - Chuan hoa buoc EC11 (4 count/step) de tranh nhay 2 so.
  - Dieu huong dieu chinh theo mode.
- `Core/modules/pwm_module.h/.c`
  - Lop logic gon, ap cau hinh PWM tu `app`.
- `Core/modules/debug_module.h/.c`
  - In log he thong theo dinh ky 500 ms.
- `Core/modules/display_module.h/.c`
  - Tao 4 dong text hien thi.
  - Chi update man khi state doi (giam nhay man/giam tai).

## 7.4 Nhom driver (truy cap phan cung)

- `Core/drivers/pwm_driver.h/.c`
  - Cau hinh TIM1 cho PWM.
  - Tu dong tinh PSC/ARR de giu tan so trong timer 16-bit.
  - Giu duty dung ty le sau moi lan doi frequency.
  - API: `PWM_Init`, `PWM_SetFrequency`, `PWM_SetDuty`, `PWM_Start`, `PWM_Stop`.
- `Core/drivers/encoder_driver.h/.c`
  - Start TIM2 encoder mode.
  - Tra counter hien tai.
- `Core/drivers/button_driver.h/.c`
  - Cau hinh PA2, PA3 input pull-up.
  - Tra ve state da nhan (active low).
- `Core/drivers/st7920_driver.h/.c`
  - Dong goi U8g2 de dieu khien ST7920 qua SPI2.
  - Quan ly buffer text 4 dong x 21 ky tu.
  - Render toan bo frame bang `u8g2_ClearBuffer/DrawStr/SendBuffer`.
  - Callback:
    - `u8x8_gpio_and_delay`: CS/RESET/delay
    - `u8x8_spi`: truyen byte qua `HAL_SPI_Transmit`
  - Ghi chu: file dang include truc tiep mot so file `third_party/csrc/*.c`
    de tranh sua makefile generated cua CubeIDE.
- `Core/drivers/u8g2/*`
  - Header va source U8g2/U8x8 su dung cho ST7920.

## 7.5 Nhom CubeMX generated

- `Core/Src/main.c`
  - Khoi tao HAL + peripheral.
  - Goi init cac module/driver.
  - Dang ky scheduler task.
  - `while(1)` chi chay `Scheduler_Run()`.
- `Core/Src/gpio.c`, `tim.c`, `spi.c`, `usart.c`
  - Cau hinh peripheral generated.
- `Core/Inc/*.h`
  - Header generated cho peripheral.
- `Core/Src/stm32f1xx_it.c`, `stm32f1xx_hal_msp.c`, `system_stm32f1xx.c`
  - IRQ, MSP, system setup.
- `Core/Startup/startup_stm32f103c8tx.s`
  - Startup va vector table.
- `Core/Src/syscalls.c`, `sysmem.c`
  - Ho tro runtime/newlib.

## 8. Trinh tu boot den khi co PWM output

1. MCU boot -> `main()`.
2. Init HAL + clock + GPIO/UART/TIM/SPI.
3. `PWM_Init()` nap tan so/duty vao TIM1 nhung chua start.
4. User bam PA3 lan dau.
5. `Button_Task` goi `App_SetRun(1)`.
6. `App_SetRun(1)` goi `PWM_Start()`.
7. TIM1 CH1/CH1N bat dau phat xung.

## 9. Rule va gioi han dang ap dung

- Frequency clamp: `1000..50000` Hz.
- Duty clamp: `5..95` %.
- Encoder step logic: 1 nac ~ 1 buoc (sau chuan hoa 4 count/step).
- Display update theo thay doi state, khong refresh lien tuc vo ich.

## 10. Huong dan doc code theo thu tu de hoc nhanh

1. Doc `Core/Src/main.c` de thay luong tong.
2. Doc `Core/app/app.c` de hieu state trung tam.
3. Doc `Core/modules/button_module.c` + `encoder_module.c`.
4. Doc `Core/drivers/pwm_driver.c` de hieu tinh toan timer.
5. Doc `Core/modules/display_module.c` + `Core/drivers/st7920_driver.c`.
6. Cuoi cung doc `Core/system/scheduler.c` de hieu nen chay task.
