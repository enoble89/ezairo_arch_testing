// start of ezairo_memory.c (lives on the CM3)

#include <stdint.h>
#include "x_mem.h"

#define MAX_EZAIRO_MEMORY_VARIABLES_PER_CMD 20
#define GET_VARIABLE_NAME(Variable) (#Variable)

enum ezairo_memory_format {
    FIXED_POINT_1_23,
};

typedef struct
{
    volatile uint32_t* address;
    uint8_t num_words;                  // used to support writing arrays
    uint8_t word_size;                  // right now we only support shared X mem, so this will always be 3
    enum ezairo_memory_format format; 
    char* name;
} ezairo_memory_meta_t;


enum ezairo_memory_cmds {
    MPO_PARAMS = 0,
    GAINS = 1,
    EZAIRO_MEMORY_CMDS__LENGTH,
};


ezairo_memory_meta_t mpo_params[3] = {
    { .address = &shared_x_memory.mpo_runtime_data.mpo_max_gain, .num_words = 1, .word_size = 3, .format = FIXED_POINT_1_23, .name = GET_VARIABLE_NAME(shared_x_memory.mpo_runtime_data.mpo_max_gain)},
    { .address = &shared_x_memory.mpo_runtime_data.attack, .num_words = 1, .word_size = 3, .format = FIXED_POINT_1_23, .name = GET_VARIABLE_NAME(shared_x_memory.mpo_runtime_data.attack)},
    { .address = shared_x_memory.mpo_runtime_data.gains, .num_words = 4, .word_size = 3, .format = FIXED_POINT_1_23, .name = GET_VARIABLE_NAME(shared_x_memory.mpo_runtime_data.gains)},
};

ezairo_memory_meta_t gains_meta[1] = {
    { .address = shared_x_memory.gains.gains_64ch, .num_words = 64, .word_size = 3, .format = FIXED_POINT_1_23, .name = GET_VARIABLE_NAME(shared_x_memory.gains.gains_64ch)},
};

ezairo_memory_meta_t* ezairo_memory_table[EZAIRO_MEMORY_CMDS__LENGTH] = {
    mpo_params,
    gains_meta,
};
