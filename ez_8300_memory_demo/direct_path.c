#include "mpo.h"

int FIFO_A1[512];

int main()
{
    // maybe this will return gains or it will update the gain state by itself
    mpo_run(FIFO_A1);   
}