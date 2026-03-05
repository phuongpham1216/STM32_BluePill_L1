/*
 * app.h
 *
 *  Created on: Mar 1, 2026
 *      Author: P2Lap
 */

#ifndef APP_H
#define APP_H

#include <stdint.h>

typedef enum {
    MODE_FREQUENCY = 0,
    MODE_DUTY,
    MODE_RUN
} AppMode_t;

typedef struct {
    AppMode_t mode;
    uint32_t freq;
    uint16_t duty;
    uint8_t pwm_running;
} App_t;

extern App_t app;

void App_AdjustFrequency(int32_t delta);
void App_AdjustDuty(int32_t delta);
void App_SetRun(uint8_t state);

#endif/* APP_APP_H_ */
