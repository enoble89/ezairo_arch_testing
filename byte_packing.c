#include "stdio.h"

// we are encoding 5 bits
#define STATE_TRANSISION_KEY(state, enabled, updating) (enabled << 4 | updating << 3 | state)

staic int trans_table[32] = {};

trans_table[12] = 3;
int main(void) {
    // init trans table
    for(int i = 0; i < 32; i++)
    {
        trans_table[i] = i;
    }

    int x = STATE_TRANSISION_KEY(6, 1, 1);
    printf("%d", trans_table[x]);
}

// I have 28 transisions
// 3 pieces of information defines these states
// 7 states can be coded into 3 bits
// 2 bool events can be coded into 2 bits
// we just need a state table of length 2^5 = 32