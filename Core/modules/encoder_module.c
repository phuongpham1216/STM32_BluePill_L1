/*
 * encoder_module.c
 *
 *  Created on: Feb 28, 2026
 *      Author: P2Lap
 */

#include "encoder_module.h"
#include "encoder_driver.h"
#include "app.h"

/*
 * EC11 thuong cho 2 hoac 4 count cho moi 1 nac tuy cach doc.
 * Cau hinh hien tai dung 4 count/step de 1 nac ~ 1 buoc logic.
 */
#define ENCODER_COUNTS_PER_STEP 4

static uint16_t last_count = 0u;
static uint8_t initialized = 0u;
static int16_t accum_delta = 0;

void Encoder_Task(void)
{
    uint16_t now_count = (uint16_t)Encoder_GetPosition();

    if (!initialized)
    {
        last_count = now_count;
        initialized = 1u;
        return;
    }

    /* Wrap-safe cho counter 16-bit. */
    int16_t raw_delta = (int16_t)(now_count - last_count);
    last_count = now_count;

    if (raw_delta == 0) return;

    accum_delta += raw_delta;

    /* Chuan hoa ve don vi step de moi nac chi doi 1 lan. */
    int16_t step = accum_delta / ENCODER_COUNTS_PER_STEP;
    accum_delta = accum_delta % ENCODER_COUNTS_PER_STEP;

    if (step == 0) return;

    switch (app.mode)
    {
        case MODE_FREQUENCY:
            App_AdjustFrequency(step);
            break;

        case MODE_DUTY:
            App_AdjustDuty(step);
            break;

        default:
            break;
    }
}
