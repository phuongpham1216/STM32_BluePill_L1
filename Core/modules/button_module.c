/*
 * button_module.c
 *
 *  Created on: Mar 1, 2026
 *      Author: P2Lap
 */

#include "button_module.h"
#include "button_driver.h"
#include "app.h"

/* Cac tham so co the chinh theo yeu cau bai toan. */
#define BUTTON_DEBOUNCE_MS  50u
#define MODE_LONG_PRESS_MS  3000u

void Button_Task(void)
{
    static uint8_t mode_last = 0u;
    static uint8_t run_last = 0u;
    static uint32_t run_last_tick = 0u;

    /* Bien phuc vu nhan-giu nut MODE. */
    static uint32_t mode_press_tick = 0u;
    static uint8_t mode_long_handled = 0u;

    uint32_t now = HAL_GetTick();
    uint8_t mode_current = Button_GetModeState();
    uint8_t run_current = Button_GetRunState();

    /* -------------------- MODE button -------------------- */
    if (mode_current && !mode_last)
    {
        /* Vua nhan xuong. */
        mode_press_tick = now;
        mode_long_handled = 0u;
    }
    else if (mode_current && mode_last)
    {
        /* Dang giu: neu qua nguong thi reset default 1 lan. */
        if (!mode_long_handled && (now - mode_press_tick >= MODE_LONG_PRESS_MS))
        {
            App_ResetToDefaultByPolicy();
            mode_long_handled = 1u;
        }
    }
    else if (!mode_current && mode_last)
    {
        /* Vua nha nut MODE: neu chua long-press thi xu ly short-press. */
        uint32_t held_ms = now - mode_press_tick;

        if (!mode_long_handled && (held_ms >= BUTTON_DEBOUNCE_MS))
        {
            app.mode = (app.mode == MODE_FREQUENCY) ? MODE_DUTY : MODE_FREQUENCY;
        }
    }

    /* -------------------- RUN button --------------------- */
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
