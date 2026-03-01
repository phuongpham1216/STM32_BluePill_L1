/*
 * scheduler.h
 *
 *  Created on: Feb 28, 2026
 *      Author: P2Lap
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "main.h"

typedef void (*TaskFunction_t)(void);

typedef struct
{
    TaskFunction_t Task;
    uint32_t interval;
    uint32_t last_run;
}Task_t;

void Scheduler_Init(void);
void Scheduler_AddTask(TaskFunction_t task, uint32_t interval);
void Scheduler_Run(void);

#endif
/* SYSTEM_SCHEDULER_H_ */
