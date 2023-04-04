idf_component_register(SRCS "my_wifi.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES "MyLog" "MyConstant" "MyEventLoop"
                       PRIV_REQUIRES esp_wifi nvs_flash)