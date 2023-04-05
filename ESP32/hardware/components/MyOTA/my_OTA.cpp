#include "my_OTA.h"
#include "my_MQTT.h"


const uint16_t OTA::BUFFSIZE = 1024;
const uint16_t OTA::OTA_URL_SIZE = 256;
const uint8_t OTA::HASH_LEN = 32;

std::string OTA::URL;

MyLog OTA::OTALog(LOG_TAG_OTA);
char OTA::ota_write_data[OTA::BUFFSIZE + 1] = {0};

void OTA::Init(){
    OTALog.logI("start OTA task.");

    uint8_t sha_256[OTA::HASH_LEN] = {0};
    esp_partition_t partition;

    // get sha256 digest for the partition table
    partition.address   = ESP_PARTITION_TABLE_OFFSET;
    partition.size      = ESP_PARTITION_TABLE_MAX_LEN;
    partition.type      = ESP_PARTITION_TYPE_DATA;
    esp_partition_get_sha256(&partition, sha_256);
    print_sha256(sha_256, "SHA-256 for the partition table: ");

    // get sha256 digest for bootloader
    partition.address   = ESP_BOOTLOADER_OFFSET;
    partition.size      = ESP_PARTITION_TABLE_OFFSET;
    partition.type      = ESP_PARTITION_TYPE_APP;
    esp_partition_get_sha256(&partition, sha_256);
    print_sha256(sha_256, "SHA-256 for bootloader: ");

    // get sha256 digest for running partition
    esp_partition_get_sha256(esp_ota_get_running_partition(), sha_256);
    print_sha256(sha_256, "SHA-256 for current firmware: ");

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            // run diagnostic function ...
            bool diagnostic_is_ok = diagnostic();
            if (diagnostic_is_ok) {
                OTA::OTALog.logI("Diagnostics completed successfully! Continuing execution ...");
                esp_ota_mark_app_valid_cancel_rollback();
            } else {
                OTA::OTALog.logI("Diagnostics failed! Start rollback to the previous version ...");
                esp_ota_mark_app_invalid_rollback_and_reboot();
            }
        }
    }

    MQTT::Subscribe(SUBSCRIBE_TOPIC1, OTA::task_MQTT_OTA_callback);
    
}

void OTA::print_sha256(const uint8_t *image_hash, const std::string label){
    char hash_print[HASH_LEN * 2 + 1];
    hash_print[HASH_LEN * 2] = 0;
    for (int i = 0; i < HASH_LEN; ++i) {
        sprintf(&hash_print[i * 2], "%02x", image_hash[i]);
    }
    OTA::OTALog.logI("%s: %s", label.c_str(), hash_print);
}

void OTA::http_cleanup(esp_http_client_handle_t client)
{
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
}

void __attribute__((noreturn)) OTA::task_fatal_error(void)
{
    OTA::OTALog.logI("Exiting task due to fatal error...");
    (void)vTaskDelete(NULL);
    
    // This function should never return, but adding a while loop here
    // will ensure that the task is blocked indefinitely if the system
    // restart fails for some reason
    while (1) {
        ;
    }
}

bool OTA::diagnostic(void)
{
    gpio_config_t io_conf;
    io_conf.intr_type    = GPIO_INTR_DISABLE;
    io_conf.mode         = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << PIN_ROLLBACK);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en   = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    OTA::OTALog.logI("Diagnostics (5 sec)...");
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    bool diagnostic_is_ok = gpio_get_level(PIN_ROLLBACK);

    gpio_reset_pin(PIN_ROLLBACK);
    return diagnostic_is_ok;
}

void OTA::task_MQTT_OTA_callback(std::string topic, std::string message){
    DynamicJsonDocument msg(1024);

    deserializeJson(msg, message);

    const char* _url = msg["url"];
    OTA::URL = _url;
    OTA::OTALog.logI("topic==> < %s > | msg:==> < %s >", topic.c_str(), message.c_str());
    xTaskCreate(&OTA::task_ota, "ota_example_task", 8192, NULL, 5, NULL);

    return;
}

