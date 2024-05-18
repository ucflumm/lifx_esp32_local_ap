#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_console.h"
#include "esp_log.h"
#include "include/main.h"

static const char* TAG = "console_task";

// In this function I've removed the NVS INIT because the wifi component calls it.
// This is a temp solution to double NVS INIT which might knock the wifi off.
void console_task(void *pvParameters) {
    ESP_LOGI(TAG, "Initializing console in task");

    // Set up REPL configuration and start REPL
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = CONFIG_IDF_TARGET ">";

    // Choose console type based on configuration
#if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) || defined(CONFIG_ESP_CONSOLE_UART_CUSTOM)
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));
#elif defined(CONFIG_ESP_CONSOLE_USB_CDC)
    esp_console_dev_usb_cdc_config_t usb_cdc_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&usb_cdc_config, &repl_config, &repl));
#elif defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG)
    esp_console_dev_usb_serial_jtag_config_t usb_serial_jtag_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&usb_serial_jtag_config, &repl_config, &repl));
#else
    #error Unsupported console type
#endif

    ESP_ERROR_CHECK(esp_console_start_repl(repl));
    register_commands();
    vTaskDelay(100 / portTICK_PERIOD_MS); // Keep the task running indefinitely (until user exits REPL

    vTaskDelete(NULL); // Clean up the task if exited
}