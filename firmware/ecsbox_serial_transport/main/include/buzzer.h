#ifndef BUZZER_H
#define BUZZER_H

#define BUZZER_PIN  GPIO_NUM_23

#include <stdbool.h>

void buzzer_toggle(void);

bool buzzer_init (void);

#endif
