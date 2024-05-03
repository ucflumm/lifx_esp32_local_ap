idf_component_register(SRCS "device_list.c"
                            "network_init.c"
                            "tx_encode.c"
                            "rx_parser.c"
                            "main.c"
                            "init_console.c"
                      INCLUDE_DIRS "."
                      REQUIRES console simple_wifi driver)