#include <stddef.h>
#include "task_list.h"

void task_list_remove(linked_list_node_t *node, linked_list_t *linked_list)
{
    if (node->prev_node == NULL) {
        linked_list->head = node->next_node;
    } else if (linked_list->num_elements != 1)
    {
        node->prev_node->next_node = node->next_node;
    }
    linked_list->num_elements--;
}

void task_list_init(linked_list_t *linked_list)
{
    linked_list->write_idx = 0;
    linked_list->num_elements = 0;
    linked_list->head = NULL;
}
