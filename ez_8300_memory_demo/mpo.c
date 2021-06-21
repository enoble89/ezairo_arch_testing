#include "mpo_memory.h"

// this is defined for demonstration purposes only. This type exits on CFX
typedef uint32_t frac24;


void mpo_run(void* FIFO_POINTER)
{
    frac24 attack = mpo_runtime_data->attack;
    frac24 release = mpo_runtime_data->release;
    // calculate some power
    // average the signal
    // calculate gain
}

