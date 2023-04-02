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

// ESP32 GPIO12 can not be set to high
#define TXD_PIN (GPIO_NUM_21)
#define RXD_PIN (GPIO_NUM_19)

using namespace std;

MyLog demoLog(LOG_TAG_MAIN);
GPS *sensorGPS;
Ultrasonic *ultrasonic;
Servo *servo;
Humiture *humiture;
int num = 0;
uint8_t s_led_state = 0;

/**
 * typedef enum {
 *    ESP_LOG_NONE,       !< No log output
 *    ESP_LOG_ERROR,      !< Critical errors, software module can not recover on its own
 *    ESP_LOG_WARN,       !< Error conditions from which recovery measures have been taken
 *    ESP_LOG_INFO,       !< Information messages which describe normal flow of events
 *    ESP_LOG_DEBUG,      !< Extra information which is not necessary for normal use (values, pointers, sizes, etc).
 *    ESP_LOG_VERBOSE     !< Bigger chunks of debugging information, or frequent messages which can potentially flood the output.
 * } esp_log_level_t;
 **/

void test_sub_callback(const std::string topic, const std::string msg)
{
    demoLog.logI("test_sub_callback==>topic: %s, msg: %s", topic.data(), msg.data());
    return;
}



void task_GPS(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
    // GPS_task_t *parameters = (GPS_task_t *)event_handler_arg;

    gps_t *GPS_info = NULL;
    switch(event_id){
        case GPS_UPDATE:
            GPS_info = (gps_t *)event_data;
            demoLog.logI("%d/%d/%d %d:%d:%d => \r\n\t\t\t\t\t\tlatitude   = %.05f°N\r\n\t\t\t\t\t\tlongitude = %.05f°E\r\n\t\t\t\t\t\taltitude   = %.02fm\r\n\t\t\t\t\t\tspeed      = %fm/s",
                 GPS_info->date.year + 1970, GPS_info->date.month, GPS_info->date.day,
                 GPS_info->tim.hour -3, GPS_info->tim.minute, GPS_info->tim.second,
                 GPS_info->latitude, GPS_info->longitude, GPS_info->altitude, GPS_info->speed);

            // if (xSemaphoreTake(parameters->mutex_data, portMAX_DELAY) == pdTRUE){
            //     parameters->GPS_info = * new GPS_info_t{
            //         .altitude = GPS_info->altitude,
            //         .latitude = GPS_info->latitude,
            //         .longitude = GPS_info->longitude,
            //         .year = GPS_info->date.year + GPS::YEAR_BASE,
            //         .month = GPS_info->date.month,
            //         .day = GPS_info->date.day,
            //         .hour = GPS_info->tim.hour + GPS::TIME_ZONE,
            //         .minute = GPS_info->tim.minute,
            //         .second = GPS_info->tim.second
            //     };

                // xSemaphoreGive(parameters->mutex_data);
            // }
            break;
        case GPS_UNKNOWN:
            demoLog.logW("Unkown statement : %s", (char*)event_data);
            break;
        default:
            break;
    }
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
    demoLog.logI("Create a FreeRTOS Event Group and Initialize the customized EventLoop.");
    MyEventLoop::init();

    demoLog.logI("Start Init!");

    esp_log_level_set(LOG_TAG_MAIN, ESP_LOG_DEBUG);
    WiFi::connect();

    MQTT::Init();


    MQTT::Subscribe("test", test_sub_callback);
    MQTT::Subscribe("test2", test_sub_callback);

}

extern "C" void app_main(void)
{
    set_up();

    DynamicJsonDocument doc(1024);

    demoLog.logI("Start Loop!");


    doc["name"] = "John";
    doc["age"] = 43;
    doc["message"] = "Hello world!";
    int i = 0;
    while( i < 2){
        i++;
        MQTT::Publish("test", "hello world");
        
        // 获取当前时间点
        auto now = std::chrono::system_clock::now();
        
        // 转换为time_t格式
        std::time_t current_time = std::chrono::system_clock::to_time_t(now);
        
        // 转换为本地时间
        std::tm* local_time = std::localtime(&current_time);
        
        // 格式化时间字符串
        char time_str[20];
        std::strftime(time_str, sizeof(time_str), "%H:%M:%S", local_time);
        doc["time"] = time_str;
        std::string msg;
        serializeJson(doc, msg);
        MQTT::Publish("test2", msg);

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    
    }

    demoLog.logW("subscribe test2\n");

    
    while(1){
        MQTT::Publish("test", "hello world");
        
        // 获取当前时间点
        auto now = std::chrono::system_clock::now();
        
        // 转换为time_t格式
        std::time_t current_time = std::chrono::system_clock::to_time_t(now);
        
        // 转换为本地时间
        std::tm* local_time = std::localtime(&current_time);
        
        // 格式化时间字符串
        char time_str[20];
        std::strftime(time_str, sizeof(time_str), "%H:%M:%S", local_time);
        doc["time"] = time_str;
        std::string msg;
        serializeJson(doc, msg);
        MQTT::Publish("test2", msg);

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }


    sensorGPS = new GPS(PIN_GPS_RX, PIN_GPS_TX);
    // sensorGPS->add_handler(task_GPS);
    // while(1){
    //     GPS_info_t gps_info = sensorGPS->get_location();
    //     demoLog.logI("GPS latitude: %.05f°N", gps_info.latitude);
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);
    // }


    // ultrasonic = new Ultrasonic(GPIO_NUM_4, GPIO_NUM_5);
    // while(1){
    //     demoLog.logI("Ultrasonic data: %s", ultrasonic->get_distance().data());
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);
    // }

    // servo = new Servo(PIN_SERVO, LEDC_CHANNEL_0);
    // while(1) {
    //     servo->task_Servo();
    // }

    // humiture = new Humiture(PIN_HUMITURE);
    // while (1) {
    //     HumiAndTemp humiTmp = humiture->getHumiTemp();
    //     demoLog.logI("Humidity: %d, temperature: %d", humiTmp.getHumidity(), humiTmp.getTemperature());
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);
    // }
    
    Led red = Led(PIN_RED, OUTPUT_MODE);
    Led yellow = Led(PIN_YELLOW, OUTPUT_MODE);
    Led green = Led(PIN_GREEN, OUTPUT_MODE);
    while (1) 
    {
        
        red.toggle_led();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        red.toggle_led();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        yellow.toggle_led();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        yellow.toggle_led();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        green.toggle_led();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        green.toggle_led();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
