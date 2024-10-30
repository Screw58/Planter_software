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
static led_rgb_cfg_t led_rgb[LED_PIN_MAX];
static uint8_t led_blink_counter;
static uint8_t colour = 0;
static gptimer_handle_t timer_handle;
static uint32_t pin_level = 0;
static uint8_t led_blink_val = 0;

static led_colour_cfg_t led_colours_matrix[LED_COLOUR_MAX] = {
   {255,  0,   0  }, // RED
   { 0,   255, 0  }, // GREEN
   { 0,   0,   255}, // BLUE
   { 255, 0,   255}, // PINK
   { 255, 165, 0  }, // ORANGE
   { 0,   255, 255}, // CYAN
};

gptimer_event_callbacks_t cbs = {
   .on_alarm = LED_Timer_Alarm,  // register user callback
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
static void LED_Timer_Init(void);

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
   gpio_config_t gpio_cfg = {
      .pin_bit_mask = (1ULL << LED_RGB_VCC_PIN),
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
   };
   gpio_config(&gpio_cfg);
}

/* Timer Configuration */
static void LED_Timer_Init(void)
{
   gptimer_config_t timer_cfg = {
      .clk_src = GPTIMER_CLK_SRC_DEFAULT,
      .direction = GPTIMER_COUNT_UP,
      .resolution_hz = 1 * 1000 * 1000,
   };
   ESP_ERROR_CHECK(gptimer_new_timer(&timer_cfg, &timer_handle));
}

static void LED_Timer_Start(uint8_t blink_val, uint16_t blink_period_ms)
{
   gptimer_alarm_config_t tim_alarm_cfg = {
      .alarm_count = blink_period_ms * 1000,
      .reload_count = 0,
      .flags.auto_reload_on_alarm = true,
   };
   gptimer_set_alarm_action(timer_handle, &tim_alarm_cfg);
   led_blink_counter = 0;
   pin_level = 0;
   led_blink_val = blink_val;
   ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer_handle, &cbs, (void *)&led_blink_val));
   ESP_ERROR_CHECK(gptimer_enable(timer_handle));
   ESP_ERROR_CHECK(gptimer_start(timer_handle));
}

bool LED_Timer_Alarm(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
   uint8_t blink_amount = 0;
   uint8_t blink_period_max = 0;

   blink_amount = *(uint8_t *)user_ctx;
   blink_period_max = (blink_amount * 2) - 1;

   if(led_blink_counter < blink_period_max)
   {
      pin_level ^= 1;
      gpio_set_level(LED_RGB_VCC_PIN, pin_level);
      led_blink_counter++;
   }
   else
   {
      Turn_LED_Off();
      // turn off the counter
      gptimer_stop(timer_handle);
      gptimer_disable(timer_handle);
      // Turn_LED_Off();
      led_blink_val = 0;
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
   LED_Timer_Init();
}


void Turn_LED_On(led_colours_t colour)
{
   LED_set_colour(colour);
   gpio_set_level(LED_RGB_VCC_PIN, GPIO_LEVEL_HIGH);
}

void Turn_LED_Off(void)
{
   gpio_set_level(LED_RGB_VCC_PIN, GPIO_LEVEL_LOW);
   ledc_stop(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_RED].led_channel, 0);
   ledc_stop(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_GREEN].led_channel, 0);
   ledc_stop(LEDC_LOW_SPEED_MODE, led_rgb[LED_PIN_BLUE].led_channel, 0);
}

void Blink_LED(led_colours_t colour, uint8_t blink_num, uint16_t blink_period_ms)
{
   LED_set_colour(colour);
   LED_Timer_Start(blink_num, blink_period_ms);
}
