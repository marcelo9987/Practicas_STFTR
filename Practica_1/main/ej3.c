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