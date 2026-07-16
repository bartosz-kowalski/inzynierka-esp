#ifndef SENSOR_H
#define SENSOR_H

#include "i2c.h"
#include "VL53L0X.h"

#include "definitions.h"

#define SDA 20
#define SCL 21

#define SENSOR_TAG "ToF_SENSOR"

VL53L0X sensor;

static esp_err_t sensor_init(){

    i2c.init(SCL, SDA);
    sensor.setTimeout(500);

    if(sensor.init()!=ESP_OK){
        ESP_LOGE(SENSOR_TAG, "Failed to initialize the ToF sensor");
        return ESP_FAIL;
    }

    ESP_LOGI(SENSOR_TAG, "ToF sensor initialized correctly");
    return ESP_OK;
}

void sensor_task(void *pvParameters){

    sensorTaskParams *params = (sensorTaskParams*)pvParameters;
    uint16_t temp;
    uint8_t tooClose = 0;   // preventing the drone from going too close

    sensor.startContinuous();   // continuous range metering (back-to-back mode)

    while (1)
    {
        temp = sensor.readRangeContinuousMillimeters();
        if (sensor.timeoutOccurred()) {
            temp = 0;
        }
        else{

            xSemaphoreTake(params->mutex, portMAX_DELAY);
            params->oldRange = params->rangeMilimeters;
            params->rangeMilimeters = temp;
            xSemaphoreGive(params->mutex);

            if(temp <= 750 && tooClose == 0){
                tooClose = 1;
                eStop = 1;  // notifying control_task to stop ignoring all movement instructions
                ESP_LOGW(SENSOR_TAG, "Too close to the target. Distance: %d mm", temp);
            }
            else if(tooClose == 1 && temp >= 1600){
                tooClose = 0;
                eStop = 0;  // notifying control_task to stop ignoring all movement instructions
                ESP_LOGI(SENSOR_TAG, "Target far enough to go. Distance: %d mm", temp);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

#endif