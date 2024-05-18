#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_console.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "driver/uart.h"
#include "esp_console.h"
#include <unistd.h>
#include <stdint.h>
#include "esp_vfs.h"
#include "vfs/vfs_uart.h"

void initialize_console(void) {
    /* Drain stdout before reconfiguring it */
    fflush(stdout);
    fsync(fileno(stdout));

    /* Disable buffering on stdin */
    setvbuf(stdin, NULL, _IONBF, 0);

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    esp_vfs_dev_uart_port_set_rx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_uart_port_set_tx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CRLF);

    /* Install UART driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK( uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM,
                                         256, 0, 0, NULL, 0) );

    /* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);

    /* Initialize the console */
    esp_console_config_t console_config = {
        .max_cmdline_args = 8,
        .max_cmdline_length = 256,
#if CONFIG_LOG_COLORS
        .hint_color = atoi(LOG_COLOR_CYAN)
#endif
    };
    ESP_ERROR_CHECK( esp_console_init(&console_config) );

    /* Configure linenoise line completion library */
    /* Enable multiline editing. If not set, long commands will scroll within single line */
    linenoiseSetMultiLine(1);

    /* Tell linenoise where to get command completions and hints */
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback*) &esp_console_get_hint);

    /* Set command history size */
    linenoiseHistorySetMaxLen(100);
}

int cmd_light_control(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: set_power <power_level> <duration>\n");
        return 1;
    }

    uint16_t power = strtol(argv[1], NULL, 0);
    uint32_t duration = strtol(argv[2], NULL, 0);
    printf("Setting light power to %d with duration %d\n", power, duration);

    // Build the payload
    lifx_payload_set_power_t* payload = build_set_power_payload(power, duration);
    if (payload == NULL) {
        ESP_LOGE("cmd_light_control", "Failed to allocate payload");
        return 1;
    }

    // Prepare destination address
    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(56700);
    dest.sin_addr.s_addr = inet_addr("255.255.255.255"); // Example: Broadcast address

    // Assume sock is globally available or passed as needed
    extern int sock;

    // Build and send the message
    uint8_t target[8] = {0}; // All zeros for broadcast to all devices
    lifx_message_t* message = build_lifx_message(target, sizeof(lifx_payload_set_power_t), payload, send_lifx_message, sock, &dest);

    free(payload);
    if (message != NULL) {
        free(message); // Free the message if not handled inside send_lifx_message
    }

    return 0;
}

void register_commands(void) {
    const esp_console_cmd_t cmd = {
        .command = "set_power",
        .help = "Set the power of the light: set_power <power_level>",
        .hint = NULL,
        .func = &cmd_light_control,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}