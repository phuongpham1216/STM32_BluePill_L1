#include "scheduler.h"

#define MAX_TASKS 10

static Task_t tasks[MAX_TASKS];
static uint8_t task_count = 0;

void Scheduler_Init(void)
{
    task_count = 0;
}

void Scheduler_AddTask(TaskFunction_t task, uint32_t interval)
{
    if(task_count < MAX_TASKS)
    {
        tasks[task_count].Task = task;
        tasks[task_count].interval = interval;
        tasks[task_count].last_run = 0;
        task_count++;
    }
}

void Scheduler_Run(void)
{
    uint32_t now = HAL_GetTick();

    for(uint8_t i = 0; i < task_count; i++)
    {
        if(now - tasks[i].last_run >= tasks[i].interval)
        {
            tasks[i].last_run = now;
            tasks[i].Task();
        }
    }
}
