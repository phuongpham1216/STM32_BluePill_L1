/*
 * app.c
 *
 *  Created on: Mar 1, 2026
 *      Author: P2Lap
 */


#include "app.h"

App_t app = {
    .mode = MODE_FREQUENCY,
    .freq = 20000,
    .duty = 50,
    .pwm_running = 0
};
