/*
 * pwm_driver.c
 *
 *  Created on: Feb 28, 2026
 *      Author: P2Lap
 */

#include "pwm_driver.h"

extern TIM_HandleTypeDef htim1;

#define TIM1_CLK_HZ      72000000u
#define PWM_MIN_FREQ_HZ  1000u
#define PWM_MAX_FREQ_HZ  50000u
#define PWM_MIN_DUTY     5u
#define PWM_MAX_DUTY     95u

static uint32_t pwm_freq = 20000u;
static uint16_t pwm_duty = 50u;

static void PWM_ApplyDutyToCurrentArr(void)
{
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(&htim1);
    /*
     * CCR (pulse) theo phan tram duty:
     * duty(%) = CCR / (ARR + 1)
     */
    uint32_t pulse = ((arr + 1u) * pwm_duty) / 100u;

    if (pulse > arr) pulse = arr;
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pulse);
}

static void PWM_ApplyFrequencyAndDuty(void)
{
    uint32_t prescaler = 0u;
    uint32_t arr = (TIM1_CLK_HZ / pwm_freq) - 1u;

    /*
     * TIM1 la timer 16-bit, ARR toi da 65535.
     * Neu ARR vuot nguong, tang PSC de ha ARR ve dung dai.
     */
    if (arr > 0xFFFFu)
    {
        prescaler = arr / 0x10000u;
        if (prescaler > 0xFFFFu) prescaler = 0xFFFFu;
        arr = (TIM1_CLK_HZ / ((prescaler + 1u) * pwm_freq)) - 1u;
    }

    if (arr > 0xFFFFu) arr = 0xFFFFu;

    __HAL_TIM_SET_PRESCALER(&htim1, prescaler);
    __HAL_TIM_SET_AUTORELOAD(&htim1, arr);
    __HAL_TIM_SET_COUNTER(&htim1, 0u);

    /*
     * Phat sinh su kien update de PSC/ARR moi co hieu luc ngay,
     * tranh trang thai chuyen tiep sai tan so.
     */
    HAL_TIM_GenerateEvent(&htim1, TIM_EVENTSOURCE_UPDATE);

    /* Luon ap duty sau khi doi ARR de giu dung phan tram dau ra. */
    PWM_ApplyDutyToCurrentArr();
}

void PWM_Init(void)
{
    /* Chi nap cau hinh, KHONG start xung luc khoi dong. */
    PWM_ApplyFrequencyAndDuty();
}

void PWM_SetDuty(uint16_t duty_percent)
{
    if (duty_percent < PWM_MIN_DUTY) duty_percent = PWM_MIN_DUTY;
    if (duty_percent > PWM_MAX_DUTY) duty_percent = PWM_MAX_DUTY;

    pwm_duty = duty_percent;
    PWM_ApplyDutyToCurrentArr();
}

void PWM_SetFrequency(uint32_t freq)
{
    if (freq < PWM_MIN_FREQ_HZ) freq = PWM_MIN_FREQ_HZ;
    if (freq > PWM_MAX_FREQ_HZ) freq = PWM_MAX_FREQ_HZ;

    pwm_freq = freq;
    PWM_ApplyFrequencyAndDuty();
}

void PWM_Start(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
}

void PWM_Stop(void)
{
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
}
