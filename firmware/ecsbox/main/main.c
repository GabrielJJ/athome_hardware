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
#include <sensor_msgs/msg/battery_state.h>

#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros/rmw_microros.h>
#include <std_msgs/msg/bool.h>
#include "esp32_serial_transport.h"

#include <esp_adc/adc_oneshot.h>
#include "adcreader.h"
#include "test_button.h"
#include "rele.h"
#include "buzzer.h"

static size_t uart_port = UART_NUM_0;
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc);vTaskDelete(NULL);}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

std_msgs__msg__Bool msg_e_stop;
sensor_msgs__msg__BatteryState msg_batmiss;
rcl_publisher_t publisher_e_stop;
rcl_publisher_t publisher_batmiss;
bool buzzer_started = false;
float warning_voltage = 35.0;

void thread_e_button(void * arg)
{
	while(1){
		// int e_button_status = gpio_get_level(E_BUTTON);
		// if (e_button_status == 0){
		// 	rele_status(true);

		// }
		// else {
		// 	rele_status(false);

		// }

		// msg_e_stop.data = e_button_status == 0 ? true : false;
		// RCSOFTCHECK(rcl_publish(&publisher_e_stop, &msg_e_stop, NULL));
		vTaskDelay( 100 / portTICK_PERIOD_MS);
	}
}

void thread_battery_status(void * arg)
{
	while(1){
		float voltage = update_voltage();
		if (voltage < warning_voltage){
			buzzer_toggle();
		}
		msg_batmiss.voltage = voltage;
		RCSOFTCHECK(rcl_publish(&publisher_batmiss, &msg_batmiss, NULL));
		vTaskDelay( 1000 / portTICK_PERIOD_MS);
	}
}

void micro_ros_task(void * arg)
{
	rcl_allocator_t allocator = rcl_get_default_allocator();
	rclc_support_t support;

	rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
	RCCHECK(rcl_init_options_init(&init_options, allocator));
	// create init_options
	RCCHECK(rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator));

	rcl_node_t node;
	RCCHECK(rclc_node_init_default(&node, "ecsbox_node", "misskal", &support));

	// Create publishers
	// RCCHECK(rclc_publisher_init_default(&publisher_e_stop, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "e_stop"));
	RCCHECK(rclc_publisher_init_default(&publisher_batmiss, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, BatteryState), "battery_state_publisher"));

	// Create timers
	rcl_timer_t timer_e_stop;
	rcl_timer_t timer_bms;
	// RCCHECK(rclc_timer_init_default(&timer_e_stop, &support, RCL_MS_TO_NS(100), thread_e_button));
	RCCHECK(rclc_timer_init_default(&timer_bms, &support, RCL_MS_TO_NS(500), thread_battery_status));

	// Create executor
	rclc_executor_t executor;
	RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator));
	RCCHECK(rclc_executor_add_timer(&executor, &timer_e_stop));
	RCCHECK(rclc_executor_add_timer(&executor, &timer_bms));

	// Start threads
	// xTaskCreate(thread_e_button, "e_button_task", 2048, NULL, 5, NULL);
	xTaskCreate(thread_battery_status, "battery_status_task", 2048, NULL, 5, NULL);

	while (1) {
		RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

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
