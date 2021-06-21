#include "x_mem.h"

volatile shared_memory shared_x_memory = {};

volatile mpo_runtime_data_t* mpo_runtime_data = &shared_x_memory.mpo_runtime_data;
volatile gains_t* gains = &shared_x_memory.gains;