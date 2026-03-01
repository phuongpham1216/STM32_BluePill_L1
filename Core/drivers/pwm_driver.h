/*
 * pwm_driver.h
 *
 *  Created on: Feb 28, 2026
 *      Author: P2Lap
 */

#ifndef PWM_DRIVER_H
#define PWM_DRIVER_H

#include "main.h"

void PWM_Init(void);
void PWM_SetDuty(uint16_t duty_percent);
void PWM_SetFrequency(uint32_t freq);
void PWM_Start(void);
void PWM_Stop(void);

#endif
/* DRIVERS_PWM_DRIVER_H_ */
