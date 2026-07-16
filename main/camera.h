#ifndef CAMERA_H
#define CAMERA_H

#include "esp_camera.h"

#include "definitions.h"

#define CAM_PIN_PWDN    -1
#define CAM_PIN_RESET   -1

#define CAM_PIN_VSYNC   6
#define CAM_PIN_HREF    7
#define CAM_PIN_PCLK    13
#define CAM_PIN_XCLK    15
#define CAM_PIN_SDA   4
#define CAM_PIN_SCL   5

#define CAM_PIN_D7      16
#define CAM_PIN_D6      17
#define CAM_PIN_D5      18
#define CAM_PIN_D4      12
#define CAM_PIN_D3      10
#define CAM_PIN_D2      8
#define CAM_PIN_D1      9
#define CAM_PIN_D0      11

#define CAMERA_TAG "CAMERA"

sensor_t *s;

static esp_err_t camera_init(){

    camera_config_t config;

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;

    config.pin_d0 = CAM_PIN_D0;
    config.pin_d1 = CAM_PIN_D1;
    config.pin_d2 = CAM_PIN_D2;
    config.pin_d3 = CAM_PIN_D3;
    config.pin_d4 = CAM_PIN_D4;
    config.pin_d5 = CAM_PIN_D5;
    config.pin_d6 = CAM_PIN_D6;
    config.pin_d7 = CAM_PIN_D7;

    config.pin_xclk = CAM_PIN_XCLK;
    config.pin_pclk = CAM_PIN_PCLK;
    config.pin_vsync = CAM_PIN_VSYNC;
    config.pin_href = CAM_PIN_HREF ;

    config.pin_sccb_sda = CAM_PIN_SDA;
    config.pin_sccb_scl = CAM_PIN_SCL;

    config.pin_pwdn = CAM_PIN_PWDN;
    config.pin_reset = CAM_PIN_RESET;

    config.xclk_freq_hz = 20000000;

    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_FHD;
    config.jpeg_quality = 10;

    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_LATEST;

    config.fb_count = 2;

    esp_err_t err = esp_camera_init(&config);

    if (err != ESP_OK)
    {
        ESP_LOGE(CAMERA_TAG, "Camera init failed");
        return ESP_FAIL;
    }

    s = esp_camera_sensor_get();
    s->set_vflip(s, 1); // esp-idf examples

    if (s->id.PID == OV3660_PID) {
        s->set_saturation(s, -2); // esp-idf examples
        s->set_vflip(s, 1);
    }

    ESP_LOGI(CAMERA_TAG, "Camera initialized correctly");
    return ESP_OK;
}

void camera_task(void *pvParameters){

    cameraTaskParams *params = (cameraTaskParams*)pvParameters;
    while (1)
    {
        uint8_t inactive = params->active == 1 ? 0 : 1; 
        camera_fb_t *fb = esp_camera_fb_get();

        //xSemaphoreTake(params->mutex, portMAX_DELAY);
        memcpy(params->frame[inactive], fb->buf, fb->len); 

        xSemaphoreTake(params->mutex, portMAX_DELAY);
        params->frameLen[inactive] = fb->len;
        params->active = inactive;
        xSemaphoreGive(params->mutex);

        esp_camera_fb_return(fb);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

#endif