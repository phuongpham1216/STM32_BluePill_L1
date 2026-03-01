/*
 * button_module.c
 *
 *  Created on: Mar 1, 2026
 *      Author: P2Lap
 */


#include "button_module.h"
#include "button_driver.h"
#include "app.h"

#define DEBOUNCE_MS 200

void Button_Task(void)
{
    static uint32_t last_tick = 0;

    if(Button_GetState())
    {
        if(HAL_GetTick() - last_tick > DEBOUNCE_MS)
        {
            last_tick = HAL_GetTick();

            app.mode++;
            if(app.mode > MODE_RUN) app.mode = MODE_FREQUENCY;
        }
    }
}
