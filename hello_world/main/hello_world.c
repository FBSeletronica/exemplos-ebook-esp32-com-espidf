/***************************************************************
Exemplo: Hello World com ESP-IDF (ESP32)
Descrição: Este exemplo pisca o LED da placa a cada 1 segundo e imprime
uma mensagem na serial com o status do LED.
Autor: Fábio Souza
****************************************************************/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// Função principal do programa
void app_main(void)
{
    // Configuração do pino GPIO para o LED (número do pino pode variar)
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    
    printf("Hello World! \n");  // Mensagem de inicialização

    while (1)                                  // Loop infinito
    {
        gpio_set_level(GPIO_NUM_2, 1);         // Liga o LED
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Aguarda 1 segundo
        printf("LED - ON \n");                 // Mensagem de LED ligado

        gpio_set_level(GPIO_NUM_2, 0);         // Desliga o LED
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Aguarda 1 segundo
        printf("LED - OFF \n");                // Mensagem de LED desligado
    }
}
