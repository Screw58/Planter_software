/*!
******************************************************************
* \file:
* \date:
* \author:
* \brief:
******************************************************************/

//======================================[INCLUDES]======================================//
#include "led.h"
//==================================[EXTERN VARIABLES]==================================//

//=============================[PRIVATE MACROS AND DEFINES]=============================//

//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//
static led_rgb_cfg_t led_rgb[LED_PIN_MAX];
//==================================[GLOBAL VARIABLES]==================================//

//=============================[LOCAL FUNCTION PROTOTYPES]==============================//

//==================================[LOCAL FUNCTIONS]===================================//

//==================================[GLOBAL FUNCTIONS]==================================//

led_colour_cfg_t led_colours_matrix[LED_COLOUR_MAX] = {
   {255, 0,   0  },
   { 0,  255, 0  },
   { 0,  0,   255}
};


void LED_Init(void)
{
   led_rgb[LED_PIN_RED].led_gpio_num = LED_RGB_IO_RED;
   led_rgb[LED_PIN_RED].led_channel = LED_RGB_CHANNEL_RED;
   led_rgb[LED_PIN_GREEN].led_gpio_num = LED_RGB_IO_GREEN;
   led_rgb[LED_PIN_GREEN].led_channel = LED_RGB_CHANNEL_GREEN;
   led_rgb[LED_PIN_BLUE].led_gpio_num = LED_RGB_IO_BLUE;
   led_rgb[LED_PIN_BLUE].led_channel = LED_RGB_CHANNEL_BLUE;

   ledc_timer_config_t led_timer_cfg = {
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .duty_resolution = LEDC_TIMER_8_BIT,
      .timer_num = LEDC_TIMER_0,
      .freq_hz = 50,
      .clk_cfg = LEDC_AUTO_CLK,
   };
   ESP_ERROR_CHECK(ledc_timer_config(&led_timer_cfg));


   ledc_channel_config_t led_channel_cfg = {
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER_0,
      .duty = 0,
   };

   for(led_rgb_pins_t led = LED_PIN_RED; led < LED_PIN_MAX; led++)
   {
      led_channel_cfg.gpio_num = led_rgb[led].led_gpio_num;
      led_channel_cfg.channel = led_rgb[led].led_channel;

      ledc_channel_config(&led_channel_cfg);
   }
}

void LED_set_colour(led_colours_t led_colour)
{
   ledc_set_duty(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_RED].led_channel, led_colours_matrix[led_colour].led_duty_red);
   ledc_update_duty(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_RED].led_channel);

   ledc_set_duty(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_GREEN].led_channel, led_colours_matrix[led_colour].led_duty_green);
   ledc_update_duty(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_GREEN].led_channel);

   ledc_set_duty(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_BLUE].led_channel, led_colours_matrix[led_colour].led_duty_blue);
   ledc_update_duty(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_BLUE].led_channel);
}