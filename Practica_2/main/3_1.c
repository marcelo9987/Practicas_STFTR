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
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

#define STACK_SIZE  (4 * 1024) // 4 KiB
#define N           12

QueueHandle_t handleCola = NULL;

/* -------------------------------------------------- */
void tareaLectura(void *pvParametros)
{
    const adc_oneshot_unit_handle_t adc1_handle =
            (adc_oneshot_unit_handle_t) pvParametros;

    const TickType_t xDelayTicks   = pdMS_TO_TICKS(200);
    TickType_t       xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        int adc_raw = 0;
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_5, &adc_raw);

        if (!xQueueSend(handleCola, &adc_raw, 0))
        {
            fprintf(stderr, "fallo en la cola (subir)\n");
        }

        xTaskDelayUntil(&xLastWakeTime, xDelayTicks);
    }
}

/* -------------------------------------------------- */
void tareaCalculo(void *pvParametros)
{
    const TickType_t xDelayTicks   = pdMS_TO_TICKS(200);
    TickType_t       xLastWakeTime = xTaskGetTickCount();

    int indice_elemento_lectura = 0;
    int datos_leidos[10]        = {0};
    int muestras_validas        = 0;

    for (;;)
    {
        if (xQueueReceive(handleCola,
                          &datos_leidos[indice_elemento_lectura],
                          portMAX_DELAY))
        {
            if (muestras_validas < 10)
            {
                muestras_validas++;
            }

            int media = 0;
            for (int i = 0; i < muestras_validas; i++)
        {
                media += datos_leidos[i];
        }

            media /= muestras_validas;

            ESP_LOGI("CALCULO",
                     "Recibido: %i; Media: %i",
                     datos_leidos[indice_elemento_lectura],
                     media);

            indice_elemento_lectura =
                    (indice_elemento_lectura + 1) % 10;
        }

        xTaskDelayUntil(&xLastWakeTime, xDelayTicks);
    }
}

/* -------------------------------------------------- */
void app_main(void)
{
    handleCola = xQueueCreate(N, sizeof(int));

    adc_oneshot_unit_handle_t adc1_handle;

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1
    };
    adc_oneshot_new_unit(&init_config, &adc1_handle);

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT
    };
    adc_oneshot_config_channel(adc1_handle,
                               ADC_CHANNEL_5,
                               &config);

    xTaskCreate(tareaLectura,
                "LECTURA",
                STACK_SIZE,
                (void *) adc1_handle,
                1,
                NULL);

    xTaskCreate(tareaCalculo,
                "CALCULO",
                STACK_SIZE,
                NULL,
                1,
                NULL);

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
