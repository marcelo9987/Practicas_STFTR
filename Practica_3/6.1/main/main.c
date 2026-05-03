/*
Código base proporcionado por el profesorado de la asignatura.
Autor original: Javier Burgoa.
Uso exclusivamente académico para la práctica 3.

Creado por Javier Burgoa el 24/11/2024
Código base para la práctica 3.
5 tareas se ejecutan de forma concurrente.
5 tareas de igual prioridad
5 tareas con la misma carga computacional

Al pulsar BOOT se muestran las estadístcias por pantalla

El CMakeLists debe incluir >> PRIV_REQUIRES driver esp_adc esp_timer

*/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/ledc.h"
#include "esp_timer.h"
//#include "driver/uart.h"
#include "esp_clk_tree.h"
#include "esp_log.h"

//PERIODOS TAREAS en ms
// CAMBIO: 400 -> 200.
#define TASK1_T 200
#define TASK2_T 200 
#define TASK3_T 200
#define TASK4_T 200
#define TASK5_T 200

//CONTADORES
// CAMBIO: 15000 -> 200 000.
#define ITERATE_1 200000
#define ITERATE_2 200000
#define ITERATE_3 200000
#define ITERATE_4 200000
#define ITERATE_5 200000

//GPIO SALIDAS Y ENTRADAS DIGITALES
#define PULSADOR 0
#define STACK_SIZE 4*1024

QueueHandle_t xQueue = NULL;

bool flag_stats = false;
void imprime_estadisticas(void);

TaskHandle_t xHandleISR = NULL;


