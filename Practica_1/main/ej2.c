/*
* 2Hz
*/
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "freertos/FreeRTOS.h"
#include "esp_system.h"



#define PINLED 4

volatile int flag = 0;

bool IRAM_ATTR timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *event, void *user_data)
{
    flag = 1;
    return true;
}


void app_main()
{

    gpio_set_direction(PINLED, GPIO_MODE_OUTPUT);

    gptimer_handle_t timer = NULL;

    gptimer_config_t configuracion_timer = 
    {
        .clk_src=GPTIMER_CLK_SRC_DEFAULT,
        .direction=GPTIMER_COUNT_UP,
        .resolution_hz=1000*1000,

    };

 
    
    gptimer_new_timer(&configuracion_timer, &timer);

       gptimer_alarm_config_t configuracion_alarma = 
    {
        .reload_count = 0,
        .alarm_count=2500000,
        .flags.auto_reload_on_alarm = true,
    };

    gptimer_set_alarm_action(timer,&configuracion_alarma);

    gptimer_event_callbacks_t alarmaCallback = 
    {
        .on_alarm=timer_callback,
    };
    gptimer_register_event_callbacks(timer,&alarmaCallback, NULL);

    gptimer_enable(timer);
    gptimer_start(timer);

    bool estadoLed = 1;


    while(1)
    {
        if(flag == 1)
        {
            flag = 0;
            estadoLed = !estadoLed;
            gpio_set_level(PINLED,estadoLed);
        }

    }
}
