set(srcs #"wifi.c"
         #"camera.c"
         #"server.c"
         "main.c")

idf_component_register(
                    SRCS ${srcs}
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi esp_event nvs_flash lwip)
