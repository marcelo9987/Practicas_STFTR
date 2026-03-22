#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_adc/adc_oneshot.h"
#include "portmacro.h"


#define STACK_SIZE 2048

typedef struct  
{
    SemaphoreHandle_t mutex;
    adc_oneshot_unit_handle_t adc;
} Parametros;

void tarea1(void* params)
{

    Parametros *parametros = (Parametros *) params;
    


    const   TickType_t xDelayTicks = pdMS_TO_TICKS(200);
            TickType_t xLastWakeTime = xTaskGetTickCount();

    int lectura_minima=4096;
    for(;;)
    {
        if(!xSemaphoreTake(parametros->mutex, portMAX_DELAY))
        {
            continue;
        }

        printf("Tarea 1 coge mutex: %lu\n",(unsigned long)xTaskGetTickCount()*portTICK_PERIOD_MS);
        int *array_lecturas = (int*) malloc(1000 * sizeof(int));
        if (!array_lecturas)
        {
            fprintf(stderr, "Tarea1: malloc fallo\n");
            xSemaphoreGive(parametros->mutex);
            vTaskDelay(xDelayTicks);
            continue;
        }

        for (int i=0; i < 1000;++i)
        {
            adc_oneshot_read(parametros->adc,ADC_CHANNEL_5, array_lecturas+i);
        }

        if(xSemaphoreGive(parametros->mutex)==pdFALSE)
        {
            fprintf(stderr,"Tarea1: error al soltar el mutex: %lu\n",(unsigned long)xTaskGetTickCount()*portTICK_PERIOD_MS);
        }

        int lectura_mas_baja=array_lecturas[0];
        for(int i= 0; i<1000;++i)
        {
            if(lectura_mas_baja>= *(array_lecturas+i))
            {
                lectura_mas_baja= array_lecturas[i];
            }
        }

        lectura_minima=lectura_mas_baja<lectura_minima?lectura_mas_baja:lectura_minima;

        free(array_lecturas);
        printf("Tarea 1 lectura mínima: %d | tiempo: %lu\n",lectura_minima, (unsigned long)xTaskGetTickCount()*portTICK_PERIOD_MS);

        xTaskDelayUntil(&xLastWakeTime, xDelayTicks);
    }

}

void tarea2(void* params)
{

    Parametros *parametros = (Parametros *) params;


    const   TickType_t xDelayTicks = pdMS_TO_TICKS(200);
            TickType_t xLastWakeTime = xTaskGetTickCount();

    int lectura_maxima=0;
    for(;;)
    {
        if(!xSemaphoreTake(parametros->mutex, portMAX_DELAY))
        {
            continue;
        }

        printf("Tarea 2 coge mutex: %lu\n",(unsigned long)xTaskGetTickCount()*portTICK_PERIOD_MS);
        int *array_lecturas = (int*) malloc(1000 * sizeof(int));
        if (!array_lecturas)
        {
            fprintf(stderr, "Tarea2: malloc fallo\n");
            xSemaphoreGive(parametros->mutex);
            vTaskDelay(xDelayTicks);
            continue;
        }

        for (int i=0; i < 1000;++i)
        {
            adc_oneshot_read(parametros->adc,ADC_CHANNEL_5, array_lecturas+i);
        }

        if(xSemaphoreGive(parametros->mutex)==pdFALSE)
        {
            fprintf(stderr,"Tarea 2: error al soltar el mutex: %lu\n",(unsigned long)xTaskGetTickCount()*portTICK_PERIOD_MS);
        }

        int lectura_mas_alta=array_lecturas[0];
        for(int i= 0; i<1000;++i)
        {
            if(lectura_mas_alta < *(array_lecturas+i))
            {
                lectura_mas_alta= array_lecturas[i];
            }
        }

        lectura_maxima=lectura_mas_alta>lectura_maxima?lectura_mas_alta:lectura_maxima;

        free(array_lecturas);
        printf("Tarea 2 lectura máxima: %d | tiempo: %lu\n",lectura_maxima, (unsigned long)xTaskGetTickCount()*portTICK_PERIOD_MS);

        xTaskDelayUntil(&xLastWakeTime, xDelayTicks);
    }

}

void tarea3(void* params)
{

    Parametros *parametros = (Parametros *) params;


    const   TickType_t xDelayTicks = pdMS_TO_TICKS(200);
            TickType_t xLastWakeTime = xTaskGetTickCount();

    int lectura_promedio=0;
    for(;;)
    {
        if(!xSemaphoreTake(parametros->mutex, portMAX_DELAY))
        {
            continue;
        }

        printf("Tarea 3 coge mutex: %lu\n",(unsigned long)xTaskGetTickCount()*portTICK_PERIOD_MS);
        int *array_lecturas = (int*) malloc(1000 * sizeof(int));
        if (!array_lecturas)
        {
            fprintf(stderr, "Tarea3: malloc fallo\n");
            xSemaphoreGive(parametros->mutex);
            vTaskDelay(xDelayTicks);
            continue;
        }

        for (int i=0; i < 1000;++i)
        {
            adc_oneshot_read(parametros->adc,ADC_CHANNEL_5, array_lecturas+i);
        }

        if(xSemaphoreGive(parametros->mutex)==pdFALSE)
        {
            fprintf(stderr,"Tarea 3: error al soltar el mutex: %lu\n",(unsigned long)xTaskGetTickCount()*portTICK_PERIOD_MS);
        }

        int lectura_total = 0;
        for(int i= 0; i<1000;++i)
        {
                lectura_total += array_lecturas[i];
        }

        lectura_promedio=lectura_total/1000;

        free(array_lecturas);
        printf("Tarea 3 lectura promedio: %d | tiempo: %lu\n",lectura_promedio, (unsigned long)xTaskGetTickCount()*portTICK_PERIOD_MS);

        xTaskDelayUntil(&xLastWakeTime, xDelayTicks);
    }

}

void app_main(void)
{
    Parametros parametros;
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

    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_5, &config);

    TaskHandle_t xHandleT1 ,xHandleT2 ,xHandleT3 = NULL;

    parametros.mutex=xSemaphoreCreateMutex();
    if(parametros.mutex==NULL)
    {
        fprintf(stderr,"Error creando el mutex\n");
    }

    parametros.adc = adc1_handle;


    xTaskCreate(tarea1,"Tarea-1", STACK_SIZE ,(void * )&parametros,1, &xHandleT1);
    xTaskCreate(tarea2,"Tarea-2", STACK_SIZE ,(void * )&parametros,1, &xHandleT2);
    xTaskCreate(tarea3,"Tarea-3", STACK_SIZE ,(void * )&parametros,1, &xHandleT3);


    for(;;)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
}