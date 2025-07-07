#ifndef UROS_CLIENT_H
#define UROS_CLIENT_H

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_system.h"

#include "adcreader.h"

#include <uros_network_interfaces.h>
#include <std_msgs/msg/header.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>
#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/bool.h>
#include <sensor_msgs/msg/battery_state.h>
#include "esp_log.h"
void error_loop();

void micro_ros_task (void * arg);

void e_stop_callback (rcl_timer_t * timer, int64_t last_call_time);

void bms_callback (rcl_timer_t * timer, int64_t last_call_time);



#endif // !UROS_CLIENT_H

