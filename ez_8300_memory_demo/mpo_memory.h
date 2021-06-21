#include <stdint.h>

typedef struct mpo_runtime_data
{
    uint32_t mpo_max_gain;
    uint32_t attack;
    uint32_t release;
    uint32_t gains[4];
} mpo_runtime_data_t;

extern volatile mpo_runtime_data_t* mpo_runtime_data;