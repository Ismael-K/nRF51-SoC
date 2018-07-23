
#include "twi_TMP_102.h"
#include "twi_master.h"



#define TMP_102_ADDRESS 								0x90
#define ONESHOT_MODE    					      0x01 //!< Bit in configuration register for 1-shot mode 

const uint8_t command_read_temp          = 0x00; //!< Reads last converted temperature value from temperature register
const uint8_t command_access_config			 = 0x01; //!< Reads or writes configuration data to configuration register 
const uint8_t command_Tlow							 = 0x02; //!< low temp register read/write
const uint8_t command_Thi								 = 0x03; //!< high temp register read/write

static uint8_t tmp_102_config_read(void)
{
    uint8_t config = 1;

    // Write: command protocol
    if (twi_master_transfer(TMP_102_ADDRESS, (uint8_t*)&command_access_config, 1, TWI_DONT_ISSUE_STOP))
    {
        if (twi_master_transfer(TMP_102_ADDRESS | TWI_READ_BIT, &config, 1, TWI_ISSUE_STOP)) // Read: current configuration
        {
            // Read succeeded, configuration stored to variable "config"
        }
        else
        {
            // Read failed
            config = 0;
        }
    }

    return config;
}


bool tmp_102_init(){
    bool transfer_succeeded = true;

    uint8_t config = tmp_102_config_read();

    if (config != 0)
    {
        // Configure DS1624 for 1SHOT mode if not done so already.
        if (!(config & ONESHOT_MODE))
        {
            uint8_t data_buffer[2];

            data_buffer[0] = command_access_config;
            data_buffer[1] = ONESHOT_MODE;

            transfer_succeeded &= twi_master_transfer(TMP_102_ADDRESS, data_buffer, 2, TWI_ISSUE_STOP);
        }
    }
    else
    {
        transfer_succeeded = false;
    }

    return transfer_succeeded;
}

bool tmp_102_read(int8_t * temperature_p, int8_t * temperature_fraction_p){

	bool transfer_succeeded = false;
	
	if(!(transfer_succeeded)){ // arg = twi_master_transfer(TMP_102_ADDRESS, (uint8_t*)&command_read_temp, 1, TWI_DONT_ISSUE_STOP)
		
        uint8_t data_buffer[2];
/*< test variables	
				uint8_t arr_1 = data_buffer[0];//test var
				uint8_t arr_2 = data_buffer[1]; //test var
*/
		
        // Read: 2 temperature bytes to data_buffer
        if (twi_master_transfer(TMP_102_ADDRESS | TWI_READ_BIT, data_buffer, 2, TWI_ISSUE_STOP)) 
        {
            *temperature_p = (int8_t)data_buffer[0];//(int8_t)data_buffer[0]; //(int8_t)
            *temperature_fraction_p = (int8_t)data_buffer[1];//(int8_t)data_buffer[1]; //(int8_t)
/*< test variables					
						int8_t dummy1 = *temperature_t;//test
						int8_t dummy2 = *temperature_fraction_t;//test
*/						

            transfer_succeeded = true;
        }
    }
		
	transfer_succeeded = true;
	return transfer_succeeded; 
}	

float tmp_102_dig_to_temp(int8_t * temperature_p, int8_t * temperature_fraction_p, bool TEMPERATURE_MODE){
	
	uint8_t data_buffer[2];
	float resolution = 0.0625;
	float scale_diff = 1.7999856;
	float temperature_in_celcius;
	float temperature_in_fahren;	
	/*< test variable 
	
	uint8_t test1 = 0x16;
	uint8_t test2 = 0x90;
	uint8_t temp1;
	uint8_t temp2;
	data_buffer[0] = test1;
	data_buffer[1] = test2;
	
	*/
	
	data_buffer[0] = *temperature_p; //dereferencing to get value
	data_buffer[1] = *temperature_fraction_p; //dereferencing to get value
	
	/*< test variable */
	uint8_t temp1 = data_buffer[0];
	uint8_t temp2 = data_buffer[1];
	
	
	
	if( (temp1&0x80) > 0)	//To read neg. temps, check if MSB is 1 
	{
			temp1 = data_buffer[0] | 0xF00;		//sign-extend
	}
	///////* by -1 
	
		
	int16_t val1 = temp1<<4; // not sure why we have to cast
	int16_t val2 = temp2>>4;
	
	
	
	int val3 = val1 | val2;
	
	temperature_in_celcius = (float)((float)val3*resolution);
	
	temperature_in_fahren  = ((temperature_in_celcius)*scale_diff + 32); // formula" (temp_cel - 32)*(5/9)
	

	if(TEMPERATURE_MODE){
		return temperature_in_celcius;
	}

	return temperature_in_fahren;	
}

float temp_meas_update(int8_t * temperature_p, int8_t * temperature_fraction_p, bool TEMPERATURE_MODE){
	
	bool  temp_read_succeeded;
	float temperature;
//	temp_init_succeeded = tmp_102_init();
	temp_read_succeeded = tmp_102_read(temperature_p, temperature_fraction_p);

	if(temp_read_succeeded){
		temperature = tmp_102_dig_to_temp(temperature_p, temperature_fraction_p, TEMPERATURE_MODE);
	}
	return temperature;
	
}
