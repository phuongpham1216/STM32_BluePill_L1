/*
 * app.c
 *
 *  Created on: Mar 1, 2026
 *      Author: P2Lap
 */

#include "app.h"
#include "pwm_driver.h"

App_t app = {
    .mode = APP_DEFAULT_MODE,
    .freq = APP_DEFAULT_FREQ,
    .duty = APP_DEFAULT_DUTY,
    .pwm_running = 0u
};

void App_AdjustFrequency(int32_t delta)
{
    int32_t f = (int32_t)app.freq + (delta * 100);

    if (f < 1000) f = 1000;
    if (f > 50000) f = 50000;

    app.freq = (uint32_t)f;
    PWM_SetFrequency(app.freq);
}

void App_AdjustDuty(int32_t delta)
{
    int32_t d = (int32_t)app.duty + delta;

    if (d < 5) d = 5;
    if (d > 95) d = 95;

    app.duty = (uint16_t)d;
    PWM_SetDuty(app.duty);
}

void App_SetRun(uint8_t state)
{
    if (state)
    {
        PWM_SetFrequency(app.freq);
        PWM_SetDuty(app.duty);
        PWM_Start();
        app.pwm_running = 1u;
    }
    else
    {
        PWM_Stop();
        app.pwm_running = 0u;
    }
}

void App_ResetToDefault(uint8_t force_pwm_off)
{
    uint8_t was_running = app.pwm_running;

    app.mode = APP_DEFAULT_MODE;
    app.freq = APP_DEFAULT_FREQ;
    app.duty = APP_DEFAULT_DUTY;

    /* Nap lai tham so PWM ve mac dinh ngay khi reset logic. */
    PWM_SetFrequency(app.freq);
    PWM_SetDuty(app.duty);

    if (force_pwm_off)
    {
        App_SetRun(0u);
    }
    else
    {
        /* Giu nguyen trang thai RUN/STOP hien tai. */
        App_SetRun(was_running ? 1u : 0u);
    }
}

void App_ResetToDefaultByPolicy(void)
{
#if (APP_LONGPRESS_RESET_POLICY == APP_RESET_BOOT_DEFAULT)
    App_ResetToDefault(1u);
#elif (APP_LONGPRESS_RESET_POLICY == APP_RESET_KEEP_RUN)
    App_ResetToDefault(0u);
#else
    /* Fallback an toan neu config sai: ve boot default. */
    App_ResetToDefault(1u);
#endif
}
