#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

void app_main(void)
{
    // Configuração do pino GPIO para o LED (número do pino pode variar)
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    
    printf("Hello World! \n");

    while (1)
    {
        // Liga o LED
        gpio_set_level(GPIO_NUM_2, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Aguarda 1 segundo
        printf("LED - ON \n");

        // Desliga o LED
        gpio_set_level(GPIO_NUM_2, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Aguarda 1 segundo
        printf("LED - OFF \n");
    }
}
