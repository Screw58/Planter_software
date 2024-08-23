/*!
*******************************************************
* \file:
* \date:
* \author:
* \brief:
********************************************************/
#ifndef ADC_H_
#define ADC_H_

//======================================[INCLUDES]======================================//
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
// #include "esp_adc/interface/adc_cali_interface.h"
#include "hal/adc_types.h"


#include "esp_err.h"

//=================================[MACROS AND DEFINES]=================================//
#define ADC_SOILSENS_ADC_CH6 ADC_CHANNEL_6
// #define SoilSens_ReadRawData((x), (y), (z)) (adc_oneshot_read(x, y, z))
//======================================[TYPEDEFS]======================================//
// typedef enum
// {
//    SOILSENS = 0,
//    BATTERY,
//    ADC_USR_MAX = BATTERY,
// } adc_user_type_t;

// typedef struct {

// } adc_user_t;

//=================================[EXPORTED VARIABLES]=================================//

//=============================[GLOBAL FUNCTION PROTOTYPES]=============================//
uint32_t ADC_Read_mV(adc_channel_t channel);

void ADC_Init(void);

#endif