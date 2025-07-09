#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#include "esp32_serial_transport.h"

#include <esp_adc/adc_oneshot.h>

#include "uros_client.h"
#include "uros_tasks.h"

static size_t uart_port = UART_NUM_0;

void app_main(void)
{
	gpio_set_direction(E_BUTTON, GPIO_MODE_INPUT); // e_button
	gpio_set_direction(RELE_PIN, GPIO_MODE_OUTPUT); // relé
	gpio_set_direction(ADC_BMS, GPIO_MODE_INPUT); // bms
	gpio_set_direction(BUZZER_PIN, GPIO_MODE_OUTPUT);
	int e_button_status = gpio_get_level(E_BUTTON);

#if defined(RMW_UXRCE_TRANSPORT_CUSTOM)
	rmw_uros_set_custom_transport(
		true,
		(void *) &uart_port,
		esp32_serial_open,
		esp32_serial_close,
		esp32_serial_write,
		esp32_serial_read
	);
#else
#error micro-ROS transports misconfigured
#endif  // RMW_UXRCE_TRANSPORT_CUSTOM


	// xTaskCreate(thread_e_button, "e_button_task", 2048, NULL, 5, NULL);
	xTaskCreate(thread_battery_status, "battery_status_task", 2048, NULL, 5, NULL);
    xTaskCreate(micro_ros_task,
            "uros_task",
            CONFIG_MICRO_ROS_APP_STACK,
            NULL,
            CONFIG_MICRO_ROS_APP_TASK_PRIO,
            NULL);
}
