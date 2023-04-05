#ifndef __MY_CONST_H__
#define __MY_CONST_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

//Name of task log
#define LOG_TAG_MAIN                                "SmartBin-Main"
#define LOG_TAG_WIFI                                "SmartBin-WiFi"
#define LOG_TAG_EVENT_LOOP                          "SmartBin-Event-Loop"
#define LOG_TAG_SENSORS_GPS                         "SmartBin-Sensors_GPS"
#define LOG_TAG_SENSORS_ULTRASONIC                  "SmartBin-Sensors_Ultrasonic"
#define LOG_TAG_SERVO                               "SmartBin-Servo"
#define LOG_TAG_HUMITURE                            "SmartBin-Humiture"
#define LOG_TAG_MQTT                                "SmartBin-MQTT"
#define LOG_TAG_OTA                                 "SmartBin-OTA"

//SmartBin distance (cm)
#define DISTANCE_NORMAL                             100
#define DISTANCE_TENSIVE                            13
#define DISTANCE_EMERGENCY                          5
#define DISTANCE_DETECT_INTERVAL                    50

//MQTT
#define AWS_IOT_ENDPOINT                            "a3ravxmuj0c4e2-ats.iot.eu-west-1.amazonaws.com"
#define AWS_IOT_CLIENT_ID                           "SmartBin1"
#define PUBLISH_TOPIC_SMARTBIN_INFO                 "SmartBinInfo"
#define SUBSCRIBE_TOPIC1                            "control"
#define SUBSCRIBE_TOPIC2                            "instruction"
#define AWS_IOT_ENDPOINT_LENGTH                     ( ( uint16_t ) sizeof( AWS_IOT_ENDPOINT ) - 1 )
#define AWS_IOT_CLIENT_ID_LENGTH                    ( ( uint16_t ) sizeof( AWS_IOT_CLIENT_ID ) - 1 )
#define AWS_IOT_MQTT_PORT                           8883
#define AWS_IOT_MQTT_QOS                            MQTTQoS1
#define AWS_IOT_MQTT_MAX_SUBSCRIPTIONS              5
#define AWS_IOT_CONNACK_RECV_TIMEOUT_MS             ( 7000U )
#define AWS_IOT_KEEP_ALIVE_INTERVAL                 ( 60U )
#define AWS_IOT_INCOMING_PUBLISH_BUFFER_SIZE        ( 10U )
#define AWS_IOT_OUTGOING_PUBLISH_BUFFER_SIZE        ( 10U )
#define MQTT_PROCESS_LOOP_TIMEOUT_MS                ( 10000U )
#define MQTT_CONNECTION_RETRY_BACKOFF_BASE_MS       ( 500U )
#define MQTT_CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS  ( 128000U )

//System Configuration
#define WIFI_SSID                                   "刘阿花的iPhone"
//#define WIFI_SSID                                   "一起爬山吗"
#define WIFI_PWD                                    "qwertyuiop"
#define WIFI_MAX_RETRY_NUM                          100
#define NETWORK_BUFFER_SIZE                         1024

//for ultrasonic
#define MAX_DISTANCE_CM                             300

//Name of event base
#define SMARTBIN_EVENT_BASE                         "SmartBin-Event"

//Name of tasks
#define TASK_NAME_WIFI_APP                          "WIFI_APP_TASK"
#define TASK_NAME_GPS                               "GPS_TASK"
#define TASK_NAME_ULTRASONIC                        "ULTRASONIC_TASK"
#define TASK_NAME_HUMITURE                          "HUMITURE_TASK"
#define TASK_NAME_PUBLISH_CYCLE                     "PUBLISH_CYCLE"
#define TASK_NAME_DETECT_DISTANCE                   "DETECT_DISTANCE"
#define TASK_NAME_BIN_OPEN                          "BIN_OPEN"
#define TASK_NAME_BIN_CLOSE                         "BIN_CLOSE"
#define TASK_NAME_BIN_COMPRESS                      "BIN_COMPRESS"

//Configuration of PIN
#define PIN_RED                                     GPIO_NUM_25
#define PIN_YELLOW                                  GPIO_NUM_26
#define PIN_GREEN                                   GPIO_NUM_27
#define OUTPUT_MODE                                 GPIO_MODE_OUTPUT
#define PIN_GPS_RX                                  GPIO_NUM_23
#define OUTPUT_LEVEL                                ESP_LOG_VERBOSE
#define PIN_ULTRASONIC_TRIGGER                      GPIO_NUM_4
#define PIN_ULTRASONIC_ECHO                         GPIO_NUM_5
#define PIN_SERVO                                   GPIO_NUM_19
#define PIN_HUMITURE                                GPIO_NUM_21
#define PIN_GREEN_LED                               GPIO_NUM_32

//OTA
#define PIN_ROLLBACK                                GPIO_NUM_12
#define OTA_RECV_TIMEOUT                            5000

// configuration of Servo
#define SERVO_MAX_ANGLE                             180
#define SERVO_MIN_WIDTH                             500                 
#define SERVO_MAX_WIDTH                             1500
#define SERVO_FREQ                                  50

//Variable for FreeRTOS system
#define STA_START       BIT0
#define WIFI_DONE       BIT1
#define MQTT_START      BIT2
#define MQTT_INIT       BIT3
#define OTA_UPDATE      BIT4
#define BIN_OPEN        BIT5

#endif