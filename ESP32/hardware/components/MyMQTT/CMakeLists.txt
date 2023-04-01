idf_component_register(SRCS "my_MQTT.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES "MyLog" "MyConstant" "MyEventLoop"
                       PRIV_REQUIRES coreMQTT backoffAlgorithm posix_compat espressif__esp_secure_cert_mgr ArduinoJson)