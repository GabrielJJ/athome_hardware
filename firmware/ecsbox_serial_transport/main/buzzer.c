#include "buzzer.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"

const static char *TAGB = "LOG_BUZZER";

#define LEDC_TIMER         LEDC_TIMER_0
#define LEDC_MODE          LEDC_HIGH_SPEED_MODE
#define BUZZER_PIN          GPIO_NUM_23 // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (400) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY          (1000) //


bool buzzer_init(void){

    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num = LEDC_TIMER,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
    };  
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .channel    = LEDC_CHANNEL,
        .duty       = 0,
        .gpio_num   = BUZZER_PIN,
        .speed_mode = LEDC_MODE,
        .timer_sel  = LEDC_TIMER
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    return true;
}   

    void buzzer_toggle (void ){     
        if (true == buzzer_init()){
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(200 / portTICK_PERIOD_MS);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }else 
        ESP_LOGI(TAGB, "Buzzer init failed");
    }
