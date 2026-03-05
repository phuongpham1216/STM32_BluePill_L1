/*
 * button_module.c
 *
 *  Created on: Mar 1, 2026
 *      Author: P2Lap
 */


#include "button_module.h"
#include "button_driver.h"
#include "app.h"

#define BUTTON_DEBOUNCE_MS 50

void Button_Task(void)
{
    static uint8_t last_state = 0;
    static uint32_t last_tick = 0;

    uint8_t current = Button_GetState();

    if(current && !last_state)
    {
        if(HAL_GetTick() - last_tick > BUTTON_DEBOUNCE_MS)
        {
            last_tick = HAL_GetTick();

            app.mode++;
            if(app.mode > MODE_RUN)
                app.mode = MODE_FREQUENCY;
        }
    }

    last_state = current;
}
