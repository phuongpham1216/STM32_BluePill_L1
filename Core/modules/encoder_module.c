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

static int16_t last_position = 0;
extern TIM_HandleTypeDef htim1;

void Encoder_Task(void)
{
	int16_t now = Encoder_GetPosition();
	int16_t delta = now - last_position;
	last_position = now;

	if(delta == 0) return;

	switch(app.mode)
	{
	    case MODE_FREQUENCY:
	        App_AdjustFrequency(delta);
	        break;

	    case MODE_DUTY:
	        App_AdjustDuty(delta);
	        break;

	    case MODE_RUN:
	        if(delta > 0)
	            App_SetRun(1);
	        else
	            App_SetRun(0);
	        break;
	}
}
