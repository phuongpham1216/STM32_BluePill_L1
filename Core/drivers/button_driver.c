/*
 * button_driver.c
 *
 *  Created on: Mar 1, 2026
 *      Author: P2Lap
 */

#include "button_driver.h"

#define MODE_BUTTON_PIN   GPIO_PIN_2
#define MODE_BUTTON_PORT  GPIOA

#define RUN_BUTTON_PIN    GPIO_PIN_3
#define RUN_BUTTON_PORT   GPIOA

void Button_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = MODE_BUTTON_PIN | RUN_BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

uint8_t Button_GetModeState(void)
{
    return (HAL_GPIO_ReadPin(MODE_BUTTON_PORT, MODE_BUTTON_PIN) == GPIO_PIN_RESET);
}

uint8_t Button_GetRunState(void)
{
    return (HAL_GPIO_ReadPin(RUN_BUTTON_PORT, RUN_BUTTON_PIN) == GPIO_PIN_RESET);
}
