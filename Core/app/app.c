/*
 * app.c
 *
 *  Created on: Mar 1, 2026
 *      Author: P2Lap
 */


#include "app.h"

App_t app = {
    .mode = MODE_FREQUENCY,
    .freq = 20000,
    .duty = 50,
    .pwm_running = 0
};

#include "pwm_driver.h"

void App_AdjustFrequency(int32_t delta)
{
    int32_t f = app.freq + delta * 100;

    if(f < 1000) f = 1000;
    if(f > 50000) f = 50000;

    app.freq = f;
    PWM_SetFrequency(app.freq);
}

void App_AdjustDuty(int32_t delta)
{
    int32_t d = app.duty + delta;

    if(d < 5) d = 5;
    if(d > 95) d = 95;

    app.duty = d;
    PWM_SetDuty(app.duty);
}

void App_SetRun(uint8_t state)
{
    if(state)
    {
        PWM_Start();
        app.pwm_running = 1;
    }
    else
    {
        PWM_Stop();
        app.pwm_running = 0;
    }
}
