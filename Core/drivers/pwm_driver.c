/*
 * pwm_driver.c
 *
 *  Created on: Feb 28, 2026
 *      Author: P2Lap
 */


#include "pwm_driver.h"

extern TIM_HandleTypeDef htim1;

static uint32_t pwm_freq = 20000;
static uint16_t pwm_duty = 50;

void PWM_Init(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
}

void PWM_SetDuty(uint16_t duty_percent)
{
    if(duty_percent < 5) duty_percent = 5;
    if(duty_percent > 95) duty_percent = 95;

    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(&htim1);
    uint32_t pulse = (arr * duty_percent) / 100;

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pulse);
    pwm_duty = duty_percent;
}

void PWM_SetFrequency(uint32_t freq)
{
    if(freq < 1000) freq = 1000;
    if(freq > 50000) freq = 50000;

    uint32_t arr = 72000000 / freq - 1;

    __HAL_TIM_SET_AUTORELOAD(&htim1, arr);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, arr/2);

    pwm_freq = freq;
}
