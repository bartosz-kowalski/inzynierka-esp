#ifndef SENSOR_H
#define SENSOR_H

#include "i2c.h"
#include "VL53L0X.h"

#include "definitions.h"

#define SDA 20
#define SCL 21

#define SENSOR_TAG "ToF_SENSOR"

#define MAX_RANGE 1200
#define MIN_RANGE 40

VL53L0X sensor;

static esp_err_t sensor_init(){

    i2c.init(SCL, SDA);
    sensor.setTimeout(500);     // timeout time set as in the lib example

    if(!sensor.init(true)){     // sensor operating in 2V8 mode
        ESP_LOGE(SENSOR_TAG, "Failed to initialize the ToF sensor");
        return ESP_FAIL;
    }

    ESP_LOGI(SENSOR_TAG, "ToF sensor initialized correctly");
    return ESP_OK;
}

void sensor_task(void *pvParameters){

    sensorTaskParams *params = (sensorTaskParams*)pvParameters;
    uint16_t temp;
    uint8_t tooClose = 0;   // local flag; preventing the drone from going too close
    uint16_t currentVal = 1000;
    float alpha = 0.2;      // EMA "smoothing factor", for better smoothing (slower range changes) decrease the value, for faster changes increase it

    sensor.startContinuous(200);   // continuous range metering (5 Hz)

    while (1)
    {
        temp = sensor.readRangeContinuousMillimeters();
        if(temp > MAX_RANGE){
            temp = MAX_RANGE;   // capping the sensor reading at MAX_RANGE
        }
        if(temp < MIN_RANGE){
            temp = MIN_RANGE;   
        }
        currentVal = alpha * temp + (1.0 - alpha) * currentVal;   // exponential smoothing of the sensor readings
        if (sensor.timeoutOccurred()) {
            temp = 0;
        }
        else{

            xSemaphoreTake(params->mutex, portMAX_DELAY);
            params->oldRange = params->rangeMilimeters;     // described in "definitions.h"
            params->rangeMilimeters = currentVal;           // described in "definitions.h"
            xSemaphoreGive(params->mutex);

            if(currentVal <= 700 && tooClose == 0){
                tooClose = 1;
                eStop = 1;  // global flag; notifying control_task to ignore all movement instructions
                ESP_LOGW(SENSOR_TAG, "Too close to the target. Distance: %d mm", currentVal);
            }
            else if(tooClose == 1 && currentVal >= 1000){
                tooClose = 0;
                eStop = 0;  // global flag; notifying control_task to stop ignoring all movement instructions
                ESP_LOGI(SENSOR_TAG, "Target far enough to go. Distance: %d mm", currentVal);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

#endif
