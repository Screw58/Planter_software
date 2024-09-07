/*!
*******************************************************
* \file:
* \date:
* \author:
* \brief:
********************************************************/
#ifndef LED_H_
#define LED_H_

//======================================[INCLUDES]======================================//
#include "driver/ledc.h"
//=================================[MACROS AND DEFINES]=================================//
#define LED_RGB_IO_RED        GPIO_NUM_17
#define LED_RGB_CHANNEL_RED   LEDC_CHANNEL_0
#define LED_RGB_IO_GREEN      GPIO_NUM_18
#define LED_RGB_CHANNEL_GREEN LEDC_CHANNEL_1
#define LED_RGB_IO_BLUE       GPIO_NUM_19
#define LED_RGB_CHANNEL_BLUE  LEDC_CHANNEL_2
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

#endif