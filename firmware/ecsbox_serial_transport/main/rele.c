#include "rele.h"


int rele_status(int e_button_status){
  // gpio_set_level(RELE_PIN, 0);
  if (e_button_status == 0)
  {
    gpio_set_level(RELE_PIN, RELAY_ON);
  }
  if (e_button_status == 1)
  {
    gpio_set_level(RELE_PIN, RELAY_OFF);
  }  

  return e_button_status;
}
