#ifndef __MY_SENSORS_H__
#define __MY_SENSORS_H__

#include <stdio.h>
#include <string>
#include <cstring>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "nmea_parser.h"
#include "my_log.h"
#include "ultrasonic.h"
#include "iot_servo.h"
#include "dht.h"
#include "my_const.h"

typedef struct{
    float   altitude = 0;
    float   latitude = 0;
    float   longitude = 0;
    float   speed = 0;
    int     year = 0;
    int     month = 0;
    int     day = 0;
    int     hour = 0;
    int     minute = 0;
    int     second = 0;
} GPS_info_t;

class GPS
{
private:

    typedef struct
    {
        GPS_info_t GPS_info;
        SemaphoreHandle_t mutex_data;
    } GPS_task_t;

    int gpio_rx;
    int gpio_tx;    
    static MyLog GPSLog;
    GPS_task_t parameters;
    uint8_t UART_buffer;

    static const int RX_BUFFER_SIZE;
    static uint32_t interval;
    static const uart_config_t uart_config;
    nmea_parser_config_t GPS_config;
    nmea_parser_handle_t nmea_handler;
    static const uint8_t TIME_ZONE;
    static const uint16_t YEAR_BASE;

    static void task_GPS(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

public:
    GPS(uint32_t gpio_rx = UART_PIN_NO_CHANGE);
    const GPS_info_t get_location();
    void add_handler(void(*handler)(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data));
    
}; // GPS

class Ultrasonic 
{   
private:
    typedef struct
    {
        uint32_t *dist;
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
    uint32_t get_distance();

    static void task_Ultrasonic(void *_dist);
};

class HumiAndTemp
{
private:
    int16_t humidity;
    int16_t temperature;
public:
    HumiAndTemp(int16_t humidity = 0, int16_t temperature = 0);
    int16_t getHumidity();
    int16_t getTemperature();
};

class Humiture
{
private:
    typedef struct 
    {
        gpio_num_t pin;
        int16_t *humidity;
        int16_t *temperature;
        dht_sensor_type_t sensor_type;
        SemaphoreHandle_t mutex_humiture;
    } Humiture_task_t;
    
    static MyLog HumitureLog;
    Humiture_task_t parameters;

public:
    Humiture(gpio_num_t pin);
    static void task_Humiture(void *_parameters);
    HumiAndTemp getHumiTemp();
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

class Led
{
private:
    gpio_num_t pin;
    gpio_mode_t mode;    
    uint8_t s_led_state;

public:
    Led(gpio_num_t pin, gpio_mode_t mode);
    void toggle_led();
    void led_lightUp();
    void led_lightDown();
};

#endif
