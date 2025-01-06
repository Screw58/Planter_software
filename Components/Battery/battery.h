/*!
*******************************************************
* \file:battery.h
* \date:25.08.2024
* \author: Jacek Twardoch
* \brief:
********************************************************/

#ifndef BATTERY_H_
#define BATTERY_H_

//======================================[INCLUDES]======================================//
#include "adc.h"
//=================================[MACROS AND DEFINES]=================================//
#define BATTERY_ADC_CHANNEL ADC_CHANNEL_7 /*IO 35*/

/* Pin which turns on the transoptor output for voltage divider (2 resistors connected to battery) */
#define BATTERY_VOLT_MEAS_ENABLE_PIN GPIO_NUM_32
//======================================[TYPEDEFS]======================================//

//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//
void CheckBatteryVoltage(void);
void BatteryInit(void);
#endif
