/*!
******************************************************************
* \file:
* \date:
* \author:
* \brief:
******************************************************************/

//======================================[INCLUDES]======================================//

#include "app_wifi.h"
#include "driver/rtc_io.h"
#include "esp_log.h"
#include "UserConfig.h"
#include <time.h>

// user lib
#include "gpio_user.h"
#include "McuState.h"
#include "mqtt.h"

//==================================[EXTERN VARIABLES]==================================//
extern struct tm *time_and_date_ptr;
//=============================[PRIVATE MACROS AND DEFINES]=============================//
/*These are defined various times for debug purposes*/
#define TWELVE_HOURS_IN_US 43200000000ULL
#define ONE_HOUR_IN_US     3600000000ULL
#define TEN_MINS_IN_US     600000000ULL
#define ONE_MINS_IN_US     60000000ULL
//==================================[PRIVATE TYPEDEFS]==================================//

#define TWENTYFOURH_IN_MINS 1440
//==================================[STATIC VARIABLES]==================================//
int wakeup_debug_cause;
uint64_t wakeuptime_in_mins;
static HourAndMins_t *configtable_addr;
//==================================[GLOBAL VARIABLES]==================================//

//=============================[LOCAL FUNCTION PROTOTYPES]==============================//
/*!
 * \brief:
 * \details:
 */
static void InitWakeupConfigTable(void);

/*!
 * \brief:
 * \details:
 */
static uint64_t CalculateNextSleepTime(HourAndMins_t current_time);

/*!
 * \brief:
 * \details:
 */
static void Rtc_SetWakeUpSources(uint64_t timer_value, gpio_num_t button_pin);
//==================================[LOCAL FUNCTIONS]===================================//
static void InitWakeupConfigTable(void)
{
   configtable_addr = calloc(MEAS_EVENTS_AMOUNT, sizeof(HourAndMins_t));

#if MEAS_EVENTS_AMOUNT >= 1
   configtable_addr[0].hour = WAKEUP_EVENT_HOUR_1;
   configtable_addr[0].mins = WAKEUP_EVENT_MINS_1;
#endif
#if MEAS_EVENTS_AMOUNT >= 2
   configtable_addr[1].hour = WAKEUP_EVENT_HOUR_2;
   configtable_addr[1].mins = WAKEUP_EVENT_MINS_2;
#endif
#if MEAS_EVENTS_AMOUNT >= 3
   configtable_addr[2].hour = WAKEUP_EVENT_HOUR_3;
   configtable_addr[2].mins = WAKEUP_EVENT_MINS_3;
#endif
#if MEAS_EVENTS_AMOUNT >= 4
   configtable_addr[3].hour = WAKEUP_EVENT_HOUR_4;
   configtable_addr[3].mins = WAKEUP_EVENT_MINS_4;
#endif
#if MEAS_EVENTS_AMOUNT >= 5
   configtable_addr[4].hour = WAKEUP_EVENT_HOUR_4;
   configtable_addr[4].mins = WAKEUP_EVENT_MINS_4;
#endif
}

static uint64_t CalculateNextSleepTime(HourAndMins_t current_time)
{

   uint8_t nextpossibletime;
   uint64_t RetTime_us;
   int smallest_difference = TWENTYFOURH_IN_MINS;
   int time_difference;
   int current_hour = current_time.hour;
   int current_min = current_time.mins;
   int currenttime_in_mins = (current_hour * 60) + current_min;

   for(uint8_t x = 0; x < MEAS_EVENTS_AMOUNT; x++)
   {
      int next_event_time_in_mins = (configtable_addr[x].hour * 60) +
            configtable_addr[x].mins;  // przypisywać najpierw wartość z tablicy do jakiejs zmiennej i potem liczyc

      if(next_event_time_in_mins > currenttime_in_mins)
      {
         time_difference = next_event_time_in_mins - currenttime_in_mins;
      }
      else
      {
         time_difference = (TWENTYFOURH_IN_MINS - currenttime_in_mins) + next_event_time_in_mins;
      }

      if(time_difference < smallest_difference)
      {
         smallest_difference = time_difference;
      }
   }

   RetTime_us = smallest_difference * ONE_MINS_IN_US;

   return RetTime_us;
}

static void Rtc_SetWakeUpSources(uint64_t timer_value, gpio_num_t button_pin)
{
   rtc_gpio_pulldown_en(button_pin);                           // GPIO_NUM_4
   esp_sleep_enable_timer_wakeup(timer_value);
   esp_sleep_enable_ext0_wakeup(button_pin, GPIO_LEVEL_HIGH);  // GPIO_NUM_4
}
//==================================[GLOBAL FUNCTIONS]==================================//
void GoToSleep(void)
{
   HourAndMins_t SntpTime;
   uint64_t time_in_us;

   SntpTime = SNTP_ReadTime();
   time_in_us = CalculateNextSleepTime(SntpTime);  // zastanowić sie czy nie lepiej przekazywać jako parametr tego uinta64_t bo tak to
                                                   // trzeba chyba jakieś przerwania wyłączyć czy coś
   Mqtt_Disconnect();
   Wifi_Stop();
   ESP_LOGI("before_sleep", "calculated time for sleep: %llu ", time_in_us);
   Rtc_SetWakeUpSources(time_in_us, WAKEUP_BUTTON_PIN);

   esp_deep_sleep_start();
}

void CheckWakeUpReason(void)
{
   esp_sleep_wakeup_cause_t wakeup_cause = ESP_SLEEP_WAKEUP_UNDEFINED;
   wakeup_cause = esp_sleep_get_wakeup_cause();

   if(wakeup_cause == ESP_SLEEP_WAKEUP_TIMER)
   {
      // rtc timer triggered the wakeup - normal process
      wakeup_debug_cause = ESP_SLEEP_WAKEUP_TIMER;
   }
   else if(wakeup_cause == ESP_SLEEP_WAKEUP_EXT0)
   {
      // user_button caused the wake up
      wakeup_debug_cause = ESP_SLEEP_WAKEUP_EXT0;
   }
   else
   {
      // probably first start - esp did not exit from deep sleep ??
      // check if there was a power-up reset
      wakeup_debug_cause = 34;
      InitWakeupConfigTable();
   }
}
