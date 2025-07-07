#include "uros_client.h"
#include "test_button.h"
#include "rele.h"
#include "buzzer.h"
#include "driver/uart.h"
#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros/rmw_microros.h>
#include "esp32_serial_transport.h"


#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting. \n",__LINE__,(int)temp_rc); ESP_LOGI(TAG3,"RCCHECK ERROR LOOP");}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting. \n",__LINE__,(int)temp_rc);}}



const static char *TAG = "LOG_E_BUTTON";
const static char *TAG2 = "LOG_BMS";
const static char *TAG3 = "ERROR_LOOP";

std_msgs__msg__Bool msg_e_stop;
sensor_msgs__msg__BatteryState msg_batmiss;
rcl_publisher_t publisher_e_stop;
rcl_publisher_t publisher_batmiss;
bool buzzer_started = false;
float warning_voltage = 35.0;

void error_loop(){
  while(1){
    
    vTaskDelay( 1000 / portTICK_PERIOD_MS);
    printf("ERROR_LOOP");
  }
}

void e_stop_callback(rcl_timer_t * timer, int64_t last_call_time){
  RCLC_UNUSED(last_call_time);

  if (timer != NULL) {
    RCSOFTCHECK(rcl_publish(&publisher_e_stop, &msg_e_stop, NULL));
    int value = gpio_get_level(E_BUTTON);
    if ( value == 0 ){
      msg_e_stop.data = true;
    }
    else 
      msg_e_stop.data = false;

    // rele_status(value);
  
  }
}

void bms_callback(rcl_timer_t * timer, int64_t last_call_time){
  RCLC_UNUSED(last_call_time);
  if (buzzer_started != false && timer != NULL){
    while (update_voltage() < warning_voltage){
      buzzer_toggle();
      msg_batmiss.voltage = update_voltage();
    }
    msg_batmiss.voltage = update_voltage();
    rcl_publish(&publisher_batmiss, &msg_batmiss, NULL);
  }
  buzzer_started = buzzer_init();
}


void micro_ros_task (void * arg){

  static size_t uart_port = UART_NUM_0;

  rcl_allocator_t allocator = rcl_get_default_allocator();
  rclc_support_t support;

  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  RCCHECK(rcl_init_options_init(&init_options, allocator));


  RCCHECK(rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator));

  rcl_node_t node;
  RCCHECK(rclc_node_init_default(&node,"ecsbox_node","misskal", &support));
  
  RCCHECK(rclc_publisher_init_default(&publisher_e_stop, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "e_stop"));

  RCCHECK(rclc_publisher_init_default(&publisher_batmiss, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, BatteryState), "battery_state_publisher"));

  rcl_timer_t timer;
  const unsigned int timer_timeout = 100;
  RCCHECK(rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(timer_timeout), e_stop_callback));
  rcl_timer_t timer_bms;
  const unsigned int timer_timeout_bms = 500;
  RCCHECK(rclc_timer_init_default(&timer_bms, &support, RCL_MS_TO_NS(timer_timeout_bms), bms_callback));


  rclc_executor_t executor;
  RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));
  RCCHECK(rclc_executor_add_timer(&executor, &timer_bms));
  


  while(1){
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
    int sw_status = switch_update();
    ESP_LOGI(TAG, "executor_spinned");
    usleep(100);
  }

  RCCHECK(rcl_publisher_fini(&publisher_e_stop, &node));
  RCCHECK(rcl_publisher_fini(&publisher_batmiss, &node));
  RCCHECK(rcl_node_fini(&node));

  vTaskDelete(NULL);

}

