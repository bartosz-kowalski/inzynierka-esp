#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "wifi.h"
#include "camera.h"
#include "server.h"
#include "sensor.h"
#include "control.h"

#define CORE_0 0
#define CORE_1 1

extern "C" void app_main(void)
{

    uint8_t *frameBuf[2];
    frameBuf[0] = (uint8_t*)heap_caps_malloc(1024 * 1024, MALLOC_CAP_SPIRAM);
    frameBuf[1] = (uint8_t*)heap_caps_malloc(1024 * 1024, MALLOC_CAP_SPIRAM);
    SemaphoreHandle_t frameMutex = xSemaphoreCreateMutex();
    SemaphoreHandle_t sensorMutex = xSemaphoreCreateMutex();

    static cameraTaskParams camParams = {
        .frame = { frameBuf[0], frameBuf[1] },
        .frameLen = { 0, 0 },
        .active = 0,
        .mutex = frameMutex
    };

    static sensorTaskParams sensorParams = {
        .rangeMilimeters = 0,
        .oldRange = 0,
        .mutex = sensorMutex
    };

    static controlTaskParams ctrlParams = {};

    static serverTaskParams serverParams = {
        .cmr = camParams,
        .ctrl = ctrlParams
    };

    ESP_ERROR_CHECK(wifi_init());
    ESP_ERROR_CHECK(camera_init());
    ESP_ERROR_CHECK(server_init(&serverParams));
    ESP_ERROR_CHECK(sensor_init());
    
    xTaskCreatePinnedToCore(camera_task, "camera_task", 4096, &camParams, 4, NULL, CORE_0); //camera capture task
    xTaskCreate(sensor_task, "sensor_task", 3072, &sensorParams, 2, NULL); // ultrasonic range sensor task (TOP PRIORITY)
    xTaskCreatePinnedToCore(control_task, "control_task", 4096, NULL, 1, &controlTaskHandle, CORE_0); // drone communication task
}
