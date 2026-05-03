/*
*   Copyright (C) <2026>  Marcelo Fort Muñoz y Victor Arroyo Márquez
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