static void IRAM_ATTR turboISR(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(xQueue, &gpio_num, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// Tarea para manejar la ISR
static void vTaskISRHandler(void* pvParameters) {
    uint32_t io_num;
    while (1)
    {
        // Esperar a recibir el número del GPIO desde la ISR
        if (xQueueReceive(xQueue, &io_num, portMAX_DELAY)) {
            if (io_num == 5) 
            {
            }
        }

        if (!flag_stats)
            printf("*************************************%llu\n", esp_timer_get_time()/1000 );

       //Consume CPU cycles
       for (long i = 0; i < ITERATE_1; i++) {
           __asm__ __volatile__("NOP");
       }

       if (!flag_stats)
            printf("*************************************%llu\n", esp_timer_get_time()/1000 );

        vTaskDelay(1 / portTICK_PERIOD_MS); 
    }
}

//---------------- TASKS ----------------
void vTaskCode1( void * pvParameters )
{
    TickType_t 		    xLastWakeTime;
    const TickType_t 	xDelayTicks = TASK1_T/portTICK_PERIOD_MS;

    xLastWakeTime = xTaskGetTickCount ();         // Initialise the xLastWakeTime variable with the current time.
    while(1)
    {
       if (!flag_stats)
            printf("\tI\t\t\t\t\t%llu\n", esp_timer_get_time()/1000 );

       //Consume CPU cycles
       for (long i = 0; i < ITERATE_1; i++) {
           __asm__ __volatile__("NOP");
       }

       if (!flag_stats)
            printf("\tO\t\t\t\t\t%llu\n", esp_timer_get_time()/1000 );

       //bloquea y espera hasta TASK1_T ms
       xTaskDelayUntil( &xLastWakeTime, xDelayTicks );
    }
}
//----------------------------------------------------------------
//-----------------Task2 -----------------------------------------
//----------------------------------------------------------------
void vTaskCode2( void * pvParameters )           //
{
    TickType_t 		    xLastWakeTime;
    const TickType_t 	xDelayTicks = TASK2_T/portTICK_PERIOD_MS;

    xLastWakeTime = xTaskGetTickCount ();         // Initialise the xLastWakeTime variable with the current time.
    while(1)
    {
       if (!flag_stats)
            printf("\t\tI\t\t\t\t%llu\n", esp_timer_get_time()/1000 );

       //Consume CPU cycles
       for (long i = 0; i < ITERATE_2; i++) {
           __asm__ __volatile__("NOP");
       }
       if (!flag_stats)
            printf("\t\tO\t\t\t\t%llu\n", esp_timer_get_time()/1000 );

       //bloquea y espera hasta TASK2_T ms
       xTaskDelayUntil( &xLastWakeTime, xDelayTicks );
    }
}
//----------------------------------------------------------------
//-----------------Task3: ----------------------------------------
//----------------------------------------------------------------
void vTaskCode3( void * pvParameters )
{
    TickType_t 		    xLastWakeTime;
    const TickType_t 	xDelayTicks = TASK3_T/portTICK_PERIOD_MS;

    xLastWakeTime = xTaskGetTickCount ();         // Initialise the xLastWakeTime variable with the current time.

    while(1)
    {
       if (!flag_stats)
            printf("\t\t\tI\t\t\t%llu\n", esp_timer_get_time()/1000 );

       //Consume CPU cycles
       for (long i = 0; i < ITERATE_3; i++) {
           __asm__ __volatile__("NOP");
       }
       if (!flag_stats)
            printf("\t\t\tO\t\t\t%llu\n", esp_timer_get_time()/1000 );

       //bloquea y espera hasta TASK3_T ms
       xTaskDelayUntil( &xLastWakeTime, xDelayTicks );
    }
}

//----------------------------------------------------------------
//-----------------Task4:   --------------------------------------
//----------------------------------------------------------------
void vTaskCode4( void * pvParameters )
{
     TickType_t 		xLastWakeTime;
    const TickType_t 	xDelayTicks = TASK4_T/portTICK_PERIOD_MS;

    xLastWakeTime = xTaskGetTickCount ();         // Initialise the xLastWakeTime variable with the current time.
    while(1)
    {
       if (!flag_stats)
            printf("\t\t\t\tI\t\t%llu\n", esp_timer_get_time()/1000 );

       //Consume CPU cycles
       for (long i = 0; i < ITERATE_4; i++) {
           __asm__ __volatile__("NOP");
       }
       if (!flag_stats)
            printf("\t\t\t\tO\t\t%llu\n", esp_timer_get_time()/1000 );

       //bloquea y espera hasta TASK4_T ms
       xTaskDelayUntil( &xLastWakeTime, xDelayTicks );
    }
}
//----------------------------------------------------------------
//-----------------Task5:   --- ----------------------------------
//----------------------------------------------------------------
void vTaskCode5( void * pvParameters )
{
     TickType_t 		xLastWakeTime;
    const TickType_t 	xDelayTicks = TASK4_T/portTICK_PERIOD_MS;
    xLastWakeTime = xTaskGetTickCount ();         // Initialise the xLastWakeTime variable with the current time.
    while(1)
    {
       if (!flag_stats)
            printf("\t\t\t\t\tI\t%llu\n", esp_timer_get_time()/1000 );

       //Consume CPU cycles
       for (long i = 0; i < ITERATE_5; i++) {
           __asm__ __volatile__("NOP");
       }

       if (!flag_stats)
            printf("\t\t\t\t\tO\t%llu\n", esp_timer_get_time()/1000 );

       //bloquea y espera hasta TASK5_T ms
       xTaskDelayUntil( &xLastWakeTime, xDelayTicks );
    }
}


//----------------------------------------------------------------
//-----------------Main loop    ----------------------------------
//----------------------------------------------------------------

#define GPIO_OUTPUT_IO_0    4     //GPIO 4  - salida 1 - 3Hz
#define GPIO_OUTPUT_IO_1    5     //GPIO 5  - salida 2 - 6Hz
#define PULSADOR            0     //GPIO 0  - entrada digital

void app_main(void)
{
    /************* Config GPIO ***************/
    gpio_set_direction(GPIO_OUTPUT_IO_0,GPIO_MODE_OUTPUT)  ;  //GPIO_OUTPUT_IO_0
    gpio_set_direction(GPIO_OUTPUT_IO_1,GPIO_MODE_OUTPUT)  ;  //GPIO_OUTPUT_IO_1

    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << PULSADOR,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en = true,
        .pull_down_en = false,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    gpio_config(&io_conf);

    //Crea Handlers a las taraeas
    TaskHandle_t xHandle1 = NULL;  //Handler a la tarea 1
    TaskHandle_t xHandle2 = NULL;  //Handler a la tarea 2
    TaskHandle_t xHandle3 = NULL;  //Handler a la tarea 3
    TaskHandle_t xHandle4 = NULL;  //Handler a la tarea 4
    TaskHandle_t xHandle5 = NULL;  //Handler a la tarea 5

    //Crea tareas
    xTaskCreate( vTaskCode1, "TASK1", STACK_SIZE, NULL, 1, &xHandle1 );  //Prioridad 1
    xTaskCreate( vTaskCode2, "TASK2", STACK_SIZE, NULL, 1, &xHandle2 );  //Prioridad 1
    xTaskCreate( vTaskCode3, "TASK3", STACK_SIZE, NULL, 1, &xHandle3 );  //Prioridad 1
    xTaskCreate( vTaskCode4, "TASK4", STACK_SIZE, NULL, 1, &xHandle4 );  //Prioridad 1
    xTaskCreate( vTaskCode5, "TASK5", STACK_SIZE, NULL, 2, &xHandle5 );  //Prioridad 1 --> CAMBIO: 1 -> 2

    int entrada_digital_t0 = 0;
    int entrada_digital_t1 = 0;

    /************* Config GPIO pulsador ***************/
        gpio_config_t myGpioConfig = {
            .pin_bit_mask = 1ULL << PULSADOR,
            .mode         = GPIO_MODE_INPUT,
            .pull_up_en   = true,             //pull-up habilitada
            .pull_down_en = false,            //pull-down deshabilitada
            .intr_type    = GPIO_INTR_NEGEDGE, //Interrupción por flanco de bajada
        };
        gpio_config(&myGpioConfig);

        xQueue = xQueueCreate(10, sizeof(uint32_t)); // Crea una cola para enviar datos desde / para la interrupción
        xTaskCreate(vTaskISRHandler, "ISR_Handler", STACK_SIZE, NULL, 10, &xHandle1); // Crea una tarea para manejar la ISR

            // Configura la interrupción para el pulsador
        gpio_install_isr_service(0); // Instala el servicio de interrupciones
        gpio_isr_handler_add(PULSADOR, turboISR, (void*) NULL); // Agrega la ISR para el pulsador
    while (1)
    {
        //main loop
        vTaskDelay(100/portTICK_PERIOD_MS);   //Wait 1 sec
    }
}



void imprime_estadisticas(void)
{
    char Buff[512]      = {0};
    char task_list[512] = {0};
    uint32_t cpu_freq_hz = 0;

    //------------- Statistics ----------------------

    //Consulta el stado de todas las tareas
    vTaskList(task_list);
    vTaskGetRunTimeStats(Buff);
    esp_clk_tree_src_get_freq_hz(SOC_CPU_CLK_SRC_PLL, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &cpu_freq_hz);

    //imprime por el terminal.
    printf("**************************************\n");
    printf("Estado de las tareas:\n%s\n", task_list);

    //Imprime la carga de la CPU.
    printf("%s ", Buff);

    //CPU Freq
    printf("La CPU se ha configurado a : %lu MHz\n", cpu_freq_hz / 1000000);

    //ticks rate
    printf("TIME SLICE: %d Hz, %.1f ms\n", configTICK_RATE_HZ, (float)(1000.0/configTICK_RATE_HZ));

    printf("**************************************\n");
}

