/*
 * encoder_module.c
 *
 *  Created on: Feb 28, 2026
 *      Author: P2Lap
 */


#include "encoder_module.h"
#include "encoder_driver.h"
#include "pwm_driver.h"

static uint16_t duty = 50;

void Encoder_Task(void)
{
    int16_t delta = Encoder_GetDelta();

    if(delta != 0)
    {
        duty += delta;

        if(duty < 5) duty = 5;
        if(duty > 95) duty = 95;

        PWM_SetDuty(duty);
    }
}
