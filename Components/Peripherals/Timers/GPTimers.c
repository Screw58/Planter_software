/*!
******************************************************************
* \file: GPTimers.c
* \date: 26.12.2024
* \author: 26.12.2024
* \brief: General purpose timers interfaces used for LED and Buzzer periodic actions
******************************************************************/


//======================================[INCLUDES]======================================//
#include "driver/gptimer.h"

#include "GPTimers.h"
//==================================[EXTERN VARIABLES]==================================//

//=============================[PRIVATE MACROS AND DEFINES]=============================//

//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//
/*!
 * @brief Timer handler used for LED blinking
 */
static gptimer_handle_t LedTimer_handle;

/*!
 * @brief LED timer event callback
 */
gptimer_event_callbacks_t LedTimer_InterruptCallback;

//==================================[GLOBAL VARIABLES]==================================//

//=============================[LOCAL FUNCTION PROTOTYPES]==============================//

//==================================[LOCAL FUNCTIONS]===================================//

//==================================[GLOBAL FUNCTIONS]==================================//
void GPTimer_LedTimerConfig(uint32_t frequency_Hz)
{
   gptimer_config_t timer_cfg = {
      .clk_src = GPTIMER_CLK_SRC_DEFAULT,
      .direction = GPTIMER_COUNT_UP,
      .resolution_hz = 1 * 1000 * frequency_Hz,
   };
   ESP_ERROR_CHECK(gptimer_new_timer(&timer_cfg, &LedTimer_handle));
}

void GPTimer_LedTimerStart(uint16_t blink_period_ms, ledtimer_cbk_params_t *const cbk_params)
{
   gptimer_alarm_config_t LedTimer_alarm_cfg = {
      .alarm_count = blink_period_ms * 1000,
      .reload_count = 0,
      .flags.auto_reload_on_alarm = true,
   };
   gptimer_set_alarm_action(LedTimer_handle, &LedTimer_alarm_cfg);
   ESP_ERROR_CHECK(gptimer_register_event_callbacks(LedTimer_handle, &LedTimer_InterruptCallback, (void *)cbk_params));
   ESP_ERROR_CHECK(gptimer_enable(LedTimer_handle));
   ESP_ERROR_CHECK(gptimer_start(LedTimer_handle));
}

void GPTimer_LedTimerCallbackRegister(void *user_callback)
{
   LedTimer_InterruptCallback.on_alarm = user_callback;
}

void GPTimer_LedTimerStop(void)
{
   gptimer_stop(LedTimer_handle);
   gptimer_disable(LedTimer_handle);
}