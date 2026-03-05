/*
 * button_module.c
 *
 *  Created on: Mar 1, 2026
 *      Author: P2Lap
 */

#include "button_module.h"
#include "button_driver.h"
#include "app.h"

#define BUTTON_DEBOUNCE_MS 50u

void Button_Task(void)
{
    static uint8_t mode_last = 0u;
    static uint8_t run_last = 0u;
    static uint32_t mode_last_tick = 0u;
    static uint32_t run_last_tick = 0u;

    uint32_t now = HAL_GetTick();
    uint8_t mode_current = Button_GetModeState();
    uint8_t run_current = Button_GetRunState();

    if (mode_current && !mode_last)
    {
        if (now - mode_last_tick >= BUTTON_DEBOUNCE_MS)
        {
            mode_last_tick = now;
            app.mode = (app.mode == MODE_FREQUENCY) ? MODE_DUTY : MODE_FREQUENCY;
        }
    }

    if (run_current && !run_last)
    {
        if (now - run_last_tick >= BUTTON_DEBOUNCE_MS)
        {
            run_last_tick = now;
            App_SetRun(!app.pwm_running);
        }
    }

    mode_last = mode_current;
    run_last = run_current;
}
