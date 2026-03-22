#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

#define STACK_SIZE 2048

#define LED1 GPIO_NUM_4        // Parpadeo constante
#define LED2 GPIO_NUM_5        // Cambia con el  botón
#define BOTON GPIO_NUM_0      // BOTÓN BOOT
#define ADC_CHANNEL ADC_CHANNEL_5   // GPIO6

typedef struct 
{
    adc_oneshot_unit_handle_t adc;
    SemaphoreHandle_t sem_t3;
} ParamADC;


void tarea1(void *params) 
{
    const TickType_t espera = pdMS_TO_TICKS(50); // 10Hz
    TickType_t lastWakeTime = xTaskGetTickCount();
    bool estado_led = false;

    for (;;) 
    {
        gpio_set_level(LED1_GPIO, estado_led);
        estado_led = !estado_led;
        vTaskDelayUntil(&lastWakeTime, espera);
    }
}


void tarea2(void *params) 
{
    ParamADC *p = (ParamADC *) params;
    const TickType_t espera = pdMS_TO_TICKS(20);
    TickType_t lastWakeTime = xTaskGetTickCount();
    bool led2_state = false;


    int last_level = 1;


    for (;;) 
    {
        int level = gpio_get_level(BOTON);
        if (last_level == 1 && level == 0) 
        {
            printf("¡Botón pulsado!\n");
            led2_state = !led2_state;

            xSemaphoreGive(p->sem_t3);

            vTaskDelay(pdMS_TO_TICKS(200)); // debounce
            lastWakeTime = xTaskGetTickCount();  //Actuañlizo el momento de toma de tiempo
        }
        level=0;
        gpio_set_level(LED2, led2_state);
        vTaskDelayUntil(&lastWakeTime, espera);
    }
}


void tarea3(void *params) 
{
    ParamADC *p = (ParamADC *) params;
    int valores[10];

    for (;;) 
    {

        xSemaphoreTake(p->sem_t3, portMAX_DELAY);

        int suma = 0;
        for (int i = 0; i < 10; i++) 
        {
            adc_oneshot_read(p->adc, ADC_CHANNEL, &valores[i]);
            suma += valores[i];
        }

        printf("Promedio ADC: %d\n", suma / 10);
    }
}


void app_main() 
{
    gpio_reset_pin(LED1);
    gpio_reset_pin(LED2);
    gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED2, GPIO_MODE_OUTPUT);

    gpio_reset_pin(BOTON);
    gpio_set_direction(BOTON, GPIO_MODE_INPUT);
    gpio_pullup_en(BOTON);

    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config = 
    {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_config, &adc_handle);

    adc_oneshot_chan_cfg_t chan_cfg = 
    {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &chan_cfg);

    SemaphoreHandle_t sem_t3 = xSemaphoreCreateBinary();

    ParamADC param_adc = 
    {
        .adc = adc_handle,
        .sem_t3 = sem_t3
    };

    xTaskCreate(tarea1, "Tarea 1", STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(tarea2, "Tarea 2", STACK_SIZE, (void *)&param_adc, 1, NULL);
    xTaskCreate(tarea3, "Tarea 3", STACK_SIZE, (void *)&param_adc, 1, NULL);

    for(;;) // Para que no mate la tarea
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    } 
}