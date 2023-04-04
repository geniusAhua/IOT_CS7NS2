#include <stdio.h>
#include <chrono>
#include <ctime>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "ArduinoJSON.h"
#include "my_const.h"
#include "my_wifi.h"
#include "my_log.h"
#include "my_event_loop.h"
#include "my_sensors.h"
#include "my_MQTT.h"
#include "my_OTA.h"

//#ifndef CONFIG_EXAMPLE_USE_CERT_BUNDLE
//#error "In this SmartBin demo, you must set USE_CERT_BUNDLE, this configuration!"
//#endif

// ESP32 GPIO12 can not be set to high

/// @brief the interval second to publish the info of SmartBin to the topic
#define INTERVAL_PUBLISH_MQTT (10)


using namespace std;

MyLog demoLog(LOG_TAG_MAIN);
GPS *sensorGPS;
Ultrasonic *sensorUltrasonic;
Servo *servo;
Humiture *sensorHumiture;
int num = 0;
uint8_t s_led_state = 0;

bool isOpen = false;

Led *red;
Led *yellow;
Led *green;

/**
 * @brief 0: normal
 *        1: tensive
 *        2: emergency
 */
uint8_t emer_level = 0;

void task_bin_open(void *parameters){
    xEventGroupSetBits(MyEventLoop::smartBin_event_group(), BIN_OPEN);

    if(!isOpen) {
        isOpen = true;
        servo->task_Servo();
    }
    
    vTaskDelete(NULL);
}

void task_bin_close(void *parameters){
    xEventGroupClearBits(MyEventLoop::smartBin_event_group(), BIN_OPEN);
    if(isOpen){
        isOpen = false;
        green->led_lightDown();
        yellow->led_lightDown();
        red->led_lightUp();
        servo->task_Servo();
    }
    vTaskDelete(NULL);
}

void task_bin_compress(void *parameters){
    if(isOpen){
        xEventGroupClearBits(MyEventLoop::smartBin_event_group(), BIN_OPEN);

        green->led_lightDown();
        yellow->led_lightDown();
        red->led_lightUp();
        servo->task_Servo();
        vTaskDelay(pdMS_TO_TICKS(1000));
        servo->task_Servo();
        red->led_lightDown();

        xEventGroupSetBits(MyEventLoop::smartBin_event_group(), BIN_OPEN);
    }
    
    vTaskDelete(NULL);
}

void instruction_callback(const std::string topic, const std::string msg)
{
    demoLog.logI("test_sub_callback==>topic: %s, msg: %s", topic.data(), msg.data());

    DynamicJsonDocument json_msg(1024);
    deserializeJson(json_msg, msg);

    uint8_t _type = json_msg["type"];

    switch(_type){
        case 0:
            xTaskCreate(task_bin_open, TASK_NAME_BIN_OPEN, 1024 * 3, NULL, 1, NULL);
            break;
        case 1:
            xTaskCreate(task_bin_close, TASK_NAME_BIN_CLOSE, 1024 * 3, NULL, 1, NULL);
            break;
        case 2:
            xTaskCreate(task_bin_compress, TASK_NAME_BIN_COMPRESS, 1024 * 3, NULL, 1, NULL);
            break;
        default:
            demoLog.logW("Unknown type: %d\n\n", _type);
            break;
    }

    return;
}

void task_publish_info(void *parameters){

    while(1){
        DynamicJsonDocument SmartBinInfo(1024);
        DynamicJsonDocument GPSJson(1024);
        DynamicJsonDocument UltrasonicJson(sizeof(uint32_t) + 100);
        DynamicJsonDocument HumitureJson(sizeof(uint16_t) * 2 + 100);

        GPS_info_t gps_info = sensorGPS->get_location();
        uint32_t ultrasonic_info = sensorUltrasonic->get_distance();
        HumiAndTemp humiture_info = sensorHumiture->getHumiTemp();
        uint8_t _level = emer_level;

        GPSJson["latitude"] = gps_info.latitude;
        GPSJson["longitude"] = gps_info.longitude;
        GPSJson["altitude"] = gps_info.altitude;

        UltrasonicJson["distance"] = ultrasonic_info;

        HumitureJson["temperature"] = humiture_info.getTemperature();
        HumitureJson["Humidity"] = humiture_info.getHumidity();

        SmartBinInfo["time"] = to_string(gps_info.year) + "-" + to_string(gps_info.month) + "-" + to_string(gps_info.day) + " " + to_string(gps_info.hour) + ":" + to_string(gps_info.minute) + ":" + to_string(gps_info.second);
        SmartBinInfo["GPS"] = GPSJson;
        SmartBinInfo["ultrasonic"] = UltrasonicJson;
        SmartBinInfo["humiture"] = HumitureJson;
        SmartBinInfo["EmergencyLevel"] = _level;

        string msg;
        serializeJson(SmartBinInfo, msg);
        MQTT::Publish(PUBLISH_TOPIC_SMARTBIN_INFO, msg);

        vTaskDelay(pdMS_TO_TICKS(10000));
    }

    vTaskDelete(NULL);
}

