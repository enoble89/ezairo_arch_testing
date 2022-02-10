#include <stdio.h>

// NOTICE array differences in array length between structs
typedef struct 
{
    int num_bands;
    int comp_le_input[4];
    int comp_le_output[4];
} comp_le_t;

typedef struct 
{
    int num_bands;
    int comp_le_input[5];
    int comp_le_output[5];
} mpo_le_t;

void adaptive_le_run(WHAT_GOES_HERE* le) {
    for (int i = 0; i < le->num_bands; i++)
    {
        le->output[i] = le->input[i] * 2;
    }
}

comp_le_t comp_le = {
    .num_bands = 4,
    .comp_le_input = {1, 2, 3, 4},
    .comp_le_output = {0},
};

mpo_le_t mpo_le = {
    .num_bands = 5,
    .comp_le_input = {1, 2, 3, 4, 15},
    .comp_le_output = {0},
};

int main(void) {
    adaptive_le_run(comp_le);
    adaptive_le_run(mpo_le);
}