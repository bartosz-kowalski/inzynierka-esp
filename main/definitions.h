#ifndef DEF_H
#define DEF_H

#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_log.h"

volatile uint8_t eStop = 0;

TaskHandle_t controlTaskHandle = NULL;

typedef struct{
    uint8_t *frame[2];
    size_t frameLen[2];
    uint8_t active;
    SemaphoreHandle_t mutex;
}cameraTaskParams;

typedef struct{

}controlTaskParams;

typedef struct{
    cameraTaskParams cmr;
    controlTaskParams ctrl;
}serverTaskParams;

typedef struct{
    uint16_t rangeMilimeters;
    uint16_t oldRange;
    SemaphoreHandle_t mutex;
}sensorTaskParams;

#endif