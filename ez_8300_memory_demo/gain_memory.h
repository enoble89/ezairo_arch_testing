#include <stdint.h>

typedef struct hlc_gains
{
    uint32_t gains_64ch[64];
    uint32_t gains_12ch[12];
} gains_t;

extern volatile gains_t* gains;