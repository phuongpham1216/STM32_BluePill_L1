/*
 * debug_module.c
 *
 *  Created on: Feb 28, 2026
 *      Author: P2Lap
 */

#include "main.h"
#include "debug_module.h"
#include "stdio.h"
#include <stdint.h>

extern UART_HandleTypeDef huart1;

int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,100);
    return ch;
}

void Debug_Task(void)
{
    static uint32_t counter = 0;
    printf("System Alive %lu\r\n", (unsigned long)counter++);
}

