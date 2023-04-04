idf_component_register(SRCS "my_event_loop.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES "MyLog" "MyConstant" esp_event)