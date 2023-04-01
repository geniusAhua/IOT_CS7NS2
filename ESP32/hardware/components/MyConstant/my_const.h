#ifndef __MY_CONST_H__
#define __MY_CONST_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

//Name of task log
#define LOG_TAG_MAIN                    "SmartBin-Main"
#define LOG_TAG_WIFI                    "SmartBin-WiFi"
#define LOG_TAG_EVENT_LOOP              "SmartBin-Event-Loop"
#define LOG_TAG_SENSORS_GPS             "SmartBin-Sensors_GPS"
#define LOG_TAG_SENSORS_ULTRASONIC      "SmartBin-Sensors_Ultrasonic"
#define LOG_TAG_SERVO                   "SmartBin-Servo"
#define LOG_TAG_HUMITURE                "SmartBin-Humiture"

//System Configuration
#define WIFI_SSID                       "刘阿花的iPhone"
#define WIFI_PWD                        "qwertyuiop"
#define WIFI_MAX_RETRY_NUM              10
#define MAX_DISTANCE_CM                 200

//Name of event base
#define SMARTBIN_EVENT_BASE             "SmartBin-Event"

//Name of tasks
#define TASK_NAME_WIFI_APP              "WIFI_APP_TASK"
#define TASK_NAME_GPS                   "GPS_TASK"
#define TASK_NAME_ULTRASONIC            "ULTRASONIC_TASK"
#define TASK_NAME_HUMITURE              "HUMITURE_TASK"

//Configuration of PIN
#define PIN_RED                         GPIO_NUM_25
#define PIN_YELLOW                      GPIO_NUM_26
#define PIN_GREEN                       GPIO_NUM_27
#define OUTPUT_MODE                     GPIO_MODE_OUTPUT
#define PIN_GPS_TX                      GPIO_NUM_22
#define PIN_GPS_RX                      GPIO_NUM_23
#define OUTPUT_LEVEL                    ESP_LOG_VERBOSE

#define PIN_ULTRASONIC_TRIGGER          GPIO_NUM_4
#define PIN_ULTRASONIC_ECHO             GPIO_NUM_5

#define PIN_SERVO                       GPIO_NUM_19

#define PIN_HUMITURE                    GPIO_NUM_21

#define PIN_GREEN_LED                   GPIO_NUM_32

// configuration of Servo
#define SERVO_MAX_ANGLE                 180
#define SERVO_MIN_WIDTH                 500                 
#define SERVO_MAX_WIDTH                 2500
#define SERVO_FREQ                      50                 

//Variable for FreeRTOS system
#define STA_START BIT0
#define WIFI_DONE BIT1
#define MQTT_     BIT2

#endif