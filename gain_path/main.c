#include "stdio.h"
#include "math.h"
// #include "gain_path.h"

// Description:
// The input is a bandsum that is windowed and added together across many points in time.
// The shifts in time are calculated from the ramp and spacing paramters. This dictates the overlaping of the bands.
// The module will pre-calculate a bunch of helper data to make calcuting the bandsum efficient during runtime.


// TODO:
// 1. create a single window for each bandsum matrix.
// 1.1 use a LUT during calculation
// 5. 


#define MAX_WINDOW_SIZE 12
#define MAX_NUM_BANDSUM_MATRICES 3
#define MAX_BAND_LENGTH 12
#define MAX_TOTAL_BAND_OVERLAP 12

typedef struct
{
    double bandsum_scale[MAX_BAND_LENGTH];
    double banddist_scale[MAX_BAND_LENGTH];
    double banddist_sum[MAX_BAND_LENGTH+MAX_TOTAL_BAND_OVERLAP];

    int window[MAX_BAND_LENGTH][MAX_WINDOW_SIZE];
    int window_start_index[MAX_BAND_LENGTH];
    int window_length[MAX_BAND_LENGTH];
    int window_offset[MAX_BAND_LENGTH];

    int n_output_bands;     // as defined for a bandsum (e.g 4)
    int n_input_bands;      // as defined for a bandsum (e.g 12)
    // for dist
    int window_indexes[MAX_BAND_LENGTH];
    int dist_count[MAX_BAND_LENGTH];
} bandsum_matrix;

static int bandsum_matrix_table_index = 0;
static bandsum_matrix bandsum_matrix_table[MAX_NUM_BANDSUM_MATRICES] = {};

bandsum_matrix* bandsum_init(int* band_matrix, int band_mtx_size, int ramp, int spacing)
{
    bandsum_matrix* result = &bandsum_matrix_table[bandsum_matrix_table_index++];
    
    for(int i = 0; i < band_mtx_size; i++)
    {
        result->n_input_bands += band_matrix[i];
        if (i != band_mtx_size-1)
        {
            result->n_input_bands += spacing;
        }
    }

    result->n_output_bands = band_mtx_size;

    int agg_bands = band_matrix[0];
    result->window_start_index[0] = ramp;
    for(int i = 1; i < band_mtx_size; i++)
    {
        result->window_offset[i] = agg_bands - ramp + spacing;
        agg_bands += band_matrix[i] + spacing;
    }

    // Calculate the different window lengths
    result->window_length[0] = band_matrix[0] + (1 * ramp);
    for (int i = 1; i < band_mtx_size - 1; i++)
    {
        result->window_length[i] = band_matrix[i] + (2 * ramp);
    }
    result->window_length[band_mtx_size-1] = band_matrix[band_mtx_size-1] + (1 * ramp);

    // Create the window matrix
    for(int i = 0; i < band_mtx_size; i++)
    {
        int ramp_cnt = ramp;
        int band_cnt = band_matrix[i];
        for (int j = 0; j < band_matrix[i] + (2 * ramp); j++)
        {
            if (ramp_cnt > 0)
            {
                result->window[i][j] = ramp_cnt--;
            }
            else if (ramp_cnt == 0 && band_cnt-- != 0)
            {
                result->window[i][j] = 0;
            }
            else
            {
                result->window[i][j] = -(--ramp_cnt);
            }
        }

    }

    for(int i = 0; i < band_mtx_size; i++)
    {
        double scale = 0;
        for(int j = 0; j < result->window_length[i]; j++)
        {
            // This line should be replaced with a right shift when we move to fixed point numbers
            scale += 1/pow(2, result->window[i][result->window_start_index[i] + j]);
        }
        result->bandsum_scale[i] = 1/scale;
    }

    // Sum the bandsum matrix around axis = 1.
    // 1 / np.sum(bdmtx, axis=1)
    for(int i = 0; i < band_mtx_size; i++)
    {
        for(int j = 0; j < result->window_length[i]; j++) {
            // This line should be replaced with a right shift when we move to fixed point numbers
            result->banddist_scale[result->window_offset[i]+j] += 1/pow(2, result->window[i][result->window_start_index[i]+j]);
        }
    }

    // Determing how many bands should be included in each output band during band distribution
    for (int i = 0; i < result->n_output_bands; i++)
    {
        for(int j = 0; j < result->window_length[i]; j++) {
            result->dist_count[result->window_offset[i] + j]++;
        }
    }


    // Calculate window index for calculating banddist
    int temp[band_mtx_size];
    temp[0] = result->window_length[0];
    int last_window_end_index = result->window_length[0];
    for (int y = 1; y < band_mtx_size; y++)
    {
        temp[y] = (result->window_length[y] + result->window_offset[y] - last_window_end_index);
        last_window_end_index = result->window_length[y] + result->window_offset[y];
    }

    int seed = 0;
    int idx = 0;
    for (int x = 0; x < band_mtx_size; x++)
    {
        for(int i = 0; i < temp[x]; i++)
        {
            result->window_indexes[idx++] = seed;
        }
        seed++;
    }

    // Calculate scale when distributing bands
    int scale_factor_idx = 0;
    for (int i = 0; i < result->n_input_bands; i++)
    {
        for(int j = 0; j < result->dist_count[i]; j++) {
            int curr_window_index = result->window_indexes[i] + j;                                                      // what window are we grabbing data from, (this also tells us what input band we are looking at)
            int window_shifts = result->window_offset[result->window_indexes[i] + j];                                   // how much is the window shifted over the input bands
            int window_value_index = result->window_start_index[curr_window_index] + i - window_shifts;                 // index for the value in the window we care about on this loop
            double scale_factor = 1.0/pow(2,(result->window[result->window_indexes[i]][window_value_index]));           // calculate scale factor using the window value
            double same_f_norm = result->banddist_scale[i];                                                             // obtain the normalizing factor for this output
            result->banddist_sum[scale_factor_idx++] = (scale_factor/same_f_norm);
        }
    }

    return result;
}

