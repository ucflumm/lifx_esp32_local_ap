#include "esp_console.h"
#include "esp_log.h"
#include "include/lifx_struct_def.h"  // Assuming this is where lifx_device_t is defined

static int cmd_hello(int argc, char **argv) {
    printf("Hello ESP-IDF!\n");
    return 0;
}

static int cmd_echo(int argc, char **argv) {
    if (argc > 1) {
        printf("%s\n", argv[1]);
    } else {
        printf("Usage: echo <message>\n");
    }
    return 0;
}

static int list_lights(int argc, char** argv) {
    printf("Listing all registered LIFX devices:\n");
    lifx_device_t* current = head;
    int device_count = 0;
    while (current != NULL) {
        printf("Device %d: IP %s, Service %u, Port %u\n",
               ++device_count,
               inet_ntoa(current->ip),
               current->data.service.service,
               current->data.service.port);
        current = current->next;
    }
    if (device_count == 0) {
        printf("No LIFX devices found.\n");
    }
    return 0;
}

void register_commands(void) {
    const esp_console_cmd_t cmd_hello_def = {
        .command = "hello",
        .help = "Prints 'Hello ESP-IDF!'",
        .hint = NULL,
        .func = &cmd_hello,
    };
    esp_console_cmd_register(&cmd_hello_def);

    const esp_console_cmd_t cmd_echo_def = {
        .command = "echo",
        .help = "Echoes input to output",
        .hint = NULL,
        .func = &cmd_echo,
        .argtable = NULL
    };
    esp_console_cmd_register(&cmd_echo_def);

    const esp_console_cmd_t list_lights_cmd = {
        .command = "list_lights",
        .help = "List all registered LIFX lights",
        .hint = NULL,
        .func = &list_lights,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&list_lights_cmd));
}