#include "uros_tasks.h"

std_msgs__msg__Bool msg_e_stop;
sensor_msgs__msg__BatteryState msg_batmiss;
rcl_publisher_t publisher_e_stop;
rcl_publisher_t publisher_batmiss;
rclc_support_t support;
rcl_node_t node;
rclc_executor_t executor;
rcl_timer_t timer_bms;
rcl_timer_t timer_e_stop;

enum states 
{
  WAITING_AGENT,
  AGENT_AVAILABLE,
  AGENT_CONNECTED,
  AGENT_DISCONNECTED
} state;

bool buzzer_started;
float warning_voltage;

void micro_ros_task(void * arg)
{
	while (1) {
		switch (state) 
    {
        case WAITING_AGENT:
            EXECUTE_EVERY_N_MS(500, state = (RMW_RET_OK == rmw_uros_ping_agent(100, 1)) ? AGENT_AVAILABLE : WAITING_AGENT);
            break;
        case AGENT_AVAILABLE:
            state = (true == createEntities()) ? AGENT_CONNECTED : WAITING_AGENT;
            if (state == WAITING_AGENT) 
            {
                destroyEntities();
            }
            break;
        case AGENT_CONNECTED:
            EXECUTE_EVERY_N_MS(200, state = (RMW_RET_OK == rmw_uros_ping_agent(100, 1)) ? AGENT_CONNECTED : AGENT_DISCONNECTED);
            if (state == AGENT_CONNECTED) 
            {
                rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
            }
            break;
        case AGENT_DISCONNECTED:
            destroyEntities();
            state = WAITING_AGENT;
            break;
        default:
            break;
    }
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

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

bool createEntities(void){
    rcl_allocator_t allocator = rcl_get_default_allocator();
	
    

	rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
	RCCHECK(rcl_init_options_init(&init_options, allocator));
	// create init_options
	RCCHECK(rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator));


	RCCHECK(rclc_node_init_default(&node, "ecsbox_node", "misskal", &support));

	// Create publishers
	// RCCHECK(rclc_publisher_init_default(&publisher_e_stop, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "e_stop"));
	RCCHECK(rclc_publisher_init_default(&publisher_batmiss, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, BatteryState), "battery_state_publisher"));

	// Create timers
	
	// RCCHECK(rclc_timer_init_default(&timer_e_stop, &support, RCL_MS_TO_NS(100), thread_e_button));
	RCCHECK(rclc_timer_init_default(&timer_bms, &support, RCL_MS_TO_NS(500), thread_battery_status));

	// Create executor

	RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator));
	RCCHECK(rclc_executor_add_timer(&executor, &timer_e_stop));
	RCCHECK(rclc_executor_add_timer(&executor, &timer_bms));

    return true;
	
}


bool destroyEntities(void){

    rmw_context_t * rmw_context = rcl_context_get_rmw_context(&support.context);
    (void) rmw_uros_set_context_entity_destroy_session_timeout(rmw_context, 0);
    rcl_publisher_fini(&publisher_batmiss, &node);
    rcl_publisher_fini(&publisher_e_stop, &node);
    rcl_node_fini(&node);
    rcl_timer_fini(&timer_bms);
    rcl_timer_fini(&timer_e_stop);
    rclc_executor_fini(&executor);
    rclc_support_fini(&support);

    return true;
}
