idf_component_register(SRCS "my_sensors.cpp" "nmea_parser.c"
                       INCLUDE_DIRS "."
                       REQUIRES "MyLog" "MyConstant" "MyEventLoop"
                       PRIV_REQUIRES driver ultrasonic servo humidity)