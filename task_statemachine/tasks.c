#include <stdio.h>

// scheduler.h (already defined)
typedef enum 
{
    SCHEDULER_TASK_STATUS_READY = 0,
    SCHEDULER_TASK_STATUS_ACTIVE,
    SCHEDULER_TASK_STATUS_BLOCKED,
    SCHEDULER_TASK_STATUS_DONE,

} scheduler_task_status_t;

// task.c (new code)
// This new code has 4 purposes
// 1. Handle the logic for enabling/disabling tasks
// 2. Ensure every task is initialized and deinit
// 3. Abstract way the logic for 1 and 2 so devs don't need to re-implement these
// features over and over.
// 4. Create a standard way of performing 1 and 2 so the code is easier to navigate

typedef enum
{
    TASK_STATE_INIT  = 0,
    TASK_STATE_ACTIVE,
    TASK_STATE_PRE_UPDATE,
    TASK_STATE_UPDATE,
    TASK_STATE_POST_UPDATE,
    TASK_STATE_DEINIT,
    TASK_STATE_IDLE,
    TASK_STATE__COUNT,
} task_state_t;

typedef scheduler_task_status_t (*task_state_func_t)(void);

#define MAX_CONFIG_IDS 3
typedef struct
{
    task_state_func_t state_functions[TASK_STATE__COUNT];       
    int* enabled;                       
    int* updating;                      
    task_state_t* state;                
} task_t;

typedef struct 
{
    int updating;
    int config_ids[MAX_CONFIG_IDS];
    task_state_t* state;
} task_table_entry_t;

#define MAX_NUM_TASKS 5
task_table_entry_t task_table[MAX_NUM_TASKS];

typedef enum {
    TASK_EVENT_FALSE = 0,
    TASK_EVENT_TRUE,
    TASK_EVENT_ANY,
} task_event_t;

typedef struct
{
    task_state_t current_state;
    task_event_t enabled;
    task_event_t updating;
    task_state_t next_state;
} state_transition_entry_t;

state_transition_entry_t state_transitions[] = {
    {TASK_STATE_INIT,           TASK_EVENT_TRUE,    TASK_EVENT_ANY,     TASK_STATE_ACTIVE},
    {TASK_STATE_ACTIVE,         TASK_EVENT_TRUE,    TASK_EVENT_FALSE,   TASK_STATE_ACTIVE},
    {TASK_STATE_ACTIVE,         TASK_EVENT_FALSE,   TASK_EVENT_ANY,     TASK_STATE_DEINIT},
    {TASK_STATE_ACTIVE,         TASK_EVENT_TRUE,    TASK_EVENT_TRUE,    TASK_STATE_PRE_UPDATE},
    {TASK_STATE_PRE_UPDATE,     TASK_EVENT_ANY,     TASK_EVENT_ANY,     TASK_STATE_UPDATE},
    {TASK_STATE_UPDATE,         TASK_EVENT_ANY,     TASK_EVENT_TRUE,    TASK_STATE_UPDATE},
    {TASK_STATE_UPDATE,         TASK_EVENT_ANY,     TASK_EVENT_FALSE,   TASK_STATE_POST_UPDATE},
    {TASK_STATE_POST_UPDATE,    TASK_EVENT_ANY,     TASK_EVENT_ANY,     TASK_STATE_ACTIVE},
    {TASK_STATE_DEINIT,         TASK_EVENT_ANY,     TASK_EVENT_ANY,     TASK_STATE_IDLE},
    {TASK_STATE_IDLE,           TASK_EVENT_TRUE,    TASK_EVENT_ANY,     TASK_STATE_INIT},
    {TASK_STATE_IDLE,           TASK_EVENT_FALSE,   TASK_EVENT_ANY,     TASK_STATE_IDLE},
};

