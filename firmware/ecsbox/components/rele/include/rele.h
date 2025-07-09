#ifndef RELE_H
#define RELE_H

// #define RELE_PIN GPIO_NUM_14
#define RELE_PIN 19
#define RELAY_ON 1			// Define relay on pin state
#define RELAY_OFF 0	

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include <stdio.h>
int rele_status(int e_button_status);

#endif

