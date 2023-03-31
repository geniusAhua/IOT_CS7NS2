#ifndef __MY_SENSORS_H__
#define __MY_SENSORS_H__

#include <stdio.h>
#include <string>
#include <cstring>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "my_log.h"
#include "ultrasonic.h"
#include "iot_servo.h"

class GPS
{
private:
    typedef struct
    {
        uint8_t *data;
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
}; // GPS

class Ultrasonic 
{   
private:
    typedef struct
    {
        float *dist;
        SemaphoreHandle_t mutex_dist;
        ultrasonic_sensor_t sensor;
    } Ultrasonic_task_t;

    gpio_num_t trigger;
    gpio_num_t echo;
    static MyLog UltrasonicLog;
    Ultrasonic_task_t parameters;
    // static ultrasonic_sensor_t sensor;

public:
    Ultrasonic(gpio_num_t gpio_trigger, gpio_num_t gpio_echo);
    std::string get_distance();

    static void task_Ultrasonic(void *_dist);
};

class Servo
{   
private:
    static MyLog ServoLog;
    gpio_num_t pin;
    servo_config_t servo_cfg;

public:
    Servo(gpio_num_t pin, ledc_channel_t chan);
    void task_Servo();
    // void set_angle_to_zero();
};

#endif
