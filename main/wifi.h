#ifndef WIFI_H
#define WIFI_H

#define DEBUG

#include "esp_wifi.h"
#include "nvs_flash.h"

#define WIFI_TAG "WIFI"

#define AP_SSID "ESP32_AP"
#define AP_PASSWORD "12345678"

static esp_err_t wifi_init(){
    #ifdef DEBUG
    ESP_ERROR_CHECK(nvs_flash_erase()); 
    #endif
    nvs_flash_init();

    esp_netif_init();

    esp_event_loop_create_default();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t ap_config = {
        .ap = {
            .ssid = AP_SSID,
            .ssid_len = strlen(AP_SSID),
            .password = AP_PASSWORD,
            .channel = 6,
            .max_connection = 3,
            .authmode = WIFI_AUTH_WPA2_PSK
        }
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(WIFI_TAG, "AP started");
    ESP_LOGI(WIFI_TAG, "SSID: %s", AP_SSID);
    ESP_LOGI(WIFI_TAG, "PASS: %s", AP_PASSWORD);

    esp_netif_ip_info_t ip_info;
    esp_netif_t *ap_netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");

    ESP_ERROR_CHECK(esp_netif_get_ip_info(ap_netif, &ip_info));
    ESP_LOGI(WIFI_TAG, "Drone IP: " IPSTR, IP2STR(&ip_info.ip));

    return ESP_OK;
}

#endif
