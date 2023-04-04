idf_component_register(SRCS "my_OTA.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES "MyConstant" "MyLog" "MyMQTT"
                       PRIV_REQUIRES log esp_event nvs_flash app_update esp_http_client driver ArduinoJson mbedtls)