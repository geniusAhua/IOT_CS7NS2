#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "my_log.h"
#include "driver/gpio.h"
#include "my_const.h"
#include "my_wifi.h"

using namespace std;

MyLog demoLog(LOG_TAG_MAIN);
int num = 0;
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
void set_up(){
    MyLog::set_Mylog_level(ESP_LOG_INFO);
    WIFI::init();

    gpio_reset_pin(PIN_BUTTON1);
    gpio_set_direction(PIN_BUTTON1, PIN_BUTTON1_MOD);
}

extern "C" void app_main(void)
{
    set_up();

    while(1){
        demoLog.logI("demo - count: %d", num);
        num++;
        //1s = 1000 / portTICK_PERIOD_MS
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
