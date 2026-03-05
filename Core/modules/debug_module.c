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

void Debug_Task(void)
{
    static uint32_t last_tick = 0;

    if(HAL_GetTick() - last_tick < 500)
        return;

    last_tick = HAL_GetTick();

    LOG("Mode: %d | Freq: %lu Hz | Duty: %u %% | Run: %d",
        app.mode,
        app.freq,
        app.duty,
        app.pwm_running);
}
