#include <stdio.h>
#include "tasks.h"

scheduler_task_status_t task_filterbank()
{
    printf("Running filterbank task\n");
    return SCHEDULER_TASK_STATUS_DONE;
}

scheduler_task_status_t task_aux_out()
{
    printf("Running aux out task\n");
    return SCHEDULER_TASK_STATUS_DONE;
}

scheduler_task_status_t task_noise_reduction()
{
    printf("Running noise reduction task\n");
    return SCHEDULER_TASK_STATUS_DONE;
}