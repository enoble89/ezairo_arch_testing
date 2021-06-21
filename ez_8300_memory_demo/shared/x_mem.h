#include <stdint.h>
#include "mpo_memory.h"
#include "gain_memory.h"


typedef struct
{
    mpo_runtime_data_t mpo_runtime_data;
    gains_t gains;
} shared_memory;

extern volatile shared_memory shared_x_memory;
