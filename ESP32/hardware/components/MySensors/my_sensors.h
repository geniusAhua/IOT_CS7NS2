#ifndef __MY_SENSORS_H__
#define __MY_SENSORS_H__

#include <stdio.h>
#include <string>
#include <cstring>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "my_log.h"

class GPS{
    private:
    typedef struct{
        uint8_t* data;
        SemaphoreHandle_t mutex_data;
    } GPS_task_t;
    int gpio_rx;
    int gpio_tx;
    static MyLog GPSLog;
    GPS_task_t parameters;

    static const int RX_BUFFER;
    static uint32_t interval;
    static const uart_config_t uart_config;

    public:
    GPS(int gpio_rx = UART_PIN_NO_CHANGE, int gpio_tx = UART_PIN_NO_CHANGE);
    std::string get_location();

    static void task_GPS(void *_data);
};//GPS


#endif