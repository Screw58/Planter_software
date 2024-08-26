/*!
*******************************************************
* \file:adc.c
* \date: 11.08.2024
* \author: Jacek Twardoch
* \brief:
********************************************************/
#ifndef ADC_H_
#define ADC_H_

//======================================[INCLUDES]======================================//
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"

#include "esp_err.h"

//=================================[MACROS AND DEFINES]=================================//
#define ADC_SOILSENS_ADC_CH6 ADC_CHANNEL_6 /*IO 34*/
#define ADC_BATTERY_ADC_CH7  ADC_CHANNEL_7 /*IO 35*/
#define ADC_MIN_VOLT         0
#define ADC_MAX_VOLT         3100          /*unit mV */
//======================================[TYPEDEFS]======================================//

//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//

/*!
 * \brief: Read given channel of ADC and return value in mV unit
 * \details:
 */
uint32_t ADC_Read_mV(adc_channel_t channel);

/*!
 * \brief:
 * \details:
 */
void ADC_Init(void);

#endif