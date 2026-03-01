/*
 * button_driver.c
 *
 *  Created on: Mar 1, 2026
 *      Author: P2Lap
 */


#include "button_driver.h"

#define BUTTON_PIN   GPIO_PIN_2
#define BUTTON_PORT  GPIOA

void Button_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);
}

uint8_t Button_GetState(void)
{
    return (HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN) == GPIO_PIN_RESET);
}
