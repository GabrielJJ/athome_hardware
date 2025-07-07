#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <std_msgs/msg/int32.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros/rmw_microros.h>

#include "esp32_serial_transport.h"

#include <esp_adc/adc_oneshot.h>
#include "adcreader.h"
#include "test_button.h"
#include "uros_client.h"
#include "rele.h"
#include "buzzer.h"

static size_t uart_port = UART_NUM_0;

void app_main(void)
{
	gpio_set_direction(E_BUTTON, GPIO_MODE_INPUT); // e_button
	// gpio_input_enable(E_BUTTON);
	gpio_set_direction(RELE_PIN, GPIO_MODE_OUTPUT); // relé
	gpio_set_direction(ADC_BMS, GPIO_MODE_INPUT); // bms
	gpio_set_direction(BUZZER_PIN, GPIO_MODE_OUTPUT);
	// ebutton_init();
	buzzer_init();
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



    xTaskCreate(micro_ros_task,
            "uros_task",
            CONFIG_MICRO_ROS_APP_STACK,
            NULL,
            CONFIG_MICRO_ROS_APP_TASK_PRIO,
            NULL);
}
