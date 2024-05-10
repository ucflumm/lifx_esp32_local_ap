idf_component_register(SRCS "tx_encode.c" "rx_parser.c" "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES console driver)