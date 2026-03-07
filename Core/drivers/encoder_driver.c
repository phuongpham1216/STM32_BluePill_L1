/*
 * encoder_driver.c
 *
 *  Created on: Feb 28, 2026
 *      Author: P2Lap
 */


#include "encoder_driver.h"

extern TIM_HandleTypeDef htim2;

void Encoder_Init(void)
{
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
}

int16_t Encoder_GetPosition(void)
{
    return __HAL_TIM_GET_COUNTER(&htim2);
}
