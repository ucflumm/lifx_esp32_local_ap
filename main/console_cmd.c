#include "esp_console.h"
#include "esp_log.h"

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
}