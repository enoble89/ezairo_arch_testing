#include <stdint.h>

// Protobuf fitting packet
// Proto definition created from yaml
typedef struct 
{
    int version;    // incremented every time we add new values
    float threshold;    // found in ezairo X memory
    float kneepoint;    // found in ezairo Y memory
} ezairo_fitting_nr_data;

// maybe this can connected to region_id
// need a way to identify the fitting data
typedef enum {
    NR
} FITTING_DATA_TYPE;    


// Let's pretend for now we have access
// to the state as a global variable
typedef struct
{
    int nr_mode;
    int tnr_mode;
    int ml_nr;
} earpiece_state_t;

earpiece_state_t current_state = {0, 0, 0};


ezairo_fitting_nr_data get_fit_from_spi() {
    ezairo_fitting_nr_data result;
    return result;
}


// already defined on cm3
typedef struct {
  uint8_t cmd;
  uint8_t offset;
  uint32_t num_bytes;
  uint8_t *byte_blob;
} ezairo_memory_request_t;


earpiece_state_t get_state_from_spi() {
    // new state received over whisper protobuf
    earpiece_state_t result;
    return result;
}

// write fit to flash
// how do we make the generic for each fitting struct?
// how do we write and save multiple versions of the fitting struct?
// Do we need to store the fit data with the mode?
void write_to_flash(void* data, FITTING_DATA_TYPE type) {
}

void* load_mode_from_flash(int mode, FITTING_DATA_TYPE type) {
    // search flash from mode
}


void ezairo_memory_enqueue(ezairo_memory_request_t request) {
    // already defined on the cm3
}

void int_to_bytes(int intput_data, uint8_t* output_data)
{
}


// Version 1
void write_nr_fit(ezairo_fitting_nr_data* fit_to_write) {
    // parsing code generated from ez_mem .yaml files
    // it takes a protobuf struct and turns it into ezairo memory requests
    // parsing code will attempt to create the minimal number
    // of ezairo memory requests as possible.

    // similar to https://github.com/whisperai/whisper_system/blob/07443afc200eba2e4e93fb7b82bf10ee25d5971b/firmware/dolly_ezairo/cm3/code/ez_wpb_interface.c#L5

    // create an ezairo_memory request
    if (fit_to_write->version == 1.0)
    {
        // all the data for this struct can be obtained from yaml file
        uint8_t data[3] = {0};
        int_to_bytes(fit_to_write->kneepoint, data);
        ezairo_memory_request_t request = {
            .byte_blob = data,
            .offset = 2,
            .num_bytes = 3,
            .cmd = 1
        };
        ezairo_memory_enqueue(request);
    } else (fit_to_write->version == 1.1)
    {
        // different parsing code now that we have new data
    }
}

int float_to_8_16(float input)

// Version 2
void write_nr_fit(ezairo_fitting_nr_data* fit_to_write) {
    // this approach is simple, but bypasses the ezairo_memory lib
    // this means the earpiece tasks won't be in the correct state for an update
    // we could extend the ezairo_memory interface to accept an address instead of a
    // cmd, offset, and num_bytes.
    if (fit_to_write->version == 1.0)
    {
        // we can calculate the specific address the memory should be written to
        *0x20810824 = float_to_8_16(fit_to_write->kneepoint);
        *0x20810828 = float_to_8_16(fit_to_write->threshold);
        
    }
}

int new_fit = 0;
int new_state = 0;

int main() {
    // write new fits if we receive them over spi
    if(new_fit)
    {
        ezairo_fitting_nr_data new_fit = get_fit_from_spi();
        write_to_flash(&new_fit, NR);
    }
    
    // If there is a new state, compare modes, load new modes from flash, write fit for the mode to ram
    if (new_state)
    {
        earpiece_state_t new_state = get_state_from_spi();
        if (current_state.nr_mode != new_state.nr_mode) {
            ezairo_fitting_nr_data* fit_to_write = (ezairo_fitting_nr_data*) load_mode_from_flash(new_state.nr_mode, NR);
            write_nr_fit(fit_to_write);
            current_state.nr_mode = new_state.nr_mode;
        }
    }
}
