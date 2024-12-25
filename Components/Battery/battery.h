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

//======================================[TYPEDEFS]======================================//

//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//
void CheckBatteryVoltage(void);
void BatteryInit(void);
#endif
