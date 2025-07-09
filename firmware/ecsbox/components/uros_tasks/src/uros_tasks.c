#include "uros_tasks.h"


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