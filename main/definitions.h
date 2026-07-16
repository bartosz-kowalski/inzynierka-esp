#ifndef DEF_H
#define DEF_H

#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_log.h"

volatile uint8_t eStop = 0; // global flag that tells the FC to hover in place if too close to the target

typedef struct{
    uint8_t *frame[2];  // double buffering of captured frames - 1 MB each
    size_t frameLen[2]; // length of stored  frames
    uint8_t active;     // which buffer is now active
    SemaphoreHandle_t mutex;    // preventing tasks from altering memory that the other task is now using
}cameraTaskParams;

typedef struct{

}controlTaskParams;

typedef struct{
    cameraTaskParams *cmr;
    controlTaskParams *ctrl;
}serverTaskParams;

typedef struct{
    uint16_t rangeMilimeters;   // not used now
    uint16_t oldRange;          // not used now
    SemaphoreHandle_t mutex;
}sensorTaskParams;

#endif
