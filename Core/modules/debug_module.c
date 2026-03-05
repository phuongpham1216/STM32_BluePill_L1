/*
 * debug_module.c
 *
 *  Created on: Feb 28, 2026
 *      Author: P2Lap
 */

#include "debug_module.h"
#include "app.h"
#include "log.h"
#include "main.h"

static const char* App_ModeToString(AppMode_t mode)
{
    switch (mode)
    {
        case MODE_FREQUENCY: return "FREQ";
        case MODE_DUTY:      return "DUTY";
        default:             return "UNK";
    }
}

void Debug_Task(void)
{
    static uint32_t last_tick = 0;

    if (HAL_GetTick() - last_tick < 500u)
        return;

    last_tick = HAL_GetTick();

    LOG("Mode: %s | Freq: %lu Hz | Duty: %u %% | Run: %d",
        App_ModeToString(app.mode),
        app.freq,
        app.duty,
        app.pwm_running);
}
