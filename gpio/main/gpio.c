#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#define BUTTON_PIN GPIO_NUM_2
#define LED_PIN GPIO_NUM_21


void app_main() {
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);

    int button_state = 1;  // Initialize with button released
    bool i = 0;

    while (1) {
        int new_state = gpio_get_level(BUTTON_PIN);

        if (new_state != button_state) {
            button_state = new_state;

            if (button_state == 0) {
                printf("BT PRESSIONADO\n");
                gpio_set_level(LED_PIN, i^=1);
            } else {
                printf("BOT SOLTO\n");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));  // Debounce delay
    }
}
