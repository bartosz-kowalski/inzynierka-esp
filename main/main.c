#include <stdio.h>
#include "wifi.h"
#include "camera.h"
#include "server.h"

#define CORE_0 0
#define CORE_1 1

void app_main(void)
{
    ESP_ERROR_CHECK(wifi_init());
    ESP_ERROR_CHECK(camera_init());
    ESP_ERROR_CHECK(server_init());
    
    xTaskCreatePinnedToCore(camera_task, "camera_task", 4096, NULL, 3, NULL, CORE_0); //camera snapshots task
    xTaskCreatePinnedToCore(server_task, "server_task", 4096, NULL, 2, NULL, CORE_1); // http server task
    // ultrasonic distance sensor task (TOP PRIORITY)
    // drone communication task
    
}
