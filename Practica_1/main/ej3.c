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
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

#define PIN_BTN 0

void app_main()
{
    gpio_set_direction(PIN_BTN, GPIO_MODE_INPUT);
    gpio_pullup_en(PIN_BTN);

    int64_t pulsacionAntigua = esp_timer_get_time();

    for(;;)
    {
        int64_t ahora = esp_timer_get_time();
        if (gpio_get_level(PIN_BTN) == 0 && (ahora - pulsacionAntigua) > 250000)
        {
            printf("diferencia: %lld\n", ahora - pulsacionAntigua);
            pulsacionAntigua = ahora;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}