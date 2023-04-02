#ifndef __MY_EVENT_LOOP_H__
#define __MY_EVENT_LOOP_H__

#include <vector>
#include <stdio.h>
#include "esp_event.h"
#include "my_log.h"
#include "my_const.h"

// typedef enum{
//     WIFI_CONFIG_DONE = 0,
//     MQTT_,
// } smartBin_event_t;

enum class smartBin_event_t : int32_t{
    WIFI_CONFIG_DONE = 0,   //eventData: NULL
    MQTT_SUB_BACK,
    MQTT_PUBLISH,
    MQTT_UNSUBACK,
    MQTT_PINGRESP,
    MQTT_PUBACK,
    MQTT_UNKNOWN,
    Count
};

class MyEventLoop{
    private:
    static bool isInit;
    static esp_event_loop_handle_t smartBin_loop_handler;
    static esp_event_loop_args_t smartBin_loop_args;
    static const EventGroupHandle_t _s_wifi_event_group;
    static MyLog eventLoopLog;

    public:
    static const esp_event_base_t SMARTBIN_BASE;
    static void init();
    static void smartBin_handler_register(smartBin_event_t eventId, esp_event_handler_t eventHandler, void *eventHandlerArgs);
    static void post_event_to(smartBin_event_t eventId, const void *eventData, size_t eventDataSize, TickType_t ticksToWait);
    //get the event group to wait for the event
    static EventGroupHandle_t smartBin_event_group();
    // ~MyEventLoop();
};//MyEventLoop

#endif