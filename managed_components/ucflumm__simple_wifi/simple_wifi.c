#include "include/simple_wifi.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include <string.h>

static const char* TAG = "wifi_manager";
static EventGroupHandle_t s_wifi_event_group;

/**
 * @brief Event handler for WiFi and IP events
 * @param arg User data
 * @param event_base Esp event base type
 * @param event_id Event ID
 * @param event_data Event data
*/

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "WiFi station started, connecting to AP...");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t* disconnected = (wifi_event_sta_disconnected_t*) event_data;
        ESP_LOGI(TAG, "Disconnected from SSID: %s, Reason: %d", disconnected->ssid, disconnected->reason);
        // Reconnect logic if not a manual disconnect
        if (disconnected->reason != WIFI_REASON_ASSOC_LEAVE) {
            ESP_LOGI(TAG, "Attempting to reconnect...");
            esp_wifi_connect();
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

/**
 * @brief Check if NVS has been initialized, and initialize it if not
*/
static void check_nvs_initialization() {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &my_handle);

    if (err == ESP_ERR_NVS_NOT_INITIALIZED) {
        ESP_LOGI("NVS", "NVS has not been initialized. Initializing now...");
        err = nvs_flash_init();
        if (err != ESP_OK) {
            ESP_LOGE("NVS", "Failed to initialize NVS: %s", esp_err_to_name(err));
        } else {
            ESP_LOGI("NVS", "NVS initialized successfully.");
        }
    } else if (err == ESP_OK) {
        ESP_LOGI("NVS", "NVS is already initialized.");
        nvs_close(my_handle); // Close handle if it was successfully opened
    } else {
        ESP_LOGE("NVS", "Error (%s) opening NVS handle!", esp_err_to_name(err));
    }
}

/**
 * @brief Initialize WiFi with the given SSID and password
 * @param ssid SSID of the WiFi network
 * @param password Password of the WiFi network
*/

void wifi_manager_init(const char* ssid, const char* password) {
    s_wifi_event_group = xEventGroupCreate();
    // This now checks if NVS has been initialized
    check_nvs_initialization();
    esp_netif_init();
    esp_event_loop_create_default();

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL);

    wifi_config_t wifi_config = {};
    strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char*)wifi_config.sta.password, password, sizeof(wifi_config.sta.password));
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();
    ESP_LOGI(TAG, "WiFi initialization complete, waiting for IP...");
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
}