void bandsum(bandsum_matrix* bandsum_matrix, double* input_bands, double* output_bands)
{
    for (int i = 0; i < bandsum_matrix->n_output_bands; i++)
    {
        int sum = 0;
        for(int j = 0; j < bandsum_matrix->window_length[i]; j++) {
            // This line should be replaced with a right shift when we move to fixed point numbers
            sum += input_bands[bandsum_matrix->window_offset[i] + j] / pow(2,(bandsum_matrix->window[i][bandsum_matrix->window_start_index[i] + j]));
        }
        output_bands[i] = sum * bandsum_matrix->bandsum_scale[i];
    }
}

void banddist(bandsum_matrix* bandsum_matrix, double* input_bands, double* output_bands)
{
    int scale_index = 0;
    for (int i = 0; i < bandsum_matrix->n_input_bands; i++)
    {
        int sum = 0;
        for(int j = 0; j < bandsum_matrix->dist_count[i]; j++) {
            int curr_window_index = bandsum_matrix->window_indexes[i] + j;
            sum += input_bands[curr_window_index] * bandsum_matrix->banddist_sum[scale_index++];
        }
        output_bands[i] = sum;
    }
}



// int main(void) {
//     double input_powers[12] = {23, 34, 45, 23, 45, 23, 34, 23, 32, 43, 12, 23};
//     double output_power[4] = {};
//     int band_matrix[4] = {3, 3, 3, 3};
//     bandsum_matrix* mtx = bandsum_init(band_matrix, 4, 3, 0);
//     bandsum(mtx, input_powers, output_power);
//     int band_matrix_spread[4] = {2, 1, 1, 2};
//     double input_pow[4] = {33.26666667, 32.16666667, 30.27777778, 26.6};
//     bandsum_matrix* mtx_dist = bandsum_init(band_matrix_spread, 4, 2, 2);
//     double output_power_12ch[12] = {};
//     banddist(mtx_dist, input_pow, output_power_12ch);
// }
