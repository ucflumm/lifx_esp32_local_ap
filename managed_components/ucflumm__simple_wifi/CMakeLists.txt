idf_component_register(SRCS "simple_wifi.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_wifi esp_event nvs_flash)
