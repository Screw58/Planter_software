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


#define ADC_MIN_VOLT                  0
#define ADC_MAX_VOLT                  3100 /*unit mV */

#define NUMBER_OF_MEASUREMENT_SAMPLES 15
//======================================[TYPEDEFS]======================================//

//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//

/*!
 * \brief: Read given channel of ADC and return value in mV unit
 * \details:
 */
esp_err_t ADC_Read_mV(adc_channel_t channel, int *voltage_mV);

/*!
 * \brief:
 * \details:
 */
void ADC_Init(void);

/*!
 * \brief:
 * \details:
 */
esp_err_t ADC_ConfigChannel(adc_channel_t channel);
#endif