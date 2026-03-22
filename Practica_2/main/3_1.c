#include <stdio.h>
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "portmacro.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "hal/adc_types.h"




#define APAGADO     0
#define ENCENDIDO   1

#define STACK_SIZE 4*1024

#define N 12

QueueHandle_t handleCola = NULL;


void tareaLectura(void *pvParametros)
{

    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config =
    {
        .unit_id = ADC_UNIT_1,
    };

    adc_oneshot_new_unit(&init_config, &adc1_handle);

    adc_oneshot_chan_cfg_t config = 
    {
        .atten=ADC_ATTEN_DB_12,
        .bitwidth=ADC_BITWIDTH_DEFAULT,
    };

    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config);

    const   TickType_t xDelayTicks = pdMS_TO_TICKS(200);
            TickType_t xLastWakeTime = xTaskGetTickCount();

    for(;;)
    {
        int adc_raw = -1;
        adc_oneshot_read(adc1_handle,ADC_CHANNEL_6, &adc_raw);
        if(!xQueueSend(handleCola, &adc_raw, pdMS_TO_TICKS(10)))
        {
            fprintf(stderr,"fallo en la cola (subir)\n");
        }
        xTaskDelayUntil(&xLastWakeTime, xDelayTicks);
    }
}

void tareaCalculo(void *pvParametros)
{
    const TickType_t xDelayTicks = pdMS_TO_TICKS(150);
     TickType_t xLastWakeTime = xTaskGetTickCount();

    int indice_elemento_lectura = 0; 
    int datos_leidos[10] = {0};


    for(;;)
    {

        if(xQueueReceive(handleCola, (datos_leidos+indice_elemento_lectura), pdMS_TO_TICKS(10)))
        {
            int media = 0;
            for (int indice_datos = 0; indice_datos<10; ++indice_datos)
            {
                media += datos_leidos[indice_datos];
            }
            media/=10;
            ESP_LOGI("CALCULO", "Recibido: %i; Media: %i",*(datos_leidos+indice_elemento_lectura),media); 
            indice_elemento_lectura = (indice_elemento_lectura+1)%10;
        }

        

        xTaskDelayUntil(&xLastWakeTime, xDelayTicks);


    }

}

void app_main()
{


    handleCola = xQueueCreate(N,sizeof(int));



    TaskHandle_t xHandleTareaLectura = NULL;
    TaskHandle_t xHandleTareaCalculo = NULL;



    xTaskCreate(tareaLectura
        , "LECTURA"
        , STACK_SIZE
        , NULL
        , 1
        , &xHandleTareaLectura);

    xTaskCreate(tareaCalculo
    , "CALCULO"
    , STACK_SIZE
    , NULL
    , 1
    , &xHandleTareaCalculo);

    for(;;)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
}