scheduler_task_status_t task_runner(task_t* task)
{
    for(int i = 0; i < sizeof(state_transitions)/sizeof(state_transition_entry_t); i++)
    {
        if (state_transitions[i].current_state == *(task->state))
        {
            if (state_transitions[i].enabled == TASK_EVENT_ANY && state_transitions[i].updating == TASK_EVENT_ANY)
            {
                *(task->state) = state_transitions[i].next_state;
                return task->state_functions[*task->state]();
            }
            if(state_transitions[i].updating == *(task->updating) && state_transitions[i].enabled == *(task->enabled))
            {
                *(task->state) = state_transitions[i].next_state;
                return task->state_functions[*task->state]();
            }
            if(state_transitions[i].enabled == TASK_EVENT_ANY && state_transitions[i].updating == *(task->updating))
            {
                *(task->state) = state_transitions[i].next_state;
                return task->state_functions[*task->state]();
            }
            if(state_transitions[i].updating == TASK_EVENT_ANY && state_transitions[i].enabled == *(task->enabled))
            {
                *(task->state) = state_transitions[i].next_state;
                return task->state_functions[*task->state]();
            }
        }
    }
    printf("ERROR: we should not have reached this point. undefined state.\n");
    return SCHEDULER_TASK_STATUS_DONE;
}

// task_compressor.c

// This is the only file a developer will need to create. One file per task
#define TASK_COMPRESSOR_ID 1
// store these globals in shared memory
int updating = 0;
task_state_t task_state = TASK_STATE_IDLE;
int enabled = 0;

static scheduler_task_status_t init_compressor()
{
    printf("Init compressor\n");
    // call init functions for algorithms called from task
    return SCHEDULER_TASK_STATUS_DONE;
}

static scheduler_task_status_t compressor_state_machine()
{
    printf("Run compressor\n");
    // compressor state machine
    // KEEP STATES SHORT!
    // first call is to adaptive_le
    // second call is to compressor_lut
    return SCHEDULER_TASK_STATUS_DONE;
}

static scheduler_task_status_t deinit_compressor()
{
    printf("Deinit compressor\n");
    // call deinit function for algorithms called from this task
    // this is a good time to zero out any gains
    return SCHEDULER_TASK_STATUS_DONE;
}

static scheduler_task_status_t update_compressor_parameters()
{
    printf("update mode\n");
    // Fade out compressor gains
    // cm3 will not update adaptive_le values
    return SCHEDULER_TASK_STATUS_DONE;
}

static scheduler_task_status_t pre_update()
{
    printf("Pre update\n");
    return SCHEDULER_TASK_STATUS_DONE;
}

static scheduler_task_status_t post_update()
{
    printf("Post update\n");
    return SCHEDULER_TASK_STATUS_DONE;
}

static scheduler_task_status_t idle()
{
    printf("idle\n");
    return SCHEDULER_TASK_STATUS_DONE;
}

task_t task_compressor = {
    .state_functions = {
        init_compressor,
        compressor_state_machine,
        pre_update,
        update_compressor_parameters,
        post_update,
        deinit_compressor,
        idle
    },
    .enabled=&enabled,
    .updating=&updating,
    .state=&task_state
};

// main.c
int main(void)
{
    for(int i = 0; i < 5; i++)
    {
        // This is called from the scheduler
        task_runner(&task_compressor);
    }
    *(task_compressor.enabled) = 1;    // This is updated from ezairo memory tool. task_memory.yaml?
    for(int i = 0; i < 5; i++)
    {
        // This is called from the scheduler
        task_runner(&task_compressor);
    }
    *(task_compressor.updating) = 1;    // This is updated from ezairo memory tool. task_memory.yaml?
    for(int i = 0; i < 5; i++)
    {
        // This is called from the scheduler
        task_runner(&task_compressor);
        *(task_compressor.enabled) = 0;    // This is updated from ezairo memory tool. task_memory.yaml?
    }
    *(task_compressor.updating) = 0;    // This is updated from ezairo memory tool. task_memory.yaml?
    for(int i = 0; i < 5; i++)
    {
        // This is called from the scheduler
        task_runner(&task_compressor);
    }
    *(task_compressor.enabled) = 0;    // This is updated from ezairo memory tool. task_memory.yaml?
    for(int i = 0; i < 5; i++)
    {
        // This is called from the scheduler
        task_runner(&task_compressor);
    }
}