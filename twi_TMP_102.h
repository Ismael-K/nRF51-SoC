#include "stdbool.h"
#include "stdlib.h"
#include "stdint.h"


static uint8_t tmp_102_config_read(void);
bool tmp_102_init(void);
bool tmp_102_read(int8_t * temperature_p, int8_t * temperature_fraction_p);
float tmp_102_dig_to_temp(int8_t * temperature_p , int8_t * temperature_fraction_p, bool TEMPERATURE_MODE);
float temp_meas_update(int8_t *temperature_p, int8_t *temperature_fraction_p,  bool TEMPERATURE_MODE);
