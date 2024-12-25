/*!
*******************************************************
* \file: led.h
* \date: 07.09.2024
* \author: Jacek Twardoch
* \brief:
********************************************************/
#ifndef LED_H_
#define LED_H_

//======================================[INCLUDES]======================================//
// #include "driver/gpio.h"
#include "driver/gptimer.h"
#include "driver/gptimer_etm.h"
#include "driver/gptimer_types.h"
#include "driver/ledc.h"
#include "hal/timer_types.h"

//=================================[MACROS AND DEFINES]=================================//
#define LED_RGB_IO_RED        GPIO_NUM_17
#define LED_RGB_CHANNEL_RED   LEDC_CHANNEL_0
#define LED_RGB_IO_GREEN      GPIO_NUM_18
#define LED_RGB_CHANNEL_GREEN LEDC_CHANNEL_1
#define LED_RGB_IO_BLUE       GPIO_NUM_19
#define LED_RGB_CHANNEL_BLUE  LEDC_CHANNEL_2

#define LED_RGB_VCC_PIN       GPIO_NUM_16

//======================================[TYPEDEFS]======================================//
typedef enum
{
   LED_PIN_RED = 0,
   LED_PIN_GREEN,
   LED_PIN_BLUE,
   LED_PIN_MAX,
} led_rgb_pins_t;

typedef enum
{
   LED_COLOUR_RED = 0,
   LED_COLOUR_GREEN,
   LED_COLOUR_BLUE,
   LED_COLOUR_PINK,
   LED_COLOUR_ORANGE,
   LED_COLOUR_CYAN,
   LED_COLOUR_MAX,
} led_colours_t;

typedef struct {
   int led_gpio_num;
   ledc_channel_t led_channel;
} led_rgb_cfg_t;

typedef struct {
   uint8_t led_duty_red;
   uint8_t led_duty_green;
   uint8_t led_duty_blue;
} led_colour_cfg_t;
//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//
/*!
 * \brief: Read given channel of ADC and return value in mV unit
 * \details:
 */
void LED_Init(void);

/*!
 * \brief: Read given channel of ADC and return value in mV unit
 * \details:
 */
void Turn_LED_On(led_colours_t colour);

/*!
 * \brief: Read given channel of ADC and return value in mV unit
 * \details:
 */
void Turn_LED_Off(void);

/*!
 * \brief: Read given channel of ADC and return value in mV unit
 * \details:
 */
void Blink_LED(led_colours_t colour, uint8_t blink_num, uint16_t blink_period_ms);

/*!
 * \brief: Read given channel of ADC and return value in mV unit
 * \details:
 */
bool LED_Timer_Alarm(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx);

#endif
