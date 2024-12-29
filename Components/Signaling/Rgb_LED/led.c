/*!
******************************************************************
* \file: led.c
* \date: 07.09.2024
* \author: Jacek Twardoch
* \brief:
******************************************************************/

//======================================[INCLUDES]======================================//
#include "led.h"
#include "gpio_user.h"

//==================================[EXTERN VARIABLES]==================================//

//=============================[PRIVATE MACROS AND DEFINES]=============================//

//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//
/*!
 * @brief Array to control LED RGB pins and their PWM channels
 */
static led_rgb_cfg_t led_rgb[LED_PIN_MAX];


ledtimer_cbk_params_t ledtimer_cbk_params;

/*!
 * @brief Array that keeps timer PWM values of each RGB pins for defined colours
 */
static led_colour_cfg_t led_colours_matrix[LED_COLOUR_MAX] = {
   {255,  0,   0  }, // RED
   { 0,   255, 0  }, // GREEN
   { 0,   0,   255}, // BLUE
   { 255, 0,   255}, // PINK
   { 255, 165, 0  }, // ORANGE
   { 0,   255, 255}, // CYAN
};


//==================================[GLOBAL VARIABLES]==================================//

//=============================[LOCAL FUNCTION PROTOTYPES]==============================//
/*!
 * \brief: Read given channel of ADC and return value in mV unit
 * \details:
 */
static void LED_IO_Init(void);

/*!
 * \brief: Read given channel of ADC and return value in mV unit
 * \details:
 */
static void LED_Timer_Init(uint32_t frequency_Hz);

/*!
 * \brief: Read given channel of ADC and return value in mV unit
 * \details:
 */
static void LED_Timer_Start(uint8_t blink_val, uint16_t blink_period_ms);

/*!
 * \brief: Read given channel of ADC and return value in mV unit
 * \details:
 */
bool LED_Timer_Alarm(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx);

/*!
 * \brief: Read given channel of ADC and return value in mV unit
 * \details:
 */
static void LED_set_colour(led_colours_t led_colour);

//==================================[LOCAL FUNCTIONS]===================================//
static void LED_IO_Init(void)
{
   GPIO_VccLedPinInit(LED_RGB_VCC_PIN);
}

/* Timer Configuration */
static void LED_Timer_Init(uint32_t frequency_Hz)
{
   GPTimer_LedTimerConfig(frequency_Hz);
   GPTimer_LedTimerCallbackRegister(LED_Timer_Alarm);
}

static void LED_Timer_Start(uint8_t blink_val, uint16_t blink_period_ms)
{

   ledtimer_cbk_params.led_blink_counter = 0;
   ledtimer_cbk_params.pin_level = 0;
   ledtimer_cbk_params.led_blink_val = blink_val;
   GPTimer_LedTimerStart(blink_period_ms, &ledtimer_cbk_params);
}

bool LED_Timer_Alarm(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{

   uint8_t blink_period_max = 0;
   ledtimer_cbk_params_t *cbk_params;

   cbk_params = (ledtimer_cbk_params_t *)user_data;

   blink_period_max = (cbk_params->led_blink_val * 2) - 1;


   if(cbk_params->led_blink_counter < blink_period_max)
   {
      // cbk_params->pin_level ^= 1;
      GPIO_TogglePin(LED_RGB_VCC_PIN, &cbk_params->pin_level);
      cbk_params->led_blink_counter++;
   }
   else
   {
      Turn_LED_Off();
      // turn off the counter
      GPTimer_LedTimerStop();
      cbk_params->led_blink_val = 0;
   }

   return true;
}

static void LED_set_colour(led_colours_t led_colour)
{
   ledc_set_duty(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_RED].led_channel, led_colours_matrix[led_colour].led_duty_red);
   ledc_update_duty(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_RED].led_channel);

   ledc_set_duty(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_GREEN].led_channel, led_colours_matrix[led_colour].led_duty_green);
   ledc_update_duty(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_GREEN].led_channel);

   ledc_set_duty(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_BLUE].led_channel, led_colours_matrix[led_colour].led_duty_blue);
   ledc_update_duty(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_BLUE].led_channel);
}


//==================================[GLOBAL FUNCTIONS]==================================//

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
      // ledc_stop(LEDC_LOW_SPEED_MODE, led_rgb[led].led_channel, 0);
   }

   LED_IO_Init();
   LED_Timer_Init(1000);
}


void Turn_LED_On(led_colours_t colour)
{
   LED_set_colour(colour);
   GPIO_SetPinHigh(LED_RGB_VCC_PIN);
}

void Turn_LED_Off(void)
{
   // gpio_set_level(LED_RGB_VCC_PIN, GPIO_LEVEL_LOW);
   GPIO_SetPinLow(LED_RGB_VCC_PIN);
   ledc_stop(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_RED].led_channel, 0);
   ledc_stop(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_GREEN].led_channel, 0);
   ledc_stop(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_BLUE].led_channel, 0);
}

void Blink_LED(led_colours_t colour, uint8_t blink_num, uint16_t blink_period_ms)
{
   LED_set_colour(colour);
   LED_Timer_Start(blink_num, blink_period_ms);
}
