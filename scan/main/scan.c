/***************************************************************
Example: Scan Example - based on Espressif Example
Description: This example shows how to scan for available set of APs.
Autor: Fábio Souza

obs: This example is based on the example provided by Espressif.

Use the menuconfig to configure the number of APs to scan for.

****************************************************************/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"

#define DEFAULT_SCAN_LIST_SIZE CONFIG_EXAMPLE_SCAN_LIST_SIZE    // number of APs listed in scan list defined in menuconfig

static const char *TAG = "scan";                // tag for ESP_LOGx macros

/*
function to print authentication mode of AP
argument:
    authmode : authmode of AP
return:
    none
*/
static void print_auth_mode(int authmode)   
{
    switch (authmode) {                             // switch case to print authmode of AP
    case WIFI_AUTH_OPEN:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OPEN");
        break;
    case WIFI_AUTH_OWE:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OWE");
        break;
    case WIFI_AUTH_WEP:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WEP");
        break;
    case WIFI_AUTH_WPA_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_PSK");
        break;
    case WIFI_AUTH_WPA2_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_PSK");
        break;
    case WIFI_AUTH_WPA_WPA2_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_WPA2_PSK");
        break;
    case WIFI_AUTH_WPA2_ENTERPRISE:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_ENTERPRISE");
        break;
    case WIFI_AUTH_WPA3_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_PSK");
        break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_WPA3_PSK");
        break;
    default:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_UNKNOWN");
        break;
    }
}

/*
Function to print cipher type of AP
argument:
    pairwise_cipher : pairwise cipher of AP
    group_cipher : group cipher of AP
return:
    none
*/
static void print_cipher_type(int pairwise_cipher, int group_cipher)
{
    switch (pairwise_cipher) {                                      // switch case to print pairwise cipher of AP
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    case WIFI_CIPHER_TYPE_AES_CMAC128:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_AES_CMAC128");
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_SMS4");
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP");
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP256");
        break;
    default:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }

    switch (group_cipher) {                                     // switch case to print group cipher of AP
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_SMS4");
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP");
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP256");
        break;
    default:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }
}

/*
Function to scan for available APs
argument:
    none
return:
    none
*/
static void wifi_scan(void)
{
    ESP_ERROR_CHECK(esp_netif_init());                                  // Initialize TCP/IP network interface (esp-netif)
    ESP_ERROR_CHECK(esp_event_loop_create_default());                   // Create default event loop
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();       // Create default event loop
    assert(sta_netif);                                                  // Check if the network interface was created

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();                // Initialize WiFi with default configuration
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));                               // Initialize WiFi            

    uint16_t number = DEFAULT_SCAN_LIST_SIZE;                           // Number of APs to scan for
    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];                   // Array to store APs found during scan
    uint16_t ap_count = 0;                                              // Number of APs found during scan
    memset(ap_info, 0, sizeof(ap_info));                                // Clear array

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));                  // Set WiFi mode to station
    ESP_ERROR_CHECK(esp_wifi_start());                                  // Start WiFi
    esp_wifi_scan_start(NULL, true);                                    // Start WiFi scan       
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));    // Get number of APs found during scan
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));               // Get APs found during scan
    ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);                  // Print number of APs found during scan
    for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {          // Loop through APs found during scan
        ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);                              // Print SSID of AP
        ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi);                              // Print RSSI of AP
        print_auth_mode(ap_info[i].authmode);                                       // Print authentication mode of AP
        if (ap_info[i].authmode != WIFI_AUTH_WEP) {                                 // If AP is not using WEP encryption
            print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher); // Print cipher type of AP
        }
        ESP_LOGI(TAG, "Channel \t\t%d\n", ap_info[i].primary);                      // Print primary channel of AP
    }
}

// main function
void app_main(void)
{
    esp_err_t ret = nvs_flash_init();                                                   // Initialize NVS
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {     // If NVS was not initialized
        ESP_ERROR_CHECK(nvs_flash_erase());                                             // Erase NVS
        ret = nvs_flash_init();                                                         // Initialize NVS again
    }
    ESP_ERROR_CHECK( ret );                                                             // Check if NVS was initialized        

    wifi_scan();                                                                        // Scan for available APs
}
