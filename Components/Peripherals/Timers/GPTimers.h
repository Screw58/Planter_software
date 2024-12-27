/*!
*******************************************************
* \file: GPTimers.h
* \date: 26.12.2024
* \author: 26.12.2024
* \brief: General purpose timers interfaces used for LED and Buzzer periodic actions
********************************************************/

#ifndef GPTIMERS_H_
#define GPTIMERS_H_
//======================================[INCLUDES]======================================//
#include "driver/gptimer_types.h"
//=================================[MACROS AND DEFINES]=================================//

//======================================[TYPEDEFS]======================================//
/*!
 * @brief LED parameters structure used in the interrupt
 */
typedef struct {
   uint8_t led_blink_counter;
   uint8_t led_blink_val;
   uint32_t pin_level;
} ledtimer_cbk_params_t;
//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//
/*!
 * \brief: Configure and initialization new timer for LED blinking control
 * \details: Set APB clock as default
 *           Set counting up
 *           Set resolution to frequency given as param - in this case it's 1ms
 */
void GPTimer_LedTimerConfig(uint32_t frequency_Hz);

/*!
 * \brief: Start LED timer and set up the interrupt
 * \details: Configure the perdiod to the time in ms passes as 1st argument
 *           Set the callback function which will be executed when the interrupt come up
 *           and assign the parameters that will be used in the interrupt by this function
 *           Enable and start the LED timer
 */
void GPTimer_LedTimerStart(uint16_t blink_period_ms, ledtimer_cbk_params_t *const cbk_params);

/*!
 * \brief: Register the LED interrupt callback function
 * \details: Assign the function address from LED module
 */
void GPTimer_LedTimerCallbackRegister(void *user_callback);

/*!
 * \brief: Stop and disable LED timer
 * \details: Should be executed when LED blinking is finished
 */
void GPTimer_LedTimerStop(void);


#endif