esp_err_t OTA::_http_event_handler(esp_http_client_event_t *evt){
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        OTA::OTALog.logE("HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        OTA::OTALog.logI("HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        OTA::OTALog.logI("HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        OTA::OTALog.logI("HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        OTA::OTALog.logD("HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        OTA::OTALog.logI("HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        OTA::OTALog.logW("HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        OTA::OTALog.logW("HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;
}

void OTA::task_ota(void *parameters){
    esp_err_t err;
    /* update handle : set by esp_ota_begin(), must be freed via esp_ota_end() */
    esp_ota_handle_t update_handle = 0 ;
    const esp_partition_t *update_partition = NULL;

    OTA::OTALog.logI("Starting OTA example task");

    const esp_partition_t *configured = esp_ota_get_boot_partition();
    const esp_partition_t *running = esp_ota_get_running_partition();

    if (configured != running) {
        OTA::OTALog.logW("Configured OTA boot partition at offset 0x%08" PRIx32 ", but running from offset 0x%08" PRIx32,
                 configured->address, running->address);
        OTA::OTALog.logW("(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)");
    }

    OTA::OTALog.logI("Running partition type %d subtype %d (offset 0x%08" PRIx32 ")",
             running->type, running->subtype, running->address);

    esp_http_client_config_t config = {
        .url = OTA::URL.c_str(),
        .timeout_ms = OTA_RECV_TIMEOUT,
        .event_handler = OTA::_http_event_handler,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .keep_alive_enable = true,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        OTA::OTALog.logE("Failed to initialise HTTP connection");
        task_fatal_error();
    }
    err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        OTA::OTALog.logE("Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        task_fatal_error();
    }
    esp_http_client_fetch_headers(client);

    update_partition = esp_ota_get_next_update_partition(NULL);
    assert(update_partition != NULL);
    OTA::OTALog.logI("Writing to partition subtype %d at offset 0x%"PRIx32,
             update_partition->subtype, update_partition->address);

    int binary_file_length = 0;
    /*deal with all receive packet*/
    bool image_header_was_checked = false;
    while (1) {
        int data_read = esp_http_client_read(client, ota_write_data, BUFFSIZE);
        if (data_read < 0) {
            OTA::OTALog.logE("Error: SSL data read error");
            http_cleanup(client);
            task_fatal_error();
        } else if (data_read > 0) {
            if (image_header_was_checked == false) {
                esp_app_desc_t new_app_info;
                if (data_read > /*sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t) + sizeof(esp_app_desc_t)*/ 0 ) {
                    // check current version with downloading
                    memcpy(&new_app_info, &ota_write_data[sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t)], sizeof(esp_app_desc_t));
                    OTA::OTALog.logI("New firmware version: %s", new_app_info.version);

                    esp_app_desc_t running_app_info;
                    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK) {
                        OTA::OTALog.logI("Running firmware version: %s", running_app_info.version);
                    }

                    const esp_partition_t* last_invalid_app = esp_ota_get_last_invalid_partition();
                    esp_app_desc_t invalid_app_info;
                    if (esp_ota_get_partition_description(last_invalid_app, &invalid_app_info) == ESP_OK) {
                        OTA::OTALog.logI("Last invalid firmware version: %s", invalid_app_info.version);
                    }

                    // check current version with last invalid partition
                    if (last_invalid_app != NULL) {
                        if (memcmp(invalid_app_info.version, new_app_info.version, sizeof(new_app_info.version)) == 0) {
                            OTA::OTALog.logW("New version is the same as invalid version.");
                            OTA::OTALog.logW("Previously, there was an attempt to launch the firmware with %s version, but it failed.", invalid_app_info.version);
                            OTA::OTALog.logW("The firmware has been rolled back to the previous version.");
                            http_cleanup(client);
                            vTaskDelete(NULL);
                        }
                    }

                    if (memcmp(new_app_info.version, running_app_info.version, sizeof(new_app_info.version)) == 0) {
                        OTA::OTALog.logW("Current running version is the same as a new. We will not continue the update.");
                        http_cleanup(client);
                        vTaskDelete(NULL);
                    }


                    image_header_was_checked = true;

                    err = esp_ota_begin(update_partition, OTA_WITH_SEQUENTIAL_WRITES, &update_handle);
                    if (err != ESP_OK) {
                        OTA::OTALog.logE("esp_ota_begin failed (%s)", esp_err_to_name(err));
                        http_cleanup(client);
                        esp_ota_abort(update_handle);
                        task_fatal_error();
                    }
                    OTA::OTALog.logI("esp_ota_begin succeeded");
                } else {
                    OTA::OTALog.logE("received package is not fit len, data-read: %d\n\n", data_read);
                    http_cleanup(client);
                    esp_ota_abort(update_handle);
                    task_fatal_error();
                }
            }
            err = esp_ota_write( update_handle, (const void *)ota_write_data, data_read);
            if (err != ESP_OK) {
                http_cleanup(client);
                esp_ota_abort(update_handle);
                task_fatal_error();
            }
            binary_file_length += data_read;
            OTA::OTALog.logD("Written image length %d", binary_file_length);
        } else if (data_read == 0) {
           /*
            * As esp_http_client_read never returns negative error code, we rely on
            * `errno` to check for underlying transport connectivity closure if any
            */
            if (errno == ECONNRESET || errno == ENOTCONN) {
                OTA::OTALog.logE("Connection closed, errno = %d", errno);
                break;
            }
            if (esp_http_client_is_complete_data_received(client) == true) {
                OTA::OTALog.logI("Connection closed");
                break;
            }
        }
    }
    OTA::OTALog.logI("Total Write binary data length: %d", binary_file_length);
    if (esp_http_client_is_complete_data_received(client) != true) {
        OTA::OTALog.logE("Error in receiving complete file");
        http_cleanup(client);
        esp_ota_abort(update_handle);
        task_fatal_error();
    }

    err = esp_ota_end(update_handle);
    if (err != ESP_OK) {
        if (err == ESP_ERR_OTA_VALIDATE_FAILED) {
            OTA::OTALog.logE("Image validation failed, image is corrupted");
        } else {
            OTA::OTALog.logE("esp_ota_end failed (%s)!", esp_err_to_name(err));
        }
        http_cleanup(client);
        task_fatal_error();
    }

    err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK) {
        OTA::OTALog.logE("esp_ota_set_boot_partition failed (%s)!", esp_err_to_name(err));
        http_cleanup(client);
        task_fatal_error();
    }
    OTA::OTALog.logI("Prepare to restart system!");
    esp_restart();
    
    vTaskDelete(NULL);
}