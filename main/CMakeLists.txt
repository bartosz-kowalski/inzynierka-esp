idf_component_register(
                    SRCS 
                        "main.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES esp_driver_i2c esp_wifi nvs_flash esp_netif esp_event esp_http_server vl53l0x-esp-idf)
