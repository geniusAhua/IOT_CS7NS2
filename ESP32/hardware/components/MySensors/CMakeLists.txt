idf_component_register(SRCS "my_sensors.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES "MyLog" "MyConstant" "MyEventLoop"
                       PRIV_REQUIRES driver)