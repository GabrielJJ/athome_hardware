#ifndef E_STOP_H
#define E_STOP_H
#define E_BUTTON 33


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include <stdio.h>

bool ebutton_init();
int switch_update();
void test_button (void*);

#endif

