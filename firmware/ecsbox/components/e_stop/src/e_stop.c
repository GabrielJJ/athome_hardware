#include "e_stop.h"
#include "driver/gpio.h"

#define GPIO_OUTPUT_PIN_SEL ((1ULL<<E_BUTTON))

bool ebutton_init(void){
    gpio_config_t io_conf = {
      .intr_type = GPIO_INTR_DISABLE,
      .mode = GPIO_MODE_OUTPUT,
      .pin_bit_mask = GPIO_OUTPUT_PIN_SEL,
      .pull_down_en = 0,
      .pull_up_en = 0
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    return true;
  }

  // Check emergency button status
int switch_update(){
  int e_button_status = gpio_get_level(E_BUTTON);
  return e_button_status;
}

