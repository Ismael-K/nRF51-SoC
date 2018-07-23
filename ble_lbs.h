/* Copyright (c) 2013 Nordic Semiconductor. All Rights Reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the license.txt file.
 */

#ifndef BLE_LBS_H__
#define BLE_LBS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

#define LBS_UUID_BASE {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00} // service
#define LBS_UUID_SERVICE 0x1523 // service BLE_UUID_TEMPERATURE_MEASUREMENT_CHAR
#define LBS_UUID_LED_CHAR 0x1525 // characteristic
#define LBS_UUID_USER_INPUT_PARAM_CHAR 0x1526 // characteristic
#define LBS_UUID_IMAGE_CHAR 0x1527 // characteristic FOR AN IMAGE
#define LBS_UUID_BUTTON_CHAR 0x1524 // characteristic

#define MAX_TX_BYTES				 20

// Forward declaration of the ble_lbs_t type. 
typedef struct ble_lbs_s ble_lbs_t;

/*< start of the testing of write handling to extract information and create own version of handling multiple inputs for different sensors 
 * uncomment everything in between the "stop"
 */
/*<************ START ***************/

typedef void (*ble_lbs_led_write_handler_t) (ble_lbs_t * p_lbs, uint8_t led_state); // changed from new_state to led_state

typedef struct
{
    ble_lbs_led_write_handler_t led_write_handler;  /**< Event handler to be called when LED characteristic is written. */
} ble_lbs_init_t;

/*<************ STOP ***************/


/**@brief LED Button Service structure. This contains various status information for the service. */
typedef struct ble_lbs_s
{
    uint16_t                    service_handle; // handle for the whole service
    ble_gatts_char_handles_t    led_char_handles; // handle for the led characteristic
    ble_gatts_char_handles_t    button_char_handles; //for the button charactersitic
		ble_gatts_char_handles_t    temperature_char_handles; // characteristic handle for temperature
		ble_gatts_char_handles_t 		user_input_write_char_handles;// characteristic handle used for characteristic used for user-input
 		ble_gatts_char_handles_t    image_char_handles; //
		uint8_t                     uuid_type;
    uint16_t                    conn_handle; // the handle for BLE connection
    
		/*< the following is just for the purpose of extracting information and to try and create my own version */
		ble_lbs_led_write_handler_t led_write_handler;
} ble_lbs_t;

/**@brief Function for initializing the LED Button Service.
 *
 * @param[out]  p_lbs       LED Button Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_lbs_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_lbs_init(ble_lbs_t * p_lbs, const ble_lbs_init_t * p_lbs_init); // erase second parameter to go back to original date: 3/20/15

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the LED Button Service.
 *
 *
 * @param[in]   p_lbs      LED Button Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_lbs_on_ble_evt(ble_lbs_t * p_lbs, ble_evt_t * p_ble_evt);

/**@brief Function for sending a button state notification */
uint32_t ble_lbs_on_button_change(ble_lbs_t * p_lbs, uint8_t button_state);
uint32_t ble_send_temp(ble_lbs_t * p_lbs, uint8_t data);
uint32_t ble_send_image(ble_lbs_t * p_lbs, uint8_t *data, uint16_t length); //volatile uint16_t data


#endif // BLE_LBS_H__

/** @} */
