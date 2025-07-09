#include "uros_client.h"
#include "uros_tasks.h"

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
