idf_component_register(SRCS "console_cmd.c" "console.c" "tx_encode.c" "rx_parser.c" "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES console driver)