/*
 * encoder_driver.c
 *
 *  Created on: Feb 28, 2026
 *      Author: P2Lap
 */


#include "encoder_driver.h"

extern TIM_HandleTypeDef htim2;

static int16_t last_value = 0;

void Encoder_Init(void)
{
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
    last_value = __HAL_TIM_GET_COUNTER(&htim2);
}

int16_t Encoder_GetDelta(void)
{
    int16_t now = __HAL_TIM_GET_COUNTER(&htim2);
    int16_t delta = now - last_value;
    last_value = now;
    return delta;
}
