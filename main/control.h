#ifndef CONTROL_H
#define CONTROL_H

#include "definitions.h"

#define CONTROL_TAG "CONTROL_SYSTEM"

void control_task(void *pvParameters){

    controlTaskParams *params = (controlTaskParams*)pvParameters;
    uint8_t ignoreRPi = 0;

    while(1){
        if(eStop == 1){
            // hover in place
        }
        else{
            // go accordingly to indtructions form rPi
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

#endif