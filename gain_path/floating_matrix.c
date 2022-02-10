#define INPUT_BAND_LENGTH 12
#define OUTPUT_BAND_LENGTH 4
#define MAX_WINDOW_LENGTH 5

int window[MAX_WINDOW_LENGTH] = {2, 1, 0, 1, 2};
int window_start_index[MAX_WINDOW_LENGTH];
int window_length[MAX_WINDOW_LENGTH];

int window_offset[INPUT_BAND_LENGTH];

int get_value(int x, int y)
{
    if (window_offset[x] > y)
        return 0;   // window hasn't started yet

    if (window_length[x] + window_offset[x] < y)
        return 0;   // window is already finished

    int norm_index = x - window_offset[x];
    return window[window_start_index[x] + norm_index];
}

int add_window(int* window, int len)
{

}

int main(void) {
    int matrix_trans[12] = {0, 0, 1, 2, 3, 4};
}