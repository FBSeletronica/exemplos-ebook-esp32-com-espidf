/***************************************************************
Exemplo: USo de GPIO com ESP-IDF (ESP32)
Descrição: Este exemplo mostra como usar GPIOs como entrada e saída.
Autor: Fábio Souza
****************************************************************/

// Inclusão de bibliotecas
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

// mapeamento de pinos
#define BUTTON_PIN GPIO_NUM_2   // GPIO2 é o pino do botão
#define LED_PIN GPIO_NUM_21     //GPIO21 é o pino do LED

// Função principal do programa
void app_main() {                                           
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);              // Configura o pino do LED como saída
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);            // Configura o pino do botão como entrada
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);           // Habilita o resistor de pull-up interno

    int button_state = 1;                                       // Estado inicial do botão            
    bool i = 0;                                                 // Variável auxiliar para controlar o status do LED

    while (1) {                                                 // Loop infinito
        int new_state = gpio_get_level(BUTTON_PIN);             // Lê o estado do botão

        if (new_state != button_state) {                        // Verifica se o estado do botão mudou
            button_state = new_state;                           // se mudou, atualiza o estado do botão

            if (button_state == 0) {                            // Verifica se o botão foi pressionado
                printf("BT PRESSIONADO\n");                     // Mensagem de botão pressionado
                gpio_set_level(LED_PIN, i^=1);                  // Inverte o estado do LED
            } else {                                            // Se o botão foi solto
                printf("BOT SOLTO\n");                          // Mensagem de botão solto
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));                          // Aguarda 10ms
    }
}
