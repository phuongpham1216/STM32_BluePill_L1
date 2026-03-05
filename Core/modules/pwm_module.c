/*
 * pwm_module.c
 *
 *  Created on: Feb 28, 2026
 *      Author: P2Lap
 */


#include "pwm_module.h"
#include "pwm_driver.h"
#include "app.h"

void PWM_ApplyConfig(void)
{
    PWM_SetFrequency(app.freq);
    PWM_SetDuty(app.duty);
}
