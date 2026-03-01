/*
 * encoder_module.c
 *
 *  Created on: Feb 28, 2026
 *      Author: P2Lap
 */

#include "main.h"
#include <stdio.h>
#include "encoder_module.h"
#include "encoder_driver.h"
#include "pwm_driver.h"
#include "app.h"

extern TIM_HandleTypeDef htim1;

void Encoder_Task(void)
{
    int16_t delta = Encoder_GetDelta();

    if(delta == 0) return;

    switch(app.mode)
    {
        case MODE_FREQUENCY:
            {
                app.freq += delta * 100;
                if(app.freq < 1000)  app.freq = 1000;
                if(app.freq > 50000) app.freq = 50000;

                PWM_SetFrequency(app.freq);
                printf("Mode:FREQ -> %lu Hz\r\n", app.freq);
            }
            break;

        case MODE_DUTY:
            {
                int32_t d = app.duty + delta;
                if(d <   5) d = 5;
                if(d >  95) d = 95;
                app.duty = d;

                PWM_SetDuty(app.duty);
                printf("Mode:DUTY -> %u %%\r\n", app.duty);
            }
            break;

        case MODE_RUN:
            {
                if(delta > 0)
                {
                    if(!app.pwm_running)
                    {
                        PWM_Init();
                        app.pwm_running = 1;
                        printf("PWM START\r\n");
                    }
                }
                else
                {
                    if(app.pwm_running)
                    {
                        // stop PWM
                        HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
                        HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
                        app.pwm_running = 0;
                        printf("PWM STOP\r\n");
                    }
                }
            }
            break;
    }
}
