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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define PIN_BTN GPIO_NUM_0
#define PINLED1 GPIO_NUM_4
#define PINLED2 GPIO_NUM_5


const TickType_t myDelay = 1000 / 2 / portTICK_PERIOD_MS; 

int app_main()
{
    gpio_set_direction(PIN_BTN, GPIO_PULLDOWN_ENABLE | GPIO_MODE_INPUT);

    gpio_set_direction(PINLED1, GPIO_MODE_OUTPUT);

    gpio_set_direction(PINLED2, GPIO_MODE_OUTPUT);

    int estadoPinParpadea = 0;

    

    for(;;)
    {
        estadoPinParpadea = !estadoPinParpadea; 
        gpio_set_level(PINLED2,estadoPinParpadea);

        int valor_boton = gpio_get_level(PIN_BTN);
        if(valor_boton==0)
        {
            gpio_set_level(PINLED1,1);
        }
        else
        {
         gpio_set_level(PINLED1,0);
        }

        vTaskDelay(myDelay);



    }

}