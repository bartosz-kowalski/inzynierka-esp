#ifndef SERVER_H
#define SERVER_H

#include "esp_log.h"
#include "esp_err.h"
#include "esp_netif.h"
#include "esp_http_server.h"

#define SERVER_TAG "WEB_SERVER"
#define HTTP_PORT "80"

static esp_err_t server_init(){

    return ESP_OK;
}

void server_task(void *pvParameters){

}

#endif