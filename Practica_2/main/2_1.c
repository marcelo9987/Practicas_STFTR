#include <stdio.h>
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "portmacro.h"
#include "esp_log.h"

#define LED4 4
#define LED5 5
#define BOTON 0



#define APAGADO     0
#define ENCENDIDO   1

#define STACK_SIZE 2*1024



void tareaPin4(void *pvParametros)
{
    const TickType_t xDelayTicks = pdMS_TO_TICKS(166);

    int estadoLed = APAGADO;
    
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for(;;)
    {
        gpio_set_level(LED4, estadoLed);
        estadoLed=estadoLed==ENCENDIDO?APAGADO:ENCENDIDO;
        xTaskDelayUntil(&xLastWakeTime, xDelayTicks);
    }
}

void tareaPin5(void *pvParametros)
{
    const TickType_t xDelayTicks = pdMS_TO_TICKS(83);

    int estadoLed = APAGADO;
    
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for(;;)
    {
        gpio_set_level(LED5, estadoLed);
        estadoLed=estadoLed==ENCENDIDO?APAGADO:ENCENDIDO;
        xTaskDelayUntil(&xLastWakeTime, xDelayTicks);
    }
}

void app_main()
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    


    gpio_set_direction(LED4,GPIO_MODE_OUTPUT);
    gpio_set_direction(LED5,GPIO_MODE_OUTPUT);
    gpio_set_direction(BOTON,GPIO_MODE_INPUT);



    TaskHandle_t xHandleTarea1 = NULL;
    TaskHandle_t xHandleTarea2 = NULL;



    xTaskCreate(tareaPin4
        , "LED4"
        , STACK_SIZE
        , NULL
        , 1
        , &xHandleTarea1);

    xTaskCreate(tareaPin5
    , "LED5"
    , STACK_SIZE
    , NULL
    , 1
    , &xHandleTarea2);

    int64_t pulsacionAntigua = esp_timer_get_time();

    
    int estado = 0;
    int64_t tiempoSolicitudReanudar;

    for(;;)
    {
        const int estado_tarea = eTaskGetState(xHandleTarea2);
        ESP_LOGI("MAIN", "Estado tarea: %s", estado_tarea==0?"READY": //log INFO (Por eso la I)
            estado_tarea==1?"RUNNING":
            estado_tarea==2?"BLOCKED":
            estado_tarea==3?"SUSPENDED":"DELETED");

        int64_t ahora = esp_timer_get_time(); // MICROSEGUNDOS!!

        if(estado==2 && ahora-tiempoSolicitudReanudar >= 3000000) // 3s = 3*10³ms = 3*10⁶us
        {
            vTaskResume(xHandleTarea2);
        }


        if (gpio_get_level(BOTON) == 0 && (ahora - pulsacionAntigua) > 250000)
        {
            pulsacionAntigua = ahora;

            estado =(estado+1)%3;

           switch(estado)
           {
            case 0:
            break;
            case 1:
                vTaskSuspend(xHandleTarea2);
                break;
            case 2:
                tiempoSolicitudReanudar = esp_timer_get_time();
                break;
           }
           const TickType_t xDelayTicks = pdMS_TO_TICKS(3000);
           vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(xDelayTicks));
        }

        const TickType_t xDelayTicks = pdMS_TO_TICKS(100);     
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(xDelayTicks));
    }
    
}
