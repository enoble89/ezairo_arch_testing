#ifndef SCHEDULER_H
#define SCHEDULER_H

typedef enum 
{
    SCHEDULER_TASK_STATUS_READY = 0,
    SCHEDULER_TASK_STATUS_ACTIVE,
    SCHEDULER_TASK_STATUS_BLOCKED,
    SCHEDULER_TASK_STATUS_DONE,

} scheduler_task_status_t;

typedef enum 
{
    SCHEDULER_TASK_INDEX_FILTERBANK = 0,
    SCHEDULER_TASK_INDEX_AUX_OUT,
    SCHEDULER_TASK_INDEX_NR,
    SCHEDULER_TASK_INDEX__COUNT,
} scheduler_task_index_t;

typedef scheduler_task_status_t (*task_func)();

typedef struct
{
    int period_ticks;
    scheduler_task_status_t status;
    int ticks_till_release;
    task_func func_pointer;
} scheduler_task_control_block_t;

scheduler_task_status_t task_filterbank();
scheduler_task_status_t task_aux_out();
scheduler_task_status_t task_noise_reduction();

void scheduler_process();
void scheduler_trigger();

#endif // SCHEDULER_H