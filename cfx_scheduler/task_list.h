#ifndef TASK_LIST_H
#define TASK_LIST_H

#include "stdint.h"

#define MAX_NUM_TASKS 10

int task_list_remove(void *data);
void task_list_add(void *data);
void* task_list_next();
void task_list_reset();
void task_list_clear();

#endif // TASK_LIST_H