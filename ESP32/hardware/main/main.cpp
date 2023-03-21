#include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "my_const.h"
#include "my_wifi.h"
#include "my_log.h"
#include "my_event_loop.h"
#include "my_sensors.h"

//ESP32 GPIO12 can not be set to high
#define TXD_PIN (GPIO_NUM_21)
#define RXD_PIN (GPIO_NUM_19)

using namespace std;

MyLog demoLog(LOG_TAG_MAIN);
GPS *sensorGPS;
int num = 0;
extern EventGroupHandle_t s_wifi_event_group = xEventGroupCreate();
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
/**
 * 串口通信
 * 1. 设置通信参数 - 设置波特率，数据位，停止位等
 * 2. 设置通信引脚 - 为连接设备分配引脚
 * 3. 驱动安装 - 为UART驱动分配ESP32的资源
 * 4. 运行UART通信 - 发送/接受数据
 * 5. 使用中断 - 在特定的通信事件中触发中断
 * 6. 删除驱动程序 - 如果不再需要UART通信，释放分配的资源
*/

/*
task_list()
显示当前的所有FreeRTOS任务
使用前，请在menuconfig中启动
Enable FreeRTOS trace facility
Enable FreeRTOS stats formatting functions
*/
void set_up(){
    demoLog.logI("Create a FreeRTOS Event Group");
    
    demoLog.logI("Start Init!");

    esp_log_level_set(LOG_TAG_MAIN, ESP_LOG_DEBUG);
    WiFi::connect();

    gpio_reset_pin(PIN_BUTTON1);
    gpio_set_direction(PIN_BUTTON1, PIN_BUTTON1_MOD);
}


extern "C" void app_main(void)
{
    set_up();

    sensorGPS = new GPS(PIN_GPS_RX, PIN_GPS_TX);
    // _data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    // init_uart2();
    // xTaskCreate(rx2_task, "GPS_task", 1024 * 4, NULL, configMAX_PRIORITIES, NULL);
    while(1){
        demoLog.logI("GPS data: %s", sensorGPS->get_location().data());
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
