#include "stdio.h"
#include "stdint.h"
#include "scheduler.h"

int system_time()
{
    static uint64_t time = 0;
    return time++;
}

int main(void)
{
    while (1)
    {
        // trigger the scheduler every X times (this number has no significance)
        if (system_time() % 5000000 == 0) {
            scheduler_trigger();
        }
        scheduler_process();
    }
} 