void task_dect_distance(void *parameters){
    uint32_t distance = 0;
    uint32_t tmp_distance[3] = {0};
    uint32_t pre_distance = 0;
    while(1){
        xEventGroupWaitBits(MyEventLoop::smartBin_event_group(), BIN_OPEN, pdFALSE, pdFALSE, portMAX_DELAY);
        distance = sensorUltrasonic->get_distance();
        demoLog.logI("distance: %05d", distance);
        if(distance < DISTANCE_EMERGENCY){ // distance < 4cm
            emer_level = 2; //emergency
            
            green->led_lightDown();
            yellow->led_lightDown();
            red->led_lightUp();
        }
        else if(distance >= DISTANCE_EMERGENCY && distance < DISTANCE_TENSIVE){
            emer_level = 1; //tensive

            green->led_lightDown();
            yellow->led_lightUp();
            red->led_lightDown();
        }
        else{
            emer_level = 0; // normal

            green->led_lightUp();
            yellow->led_lightDown();
            red->led_lightDown();
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    vTaskDelete(NULL);
}

/**
 * logic:
 *  - Ultrasonic
 *      - get capacity data and send to aws iot
 *  - Led (work with ultrasonic):
 *      - three levels -- Low: turn green; Medium: turn yello; High: red;
 *  - GPS:
 *      - get geo location data and send to aws iot
 *  - Humidity:
 *      - get humidity data and send to aws iot
*/
void set_up()
{
    demoLog.logI("Start Init!");

    demoLog.logI("Create a FreeRTOS Event Group and Initialize the customized EventLoop.\n");
    MyEventLoop::init();

    demoLog.logI("Start detecting distance!!\n");
    sensorGPS = new GPS(PIN_GPS_RX);
    vTaskDelay(pdMS_TO_TICKS(500));
    sensorUltrasonic = new Ultrasonic(GPIO_NUM_4, GPIO_NUM_5);
    servo = new Servo(PIN_SERVO, LEDC_CHANNEL_0);
    sensorHumiture = new Humiture(PIN_HUMITURE);
    green = new Led(PIN_GREEN, OUTPUT_MODE);
    yellow = new Led(PIN_YELLOW, OUTPUT_MODE);
    red = new Led(PIN_RED, OUTPUT_MODE);
    if(!isOpen){
        green->led_lightDown();
        yellow->led_lightDown();
        red->led_lightUp();
    }
    xTaskCreate(task_dect_distance, TASK_NAME_DETECT_DISTANCE, 1024 * 4, NULL, 2, NULL);

    demoLog.logI("Start WIFI and MQTT init\n!");
    WiFi::connect();
    MQTT::Init();
    OTA::Init();

    vTaskDelay(pdMS_TO_TICKS(500));

    MQTT::Subscribe(SUBSCRIBE_TOPIC2, instruction_callback);

    demoLog.logI("Initialization has finished!\n\n");
    xTaskCreate(task_publish_info, TASK_NAME_PUBLISH_CYCLE, 1024 * 5, NULL, 3, NULL);
    
}

extern "C" void app_main(void)
{
    set_up();

    

//    DynamicJsonDocument doc(1024);
//
//    demoLog.logI("Start Loop!");
//
//
//    doc["name"] = "John";
//    doc["age"] = 43;
//    doc["message"] = "Hello world!";
//    while( 1 ){
//        MQTT::Publish("test", "hello world");
//        
//        // 获取当前时间点
//        auto now = std::chrono::system_clock::now();
//        
//        // 转换为time_t格式
//        std::time_t current_time = std::chrono::system_clock::to_time_t(now);
//        
//        // 转换为本地时间
//        std::tm* local_time = std::localtime(&current_time);
//        
//        // 格式化时间字符串
//        char time_str[20];
//        std::strftime(time_str, sizeof(time_str), "%H:%M:%S", local_time);
//        doc["time"] = time_str;
//        std::string msg;
//        serializeJson(doc, msg);
//        MQTT::Publish("test2", msg);
//
//        vTaskDelay(5000 / portTICK_PERIOD_MS);
//    
//    }

    
    // sensorGPS->add_handler(task_GPS);
    // while(1){
    //    GPS_info_t gps_info = sensorGPS->get_location();
    //    demoLog.logI("GPS latitude: %.09f°N", gps_info.latitude);
    //    DynamicJsonDocument Json(1024);
    //    DynamicJsonDocument GPS(1024);
    //    GPS["time"] = to_string(gps_info.year) + "-" + to_string(gps_info.month) + "-" + to_string(gps_info.day) + " " + to_string(gps_info.hour) + ":" + to_string(gps_info.minute) + ":" + to_string(gps_info.second);
    //    GPS["latitude"] = gps_info.latitude;
    //    GPS["longitude"] = gps_info.longitude;
    //    GPS["altitude"] = gps_info.altitude;
    //    Json["GPS"] = GPS;
    //    string msg;
    //    serializeJson(Json, msg);
    //    MQTT::Publish("MQTT-test", msg);
    //    MQTT::Publish("test2", "Hello World!");
    //    vTaskDelay(2000 / portTICK_PERIOD_MS);
    // }


     
    // while(1){
    //     demoLog.logI("Ultrasonic data: %.02f", ultrasonic->get_distance());
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);
    // }

     
    // while(1) {
    //     servo->task_Servo();
    // }

     
    // while (1) {
    //     HumiAndTemp humiTmp = humiture->getHumiTemp();
    //     demoLog.logI("Humidity: %d, temperature: %d", humiTmp.getHumidity(), humiTmp.getTemperature());
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);
    // }
    
    
    //while (1) 
    //{
    //    
    //    red.toggle_led();
    //    vTaskDelay(2000 / portTICK_PERIOD_MS);
    //    red.toggle_led();
    //    vTaskDelay(2000 / portTICK_PERIOD_MS);
    //    yellow.toggle_led();
    //    vTaskDelay(2000 / portTICK_PERIOD_MS);
    //    yellow.toggle_led();
    //    vTaskDelay(2000 / portTICK_PERIOD_MS);
    //    green.toggle_led();
    //    vTaskDelay(2000 / portTICK_PERIOD_MS);
    //    green.toggle_led();
    //    vTaskDelay(2000 / portTICK_PERIOD_MS);
    //}
}
