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
    {.func_pointer=task_aux_out, .status=SCHEDULER_TASK_STATUS_DONE, .ticks_till_release=START_DELAY_2, .period_ticks=SAMPLE_RATE_TO_TICKS(3000) },
    {.func_pointer=task_aux_out, .status=SCHEDULER_TASK_STATUS_DONE, .ticks_till_release=START_DELAY_2, .period_ticks=SAMPLE_RATE_TO_TICKS(3000) },
    {.func_pointer=task_noise_reduction, .status=SCHEDULER_TASK_STATUS_DONE, .ticks_till_release=START_DELAY_1, .period_ticks=SAMPLE_RATE_TO_TICKS(600) },
};

static void sleep()
{
    // replace with cfx sleep function
    return;
}

// static int run_tasks()
// {
//     task_list_reset(); // Reset read pointer so we can start with the higher priority tasks
//     scheduler_task_control_block_t *curr_task = (scheduler_task_control_block_t*) task_list_next();
//     while (curr_task != NULL)
//     {
//         if (curr_task->status == SCHEDULER_TASK_STATUS_READY)
//         {
//             task_func func_pointer = curr_task->func_pointer;
//             curr_task->status = func_pointer();

//             if (curr_task->status == SCHEDULER_TASK_STATUS_DONE) {
//                 // returns 0 if there are not tasks left
//                 return task_list_remove((void*) curr_task);
//             }
//         }
//         curr_task = (scheduler_task_control_block_t*) task_list_next();
//     }
//     return 0;
// }

void scheduler_trigger()
{
    s_scheduler_trigger = 1;
}

void scheduler_process()
{
    if (s_scheduler_trigger)
    {
        s_scheduler_trigger = 0;
        linked_list_t tasks_to_run;
        task_list_init(&tasks_to_run);

        // Let's schedule some work!
        for (int i = 0; i < MAX_NUM_TASKS; i++)
        {
            if (--scheduler_tcb_table[i].ticks_till_release == 0)
            {
                scheduler_tcb_table[i].ticks_till_release = scheduler_tcb_table[i].period_ticks;
                scheduler_tcb_table[i].status = SCHEDULER_TASK_STATUS_READY;

                linked_list_node_t *new_node = &(tasks_to_run.nodes[tasks_to_run.write_idx++]);
                new_node->data = &scheduler_tcb_table[i];
                new_node->next_node = NULL;
                new_node->prev_node = NULL;

                tasks_to_run.num_elements++;

                if (tasks_to_run.num_elements == 1)
                {
                    tasks_to_run.head = new_node;
                } else {
                    tasks_to_run.nodes[tasks_to_run.num_elements - 2].next_node = new_node;
                    new_node->prev_node = &tasks_to_run.nodes[tasks_to_run.num_elements - 2];
                }
            }
        }


        // for (int i = 0; i < MAX_NUM_TASKS; i++)
        // {
        //     scheduler_task_control_block_t curr_task = scheduler_tcb_table[i];
        //     if (scheduler_tcb_table[i].status == SCHEDULER_TASK_STATUS_READY) chess_frequent_then
        //     {
        //         task_func func_pointer = scheduler_tcb_table[i].func_pointer;
        //         scheduler_tcb_table[i].status = func_pointer();

        //         // if (curr_task->status == SCHEDULER_TASK_STATUS_DONE) {
        //         //     task_list_remove((void*) curr_task, &tasks_to_run);
        //         // }
        //     }
        // }
            
        //     {
        //         if (scheduler_tcb_table[i].status != SCHEDULER_TASK_STATUS_DONE) chess_frequent_else
        //         {
        //             // ERROR: Failed to scheduler task
        //             debug_gpio_toggle(GPIO_DEBUG_PIN_15);
        //         }

        //     }

        // }

        // Run tasks until they are all finished.

        while(tasks_to_run.num_elements > 0)
        {
            linked_list_node_t *curr_node = (linked_list_node_t*) tasks_to_run.head;
            while (curr_node != NULL)
            {
                scheduler_task_control_block_t *curr_task = (scheduler_task_control_block_t*) curr_node->data;
                if (curr_task->status == SCHEDULER_TASK_STATUS_READY)
                {
                    task_func func_pointer = curr_task->func_pointer;
                    curr_task->status = func_pointer();

                    if (curr_task->status == SCHEDULER_TASK_STATUS_DONE) {
                        task_list_remove(curr_node, &tasks_to_run);
                    }
                }
                curr_node = curr_node->next_node;
            }
        }
    }
}
