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

/*
 * Lua chon mode output:
 * 0: CH1/CH1N complementary (mac dinh timer advanced)
 * 1: CH1N "cung duty" voi CH1 (khong dao 100-D)
 * 2: SG3525 alternating A/B (2 xung luan phien, duty moi nhanh bang nhau)
 */
#define PWM_OUTPUT_MODE_COMPLEMENTARY    0u
#define PWM_OUTPUT_MODE_SAME_DUTY        1u
#define PWM_OUTPUT_MODE_SG3525_AB        2u

/* Doi macro nay de chon che do. */
#define PWM_OUTPUT_MODE                  PWM_OUTPUT_MODE_SG3525_AB

/* Dead-time ticks cho TIM1 (0..255). */
#define PWM_DEADTIME_TICKS               0u

/* Duty toi da cho SG3525 A/B (vi 2 nhanh luan phien -> moi nhanh <= 50%). */
#define PWM_SG3525_MAX_DUTY              49u

/*
 * Cach hieu tan so trong SG3525 mode:
 * 1: pwm_freq la tan so moi nhanh A/B (thuong de de so sanh voi mode thuong)
 * 0: pwm_freq la tan so dao pha (moi update), luc nay tan so moi nhanh bang 1/2.
 */
#define PWM_SG3525_FREQ_PER_CHANNEL      1u

static uint32_t pwm_freq = 20000u;
static uint16_t pwm_duty = 50u;

#if (PWM_OUTPUT_MODE == PWM_OUTPUT_MODE_SG3525_AB)
static volatile uint8_t sg3525_phase = 0u; /* 0: CH1, 1: CH1N */
#endif

static uint32_t PWM_GetTimerTickFreq(void)
{
#if (PWM_OUTPUT_MODE == PWM_OUTPUT_MODE_SG3525_AB)
#if PWM_SG3525_FREQ_PER_CHANNEL
    uint32_t tick = pwm_freq * 2u;
    if (tick < pwm_freq) tick = PWM_MAX_FREQ_HZ * 2u;
    return tick;
#else
    return pwm_freq;
#endif
#else
    return pwm_freq;
#endif
}

#if (PWM_OUTPUT_MODE == PWM_OUTPUT_MODE_SG3525_AB)
static void PWM_SG3525_ApplyPhaseOutputs(void)
{
    uint32_t ccer = htim1.Instance->CCER;

    /* Chi giu mot nhanh active tai moi thoi diem. */
    ccer &= ~(TIM_CCER_CC1E | TIM_CCER_CC1NE);

    if (sg3525_phase == 0u)
    {
        ccer |= TIM_CCER_CC1E;
    }
    else
    {
        ccer |= TIM_CCER_CC1NE;
    }

    htim1.Instance->CCER = ccer;
}
#endif

static void PWM_ConfigOutputRelation(void)
{
    uint32_t ccer = htim1.Instance->CCER;

    /* CH1 giu active high (CC1P = 0). */
    ccer &= ~TIM_CCER_CC1P;

#if (PWM_OUTPUT_MODE == PWM_OUTPUT_MODE_COMPLEMENTARY)
    /* CH1N complementary chuan. */
    ccer &= ~TIM_CCER_CC1NP;
#elif (PWM_OUTPUT_MODE == PWM_OUTPUT_MODE_SAME_DUTY)
    /* Dao polarity nhanh N de co cung duty voi CH1. */
    ccer |= TIM_CCER_CC1NP;
#elif (PWM_OUTPUT_MODE == PWM_OUTPUT_MODE_SG3525_AB)
    /* Trong SG3525 A/B: dat N cung chieu voi CH1 khi duoc enable. */
    ccer |= TIM_CCER_CC1NP;
#else
    #error "PWM_OUTPUT_MODE khong hop le"
#endif

    htim1.Instance->CCER = ccer;

    /* Cap nhat dead-time theo cau hinh runtime. */
    MODIFY_REG(htim1.Instance->BDTR, TIM_BDTR_DTG, (PWM_DEADTIME_TICKS & TIM_BDTR_DTG));
}

static void PWM_ApplyDutyToCurrentArr(void)
{
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(&htim1);
    uint32_t pulse;

#if (PWM_OUTPUT_MODE == PWM_OUTPUT_MODE_SG3525_AB)
#if PWM_SG3525_FREQ_PER_CHANNEL
    /* Timer dang chay 2*fout nen duty timer = 2*duty user. */
    pulse = ((arr + 1u) * (uint32_t)(pwm_duty * 2u)) / 100u;
#else
    /* Timer trung tan so user thi duty timer = duty user. */
    pulse = ((arr + 1u) * pwm_duty) / 100u;
#endif
#else
    pulse = ((arr + 1u) * pwm_duty) / 100u;
#endif

    if (pulse > arr) pulse = arr;
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pulse);
}

static void PWM_ApplyFrequencyAndDuty(void)
{
    uint32_t prescaler = 0u;
    uint32_t timer_freq = PWM_GetTimerTickFreq();
    uint32_t arr = (TIM1_CLK_HZ / timer_freq) - 1u;

    if (arr > 0xFFFFu)
    {
        prescaler = arr / 0x10000u;
        if (prescaler > 0xFFFFu) prescaler = 0xFFFFu;
        arr = (TIM1_CLK_HZ / ((prescaler + 1u) * timer_freq)) - 1u;
    }

    if (arr > 0xFFFFu) arr = 0xFFFFu;

    __HAL_TIM_SET_PRESCALER(&htim1, prescaler);
    __HAL_TIM_SET_AUTORELOAD(&htim1, arr);
    __HAL_TIM_SET_COUNTER(&htim1, 0u);

    HAL_TIM_GenerateEvent(&htim1, TIM_EVENTSOURCE_UPDATE);
    PWM_ApplyDutyToCurrentArr();
}

void PWM_Init(void)
{
    PWM_ConfigOutputRelation();

#if (PWM_OUTPUT_MODE == PWM_OUTPUT_MODE_SG3525_AB)
    HAL_NVIC_SetPriority(TIM1_UP_IRQn, 1u, 0u);
    HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
#endif

    PWM_ApplyFrequencyAndDuty();
}

void PWM_SetDuty(uint16_t duty_percent)
{
    if (duty_percent < PWM_MIN_DUTY) duty_percent = PWM_MIN_DUTY;

#if (PWM_OUTPUT_MODE == PWM_OUTPUT_MODE_SG3525_AB)
    if (duty_percent > PWM_SG3525_MAX_DUTY) duty_percent = PWM_SG3525_MAX_DUTY;
#else
    if (duty_percent > PWM_MAX_DUTY) duty_percent = PWM_MAX_DUTY;
#endif

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

#if (PWM_OUTPUT_MODE == PWM_OUTPUT_MODE_SG3525_AB)
    /* HAL start da bat ca 2 nhanh -> ep lai dung pha ban dau. */
    sg3525_phase = 0u;
    PWM_SG3525_ApplyPhaseOutputs();
    __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
#else
    __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);
#endif
}

void PWM_Stop(void)
{
    __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
}

#if (PWM_OUTPUT_MODE == PWM_OUTPUT_MODE_SG3525_AB)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
        sg3525_phase ^= 1u;
        PWM_SG3525_ApplyPhaseOutputs();
    }
}
#endif
