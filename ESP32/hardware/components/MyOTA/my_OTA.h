#ifndef __MY_OTA_H__
#define __MY_OTA_H__

#include <stdio.h>
#include <string>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_app_format.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"
#include "esp_crt_bundle.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "errno.h"
#include "driver/gpio.h"
#include "ArduinoJson.h"

#include "my_const.h"
#include "my_log.h"

class OTA{
    public:
    static void Init();

    private:
    static const uint16_t BUFFSIZE;
    static const uint16_t OTA_URL_SIZE;
    static const uint8_t HASH_LEN;

    static std::string URL;

    static MyLog OTALog;
    static char ota_write_data[];
    
    static void http_cleanup(esp_http_client_handle_t client);
    static void __attribute__((noreturn)) task_fatal_error(void);
    static void print_sha256 (const uint8_t *image_hash, const std::string label);
    
    static void task_ota(void *parameters);
    static void task_MQTT_OTA_callback(std::string topic, std::string message);

    static esp_err_t _http_event_handler(esp_http_client_event_t *evt);
    static bool diagnostic(void);
};

#endif