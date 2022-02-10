#ifndef TASK_LIST_H
#define TASK_LIST_H

#include "stdint.h"

#define MAX_NUM_TASKS 10

typedef struct linked_list_node_t linked_list_node_t;

struct linked_list_node_t {
    void *data;
    linked_list_node_t *next_node;
    linked_list_node_t *prev_node;
};

typedef struct {
    linked_list_node_t nodes[MAX_NUM_TASKS];
    uint8_t write_idx;
    linked_list_node_t *head;
    uint8_t num_elements;
} linked_list_t;


void task_list_remove(linked_list_node_t *node, linked_list_t *linked_list);
void task_list_init();

#endif // TASK_LIST_H