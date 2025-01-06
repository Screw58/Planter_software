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
adc_oneshot_unit_handle_t AdcUnit1_handle;
adc_oneshot_chan_cfg_t adc1_ch0_cfg;
//=============================[LOCAL FUNCTION PROTOTYPES]==============================//

//==================================[LOCAL FUNCTIONS]===================================//

//==================================[GLOBAL FUNCTIONS]==================================//
void ADC_Init(void)
{
   esp_err_t EspErr = ESP_OK;
   adc_oneshot_unit_init_cfg_t adc_cfg = {
      .unit_id = ADC_UNIT_1,
      // .clk_src = ADC_DIGI_CLK_SRC_DEFAULT,
      .ulp_mode = ADC_ULP_MODE_DISABLE,
   };


   adc1_ch0_cfg.atten = ADC_ATTEN_DB_12;
   adc1_ch0_cfg.bitwidth = ADC_BITWIDTH_12;


   EspErr |= adc_oneshot_new_unit(&adc_cfg, &AdcUnit1_handle);
   // ESP_ERROR_CHECK(adc_oneshot_config_channel(AdcUnit1_handle, ADC_SOILSENS_ADC_CH6, &adc1_ch0_cfg));
   // ESP_ERROR_CHECK(adc_oneshot_config_channel(AdcUnit1_handle, ADC_BATTERY_ADC_CH7, &adc1_ch0_cfg));

   adc_cali_line_fitting_config_t cali_config = {
      .unit_id = ADC_UNIT_1,
      .atten = ADC_ATTEN_DB_12,
      .bitwidth = ADC_BITWIDTH_12,
   };

   EspErr |= adc_cali_create_scheme_line_fitting(&cali_config, &adc_cal_handle);
}

esp_err_t ADC_ConfigChannel(adc_channel_t channel)
{
   esp_err_t EspErr = ESP_OK;
   EspErr |= adc_oneshot_config_channel(AdcUnit1_handle, channel, &adc1_ch0_cfg);
   return EspErr;
}

esp_err_t ADC_Read_mV(adc_channel_t channel, int *voltage_mV)
{
   volatile esp_err_t EspErr = ESP_OK;
   int raw_data = 0;
   int voltage_mV_temp = 0;
   int all_samples_mV = 0;
   uint8_t msrmnt_num = 0;

   *voltage_mV = 0;

   while(msrmnt_num < NUMBER_OF_MEASUREMENT_SAMPLES)
   {
      EspErr |= adc_oneshot_read(AdcUnit1_handle, channel, &raw_data);
      EspErr |= adc_cali_raw_to_voltage(adc_cal_handle, raw_data, &voltage_mV_temp);
      all_samples_mV += voltage_mV_temp;
      msrmnt_num++;
   }

   *voltage_mV = all_samples_mV / NUMBER_OF_MEASUREMENT_SAMPLES;
   // esp_err_t adc_cali_raw_to_voltage(adc_cali_handle_t handle, int raw, int *voltage)
   return EspErr;
}