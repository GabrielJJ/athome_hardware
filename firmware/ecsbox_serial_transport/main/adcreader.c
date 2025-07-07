#include "adcreader.h"

#define ADC_BMS ADC_CHANNEL_6

#define ADC_BITWIDTH_DEFAULT ADC_BITWIDTH_12

#define ADC_UNIT_1 ADC_UNIT_1

#define ADC_ATTEN ADC_ATTEN_DB_12
const static char *TAG = "TESTE_BMS";

int raw_value;
int adc_value;
// Configure the unit that we gonna use tihe ADC
adc_oneshot_unit_handle_t handle_reader;

bool init = false;
adc_oneshot_unit_handle_t adc_setup_init(){
  adc_oneshot_unit_handle_t adc_handle;
  adc_oneshot_unit_init_cfg_t init_config = {
    .unit_id = ADC_UNIT_1,
    .ulp_mode = ADC_ULP_MODE_DISABLE,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));


  adc_oneshot_chan_cfg_t config = {
    .bitwidth = ADC_BITWIDTH_DEFAULT,
    .atten = ADC_ATTEN_DB_12,
  };

  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_6, &config));

  return adc_handle;
}

float update_voltage(){

  if (!init){
    handle_reader = adc_setup_init();
    init = true;
  }
  adc_oneshot_read(handle_reader, ADC_BMS, &adc_value);
  
  if (adc_value > 1400){
    adc_value = adc_value - 300;

  }
  else if (adc_value > 1900)
  {
    adc_value = adc_value - 400;
  }

  // ESP_LOGI(TAG, ADC_CHANNEL_6 "%d", adc_value); 
  float voltage = (float)adc_value / 1000.0 ;
  
  float BatteryVoltage = voltage * (74.00 + 3.9) / (3.9);

  float BatteryADC = BatteryVoltage * (0.986353);

  return BatteryADC;

  // float voltage = (adc_value / 4095.0 ) * ( 2.2 );

  // float batteryVoltage = voltage * (74.00 + 3.9) / (3.9);
  
  // return batteryVoltage;
}

