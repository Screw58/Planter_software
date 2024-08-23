/*!
******************************************************************
* @file: adc.c
* @date: 11.08.2024
* @author: Jacek Twardoch
* @brief:
******************************************************************/

//======================================[INCLUDES]======================================//
#include "adc.h"
//==================================[EXTERN VARIABLES]==================================//

//=============================[PRIVATE MACROS AND DEFINES]=============================//

//==================================[PRIVATE TYPEDEFS]==================================//

//==================================[STATIC VARIABLES]==================================//

//==================================[GLOBAL VARIABLES]==================================//
adc_cali_handle_t adc_cal_handle;
adc_oneshot_unit_handle_t AdcSoilSens_handle;
//=============================[LOCAL FUNCTION PROTOTYPES]==============================//

//==================================[LOCAL FUNCTIONS]===================================//

//==================================[GLOBAL FUNCTIONS]==================================//
void ADC_Init(void)
{

   adc_oneshot_unit_init_cfg_t adc_cfg = {
      .unit_id = ADC_UNIT_1,
      .clk_src = ADC_DIGI_CLK_SRC_DEFAULT,
      .ulp_mode = ADC_ULP_MODE_DISABLE,
   };
   adc_oneshot_chan_cfg_t adc1_ch0_cfg = {
      .atten = ADC_ATTEN_DB_12,
      .bitwidth = ADC_BITWIDTH_12,
   };

   ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_cfg, &AdcSoilSens_handle));
   ESP_ERROR_CHECK(adc_oneshot_config_channel(AdcSoilSens_handle, ADC_SOILSENS_ADC_CH6, &adc1_ch0_cfg));

   adc_cali_line_fitting_config_t cali_config = {
      .unit_id = ADC_UNIT_1,
      .atten = ADC_ATTEN_DB_12,
      .bitwidth = ADC_BITWIDTH_12,
   };

   ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &adc_cal_handle));
}

// void ADC_Read_mV(adc_channel_t channel)
uint32_t ADC_Read_mV(adc_channel_t channel)
{
   uint32_t raw_data = 0;
   uint32_t voltage_mV = 0;
   ESP_ERROR_CHECK(adc_oneshot_read(AdcSoilSens_handle, channel, (int *)&raw_data));
   ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_cal_handle, raw_data, (int *)&voltage_mV));
   // esp_err_t adc_cali_raw_to_voltage(adc_cali_handle_t handle, int raw, int *voltage)
   return voltage_mV;
}