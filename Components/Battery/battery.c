/*!
******************************************************************
* \file:battery.c
* \date: 25.08.2024
* \author:
* \brief:
******************************************************************/

//======================================[INCLUDES]======================================//
#include "battery.h"
#include "esp_log.h"
#include "gpio_user.h"
//==================================[EXTERN VARIABLES]==================================//

//=============================[PRIVATE MACROS AND DEFINES]=============================//

//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//

//==================================[GLOBAL VARIABLES]==================================//

//=============================[LOCAL FUNCTION PROTOTYPES]==============================//

//==================================[LOCAL FUNCTIONS]===================================//

//==================================[GLOBAL FUNCTIONS]==================================//
void BatteryInit(void)
{
   ADC_ConfigChannel(BATTERY_ADC_CHANNEL);

   GPIO_BatteryPinInit(BATTERY_VOLT_MEAS_ENABLE_PIN);
}

void CheckBatteryVoltage(void)
{
   esp_err_t EspErr = ESP_OK;
   int battery_voltage_mV = 0;

   GPIO_SetPinHigh(BATTERY_VOLT_MEAS_ENABLE_PIN);
   EspErr = ADC_Read_mV(BATTERY_ADC_CHANNEL, &battery_voltage_mV);
   GPIO_SetPinLow(BATTERY_VOLT_MEAS_ENABLE_PIN);
   if((battery_voltage_mV > ADC_MIN_VOLT) && (battery_voltage_mV < ADC_MAX_VOLT))
   {
      battery_voltage_mV = battery_voltage_mV * 2;
      // !<TODO
      //   if(battery_voltage_mV <= BATTERY_MIN_THREESHOLD)
      //   {
      //     /* set low battery warning!*/
      //   }
   }
   else
   {
      /* Err_handle */
   }
   ESP_LOGI("battery", " BATTERY VOLTAGE IS: %d", battery_voltage_mV);
}