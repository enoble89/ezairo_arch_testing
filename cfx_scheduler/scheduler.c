#include <stddef.h>
#include <stdio.h>
#include "scheduler.h"
#include "task_list.h"

#define TICK_SAMPLE_RATE 3000
#define SAMPLE_RATE_TO_TICKS(x) (TICK_SAMPLE_RATE/x)
#define DELAY_MS_TO_TICKS(x) (x/TICK_PERIOD)

// Each scheduler tick equals .33ms (1/TICK_SAMPLE_RATE)
// These delays are used to offset the task load
#define START_DELAY_1    1  // 0.33ms
#define START_DELAY_2    2  // 0.66ms
#define START_DELAY_3    3  // 1.00ms
#define START_DELAY_4    4  // 1.33ms
#define START_DELAY_5    5  // 1.55ms
#define START_DELAY_6    6  // 2.00ms

// Updates in FIFO update interrupt
static int s_scheduler_trigger = 0;

// The order of the tasks in this table is the priority indicates their priority
static scheduler_task_control_block_t scheduler_tcb_table[SCHEDULER_TASK_INDEX__COUNT] = {
    {.func_pointer=task_filterbank, .status=SCHEDULER_TASK_STATUS_DONE, .ticks_till_release=START_DELAY_1, .period_ticks=SAMPLE_RATE_TO_TICKS(1500), },
    {.func_pointer=task_aux_out, .status=SCHEDULER_TASK_STATUS_DONE, .ticks_till_release=START_DELAY_2, .period_ticks=SAMPLE_RATE_TO_TICKS(3000) },
    {.func_pointer=task_noise_reduction, .status=SCHEDULER_TASK_STATUS_DONE, .ticks_till_release=START_DELAY_1, .period_ticks=SAMPLE_RATE_TO_TICKS(600) },
};

static void sleep()
{
    // replace with cfx sleep function
    return;
}

static int run_tasks()
{
    task_list_reset(); // Reset read pointer so we can start with the higher priority tasks
    scheduler_task_control_block_t *curr_task = (scheduler_task_control_block_t*) task_list_next();
    while (curr_task != NULL)
    {
        if (curr_task->status == SCHEDULER_TASK_STATUS_READY)
        {
            task_func func_pointer = curr_task->func_pointer;
            curr_task->status = func_pointer();

            if (curr_task->status == SCHEDULER_TASK_STATUS_DONE) {
                // returns 0 if there are not tasks left
                return task_list_remove((void*) curr_task);
            }
        }
        curr_task = (scheduler_task_control_block_t*) task_list_next();
    }
    return 0;
}

void scheduler_trigger()
{
    s_scheduler_trigger = 1;
}

void scheduler_process()
{
    if (s_scheduler_trigger)
    {
        printf("New scheduler trigger\n");
        s_scheduler_trigger = 0;
        task_list_clear();
        // Let's schedule some work!
        for (int i = 0; i < SCHEDULER_TASK_INDEX__COUNT; i ++)  // ensure this is a software loop
        {
            if (--scheduler_tcb_table[i].ticks_till_release == 0)
            {
                if (scheduler_tcb_table[i].status != SCHEDULER_TASK_STATUS_DONE)
                {
                    printf("FAILED TO SCHEDULE TASK\n");
                }
                scheduler_tcb_table[i].ticks_till_release = scheduler_tcb_table[i].period_ticks;
                scheduler_tcb_table[i].status = SCHEDULER_TASK_STATUS_READY;
                task_list_add(&scheduler_tcb_table[i]);
            }
        }

        // Run tasks until they are all finished.
        while(run_tasks()) {}

    }
    sleep(); // Returns when there is an interrupt
}