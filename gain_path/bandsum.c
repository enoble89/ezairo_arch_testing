#include "stdio.h"


void bandsum_64ch_to_12ch(int* input, int* output) {
    static const float scale[] = {0.4, 0.5, 0.5, 0.5, 0.5, 0.25, 0.25, 0.16666667, 0.16666667, 0.1, 0.1, 0.08};
    static const float agg[] = {2, 1, 1, 1, 1, 3, 3, 5, 5, 9, 9, 12};

    int input_idx = 0;
    for (int i = 0; i < sizeof(agg) / sizeof(float); i++)
    {
        int acc = 0;
        for (int j = 0; j < agg[i]; j++)
        {
            acc += input[input_idx++];
        }
        output[i] = acc * scale[i];
    }
}  

int main(void)
{
    int input[64] = {0};
    int output[12] = {0};
    bandsum_64ch_to_12ch(input, output);
}