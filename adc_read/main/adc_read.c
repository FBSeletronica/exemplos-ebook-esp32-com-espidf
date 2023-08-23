/***************************************************************
Exemplo: Leitura de ADC com ESP-IDF (ESP32)
Descrição: Este exemplo mostra como configurar e ler valores de um canal ADC
Autor: Fábio Souza
****************************************************************/
// Inclusão de bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Inclusão de bibliotecas do FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Inclusão de bibliotecas do ESP-IDF
#include "soc/soc_caps.h"               
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

const static char *TAG = "EXAMPLE";     // Marca para mensagens de log

// Declaração de variáveis
static int adc_raw[2][10];  
static int voltage[2][10];  
static bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);
static void example_adc_calibration_deinit(adc_cali_handle_t handle);

// Função principal
void app_main(void){                                
    //-------------ADC1 Init---------------//
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle)); 

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_0, &config));

    //-------------ADC1 Calibration Init---------------//
    adc_cali_handle_t adc1_cali_chan0_handle = NULL;
    bool do_calibration1_chan0 = example_adc_calibration_init(ADC_UNIT_1, ADC_CHANNEL_0, ADC_ATTEN_DB_11, &adc1_cali_chan0_handle);

    while (1)   // Loop infinito
    {
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &adc_raw[0][0]));                               // Lê o valor do ADC1
        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, ADC_CHANNEL_0, adc_raw[0][0]);               // Imprime o valor bruto lido
        if (do_calibration1_chan0)                                                                                   // Se o ADC foi calibrado na inicialização
        {
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_chan0_handle, adc_raw[0][0], &voltage[0][0]));        // Converte o valor bruto para tensão
            ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, ADC_CHANNEL_0, voltage[0][0]);   // Imprime o valor da tensão
        }
        vTaskDelay(pdMS_TO_TICKS(1000));                                                                            // Aguarda 1 segundo
    }

    //Se chegar nesse ponto, de-inicializa o ADC1
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
    if (do_calibration1_chan0)                                  // Se o ADC foi calibrado na inicialização
    {
        example_adc_calibration_deinit(adc1_cali_chan0_handle); // De-inicializa a calibração
    }
}
/*---------------------------------------------------------------
Função de CalibraçãO do ADC
Argumentos:
    unit: Unidade do ADC
    channel: Canal do ADC
    atten: Atenuação do ADC
    out_handle: Ponteiro para o handle da calibração
retorno:
    true: Calibração realizada com sucesso
    false: Calibração não realizada
---------------------------------------------------------------*/
static bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;        // Handle da calibração
    esp_err_t ret = ESP_FAIL;               // Variável de retorno da função
    bool calibrated = false;                // Flag de calibração

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED  // Se a calibração por curva estiver habilitada
    if (!calibrated)                         // Se não estiver calibrado
    {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting"); // Imprime a versão da calibração
        adc_cali_curve_fitting_config_t cali_config = {                     // Configuração da calibração
            .unit_id = unit,                                                // Unidade do ADC
            .chan = channel,                                                // Canal do ADC
            .atten = atten,                                                 // Atenuação do ADC
            .bitwidth = ADC_BITWIDTH_DEFAULT,                               // Largura de bits do ADC
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);  // Cria a calibração
        if (ret == ESP_OK)                                                  // Se a calibração foi criada com sucesso
        {
            calibrated = true;                                              // Flag de calibração igual a true       
        }
    }
#endif  // Fim da calibração por curva

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED                                  // Se a calibração por linha estiver habilitada
    if (!calibrated)                                                        // Se não estiver calibrado
    {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");  // Imprime a versão da calibração
        adc_cali_line_fitting_config_t cali_config = {                      // Configuração da calibração
            .unit_id = unit,                                                // Unidade do ADC
            .atten = atten,                                                 // Atenuação do ADC
            .bitwidth = ADC_BITWIDTH_DEFAULT,                               // Largura de bits do ADC
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);   // Cria a calibração
        if (ret == ESP_OK)                                                  // Se a calibração foi criada com sucesso
        {
            calibrated = true;                                              // Flag de calibração igual a true
        }
    }
#endif  // Fim da calibração por linha

    *out_handle = handle;                                                   // armazena o handle da calibração
    if (ret == ESP_OK)                                                      // Se a calibração foi criada com sucesso
    {
        ESP_LOGI(TAG, "Calibration Success");                               // Imprime mensagem de sucesso
    }
    else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated)                   // Se a calibração não for suportada ou não estiver calibrado
    {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");        // Imprime mensagem de erro
    }
    else
    {
        ESP_LOGE(TAG, "Invalid arg or no memory");                          // Imprime mensagem de erro
    }

    return calibrated;                                                      // Retorna a flag de calibração
}

/*
Função de De-inicialização da Calibração do ADC
Argumentos:
    handle: Handle da calibração
retorno:
    Nenhum
*/
static void example_adc_calibration_deinit(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED                                 // Se a calibração por curva estiver habilitada
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Curve Fitting");     // Imprime a versão da calibração
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));          // De-inicializa a calibração

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED                                // Se a calibração por linha estiver habilitada
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Line Fitting");      // Imprime a versão da calibração
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));           // De-inicializa a calibração
#endif
}
