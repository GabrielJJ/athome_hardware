#ifndef UROS_CLIENT_H
#define UROS_CLIENT_H

#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <std_msgs/msg/int32.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <sensor_msgs/msg/battery_state.h>

#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros/rmw_microros.h>
#include <std_msgs/msg/bool.h>

enum states 
{
  WAITING_AGENT,
  AGENT_AVAILABLE,
  AGENT_CONNECTED,
  AGENT_DISCONNECTED
} state;

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc);vTaskDelete(NULL);}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

std_msgs__msg__Bool msg_e_stop;
sensor_msgs__msg__BatteryState msg_batmiss;
rcl_publisher_t publisher_e_stop;
rcl_publisher_t publisher_batmiss;
rclc_support_t support;
rcl_node_t node;
rclc_executor_t executor;
rcl_timer_t timer_bms;
rcl_timer_t timer_e_stop;


bool createEntities(void);
bool destroyEntities(void);

#endif