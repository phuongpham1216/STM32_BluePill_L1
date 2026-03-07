#include "display_module.h"

#include <stdio.h>

#include "app.h"
#include "st7920_driver.h"

void Display_Init(void)
{
    ST7920_Init();
    ST7920_Clear();
}

void Display_Task(void)
{
    static uint8_t first = 1u;
    static AppMode_t last_mode = MODE_COUNT;
    static uint32_t last_freq = 0u;
    static uint16_t last_duty = 0u;
    static uint8_t last_run = 2u;

    char line1[17];
    char line2[17];

    if (!first &&
        (last_mode == app.mode) &&
        (last_freq == app.freq) &&
        (last_duty == app.duty) &&
        (last_run == app.pwm_running))
    {
        return;
    }

    (void)snprintf(line1, sizeof(line1), "F:%5luHz R:%s",
                   app.freq,
                   app.pwm_running ? "ON " : "OFF");

    (void)snprintf(line2, sizeof(line2), "D:%2u%% M:%s",
                   app.duty,
                   (app.mode == MODE_FREQUENCY) ? "FREQ" : "DUTY");

    ST7920_SetCursor(0u, 0u);
    ST7920_WriteString(line1);
    ST7920_SetCursor(1u, 0u);
    ST7920_WriteString(line2);

    last_mode = app.mode;
    last_freq = app.freq;
    last_duty = app.duty;
    last_run = app.pwm_running;
    first = 0u;
}
