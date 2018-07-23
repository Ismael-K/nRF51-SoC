#include "twi_MPL_115A2.h"
#include "twi_master.h"
#include "nrf.h"
#include "stdint.h"
#include "nrf_gpio.h"


/** set outputs to high */
// function used for a "write" condition or a "stop" condition
void twi_set_sda_high(uint8_t sda){
	/**< NOTE: if just using OUTSET does not work, refer back to twi_master and copy the rest */
	NRF_GPIO->OUTSET = sda; 	
}
// function used for start condition
void twi_set_scl_high(uint8_t scl){ 	
	/**< NOTE: if just using OUTSET does not work, refer back to twi_master and copy the rest */
	NRF_GPIO->OUTSET = scl; 	
}
/**< set outputs to low */
void twi_set_sda_low(uint8_t sda){
	/**< NOTE: if just using OUTSET does not work, refer back to twi_master and copy the rest */
	NRF_GPIO->OUTCLR = sda; 	
}

void twi_set_scl_low(uint8_t scl){ 	
	/**< NOTE: if just using OUTSET does not work, refer back to twi_master and copy the rest */
	NRF_GPIO->OUTCLR = scl; 	
}

void twi_set_as_input(uint8_t sda)
{
	nrf_gpio_cfg_input(sda, NRF_GPIO_PIN_PULLUP); // sets input NRF_GPIO_PIN_NOPULL
}

//*< pin configuration */


//		nrf_gpio_cfg_output(sda_pin_number); //may or may not be needed
//		nrf_gpio_cfg_output(scl_pin_number); //may or may not be needed
//	
//		NRF_TWI0->PSELSDA = sda_pin_number; // select as SDA
//    NRF_TWI0->PSELSCL = scl_pin_number; // select as SCL
//	  NRF_TWI0->ENABLE  = TWI_ENABLE_ENABLE_Enabled; // enabling I2C
//	
//		twi_set_sda_high(sda_pin_number);
//		twi_set_scl_high(scl_pin_number);	
//}

	
