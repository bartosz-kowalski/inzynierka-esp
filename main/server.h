#ifndef SERVER_H
#define SERVER_H

#include "esp_netif.h"
#include "esp_event.h"
#include "esp_http_server.h"

#include "definitions.h"

#define SERVER_TAG "WEB_SERVER"
#define HTTP_PORT 80

httpd_handle_t server = NULL;

static esp_err_t frame_get_handler(httpd_req_t *req)
{
    cameraTaskParams *params = (cameraTaskParams *)req->user_ctx;

    xSemaphoreTake(params->mutex, portMAX_DELAY);   // blocking the camera_task form altering images during http request service

    uint8_t active = params->active;        // described in "definitions.h"
    uint8_t *buf = params->frame[active];   
    size_t len = params->frameLen[active];  

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_send(req, (char *)buf, len);

    xSemaphoreGive(params->mutex); 

    return ESP_OK;
}

static esp_err_t frame_post_handler(httpd_req_t *req)
{
    controlTaskParams *params = (controlTaskParams *)req->user_ctx;

    /*xSemaphoreTake(params->mutex, portMAX_DELAY);

    uint8_t active = params->active;
    uint8_t *buf = params->frame[active];
    size_t len = params->frameLen[active];

    xSemaphoreGive(params->mutex);

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_send(req, (char *)buf, len);*/

    return ESP_OK;
}

static httpd_uri_t uri_get = {
    .uri = "/frame",
    .method = HTTP_GET,
    .handler = frame_get_handler,
    .user_ctx = NULL
};

static httpd_uri_t uri_post = {
    .uri = "/frame",
    .method = HTTP_POST,
    .handler = frame_post_handler,
    .user_ctx = NULL
};

static esp_err_t server_init(serverTaskParams *params){

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();    

    uri_get.user_ctx = params->cmr;
    uri_post.user_ctx = params->ctrl;

    config.core_id = 1;
    config.task_priority = 3;
    config.server_port = HTTP_PORT;     // user may change the port no if needed
    config.stack_size = 8192;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post);
        ESP_LOGI(SERVER_TAG, "HTTP Server started on port %d", HTTP_PORT);
        return ESP_OK;
    }

    ESP_LOGE(SERVER_TAG, "Failed to start HTTP server");
    return ESP_FAIL;
}

#endif
