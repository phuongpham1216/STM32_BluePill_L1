/*
 * pwm_module.c
 *
 *  Created on: Feb 28, 2026
 *      Author: P2Lap
 */


#include "pwm_module.h"
#include "pwm_driver.h"

static uint16_t duty = 50;

void PWM_Task(void)
{
    // Tạm thời test: thay đổi duty mỗi 1 giây
    static uint8_t toggle = 0;

    if(toggle == 0)
    {
        duty = 30;
        toggle = 1;
    }
    else
    {
        duty = 70;
        toggle = 0;
    }

    PWM_SetDuty(duty);
}
