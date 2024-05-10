#include <stdio.h>

#include "include/main.h"
#include "simple_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/wifi_cred.h"

#include "esp_system.h"
#include 
void app_main(void)
{
    wifi_manager_init(SSID, PASSWORD);
    xTaskCreate(send_discovery_message, "send_discovery", 2048, NULL, 5, NULL);
    xTaskCreate(receive_responses, "receive_responses", 4096, NULL, 5, NULL);
}