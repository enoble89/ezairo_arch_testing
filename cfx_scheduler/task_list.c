#include <stddef.h>
#include "task_list.h"

typedef struct linked_list_node_t linked_list_node_t;

struct linked_list_node_t {
    void *data;
    linked_list_node_t *next_node;
};

typedef struct {
    linked_list_node_t nodes[MAX_NUM_TASKS];
    uint8_t write_idx;
    linked_list_node_t *last_read;
    linked_list_node_t *head;
    uint8_t num_elements;
} linked_list_t;

static linked_list_t linked_list;

int task_list_remove(void *data)
{
    // Removes the task and returns the number of tasks left
    linked_list_node_t *last_node = NULL;
    linked_list_node_t *curr_node = linked_list.head;

    if (linked_list.num_elements == 1)
    {
        linked_list.head = NULL;
        linked_list.last_read = linked_list.head;
        return 0;
    }

    for (int i = 0; i < linked_list.num_elements; i++)
    {
        // found node to remove
        if (curr_node->data == data)
        {
            // node is at the beginning
            if (last_node == NULL) 
            {
                linked_list.head = curr_node->next_node;
                linked_list.num_elements--;
                linked_list.last_read = linked_list.head;
                return linked_list.num_elements;
            }
            
            // node is at end
            if (curr_node->next_node == NULL)
            {
                last_node->next_node = NULL;
                linked_list.num_elements--;
                linked_list.last_read = linked_list.head;
                return linked_list.num_elements;
            }

            // node is in the middle
            last_node->next_node = curr_node->next_node;
            linked_list.num_elements--;
            linked_list.last_read = linked_list.head;
            return linked_list.num_elements;
        }

        // update for next loop exec
        last_node = curr_node;
        curr_node = curr_node->next_node;
    }
    return 0;
}

void task_list_add(void *data)
{
    // The assumption is no nodes will be added after nodes are removed.
    linked_list_node_t *new_node = &linked_list.nodes[linked_list.write_idx++];
    new_node->data = data;
    new_node->next_node = NULL;

    linked_list.num_elements++;

    if (linked_list.num_elements == 1)
    {
        linked_list.head = new_node;
        return;
    }

    linked_list.nodes[linked_list.num_elements - 2].next_node = new_node;
}

void task_list_clear()
{
    linked_list.write_idx = 0;
    linked_list.num_elements = 0;
    linked_list.last_read = NULL;
    linked_list.head = NULL;
}

// Returns next item in the list or NULL if there are no items left
void* task_list_next()
{
    if (linked_list.last_read == NULL)
    {
        return NULL;
    }

    if (linked_list.last_read->next_node == NULL)
    {
        linked_list.last_read = linked_list.head;
        return linked_list.head->data;
    }

    void* result = linked_list.last_read->data;
    linked_list.last_read = linked_list.last_read->next_node;
    return result;
}

void task_list_reset()
{
    linked_list.last_read = linked_list.head;
}