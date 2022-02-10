#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


typedef struct four_values {
  uint32_t input[4];
  uint32_t output[4];
} four_values_t;

typedef struct five_values {
  uint32_t input[5];
  uint32_t output[5];
} five_values_t;

void do_compute_output(const size_t value_count, uint32_t input[value_count], uint32_t output[value_count])
{
  for (size_t i = 0; i < value_count; i++) {
    printf("computing value at index %zu\n", i);
    output[value_count] = input[value_count];
  }
}

#define compute_output(values) _Generic(values, \
                                        four_values_t: do_compute_output(4, values.input, values.output),\
                                        five_values_t: do_compute_output(5, values.input, values.output))

int main(int argc, char *argv[])
{
  four_values_t four = { {1, 2, 3, 4} };
  five_values_t five = { {1, 2, 3, 4, 5} };
  printf("Four!\n");
  compute_output(four);

  printf("Five\n");
  compute_output(five);
  return 0;
}