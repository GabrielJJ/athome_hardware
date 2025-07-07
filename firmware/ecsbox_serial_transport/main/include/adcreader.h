#ifndef ADCREADER_H
#define ADCREADER_H

#define ADC_BMS ADC_CHANNEL_6

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"



#include "esp_adc/adc_oneshot.h"



adc_oneshot_unit_handle_t adc_setup_init();
float update_voltage();


#endif 
