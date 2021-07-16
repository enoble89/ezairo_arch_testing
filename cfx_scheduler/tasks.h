#ifndef TASKS_H
#define TASKS_H

#include "scheduler.h"

scheduler_task_status_t task_filterbank();
scheduler_task_status_t task_aux_out();
scheduler_task_status_t task_noise_reduction();

#endif // TASKS_H