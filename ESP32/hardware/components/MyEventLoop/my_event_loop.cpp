#include "my_event_loop.h"

//protperties of MyEventLoop
bool MyEventLoop::isInit = false;
MyLog MyEventLoop::eventLoopLog(LOG_TAG_EVENT_LOOP);
const esp_event_base_t MyEventLoop::SMARTBIN_BASE = SMARTBIN_EVENT_BASE;
const EventGroupHandle_t MyEventLoop::_s_wifi_event_group = xEventGroupCreate();
esp_event_loop_handle_t MyEventLoop::smartBin_loop_handler;
// typedef struct {
//     int32_t queue_size;                         /**< size of the event loop queue */
//     const char *task_name;                      /**< name of the event loop task; if NULL,
//                                                         a dedicated task is not created for event loop*/
//     UBaseType_t task_priority;                  /**< priority of the event loop task, ignored if task name is NULL */
//     uint32_t task_stack_size;                   /**< stack size of the event loop task, ignored if task name is NULL */
//     BaseType_t task_core_id;                    /**< core to which the event loop task is pinned to,
//                                                         ignored if task name is NULL */
// } esp_event_loop_args_t;
esp_event_loop_args_t MyEventLoop::smartBin_loop_args = {
    .queue_size = static_cast<uint32_t>(smartBin_event_t::Count) + 10,
    .task_name = "SmartBin_Event_Loop",
    .task_priority = uxTaskPriorityGet(NULL),
    .task_stack_size = 1024*5,                  //5KB
    .task_core_id = tskNO_AFFINITY,
};

void MyEventLoop::init(){
    if(!MyEventLoop::isInit){
        MyEventLoop::isInit = true;
        eventLoopLog.logI("Create an Event Loop for SmartBin.");
        ESP_ERROR_CHECK(esp_event_loop_create(&smartBin_loop_args, &smartBin_loop_handler));
    }
}//MyEventLoop::init()

void MyEventLoop::post_event_to(smartBin_event_t eventId, const void *eventData, size_t eventDataSize, TickType_t ticksToWait){
    if(MyEventLoop::isInit){
        ESP_ERROR_CHECK(esp_event_post_to(MyEventLoop::smartBin_loop_handler, MyEventLoop::SMARTBIN_BASE, static_cast<int32_t>(eventId), eventData, eventDataSize, ticksToWait));
    }
    else{
        MyEventLoop::eventLoopLog.logE("Please Initialize MyEventLoop first");
    }
}

void MyEventLoop::smartBin_handler_register(smartBin_event_t eventId, esp_event_handler_t eventHandler, void *eventHandlerArgs){
    if(MyEventLoop::isInit){
        ESP_ERROR_CHECK(esp_event_handler_register_with(MyEventLoop::smartBin_loop_handler, MyEventLoop::SMARTBIN_BASE, static_cast<int32_t>(eventId), eventHandler, eventHandlerArgs));
    }
    else{
        MyEventLoop::eventLoopLog.logE("Please Initialize MyEventLoop first");
    }
}

EventGroupHandle_t MyEventLoop::smartBin_event_group(){
    return MyEventLoop::_s_wifi_event_group;
}

// MyEventLoop::~MyEventLoop(){
//     //Delete
//     MyEventLoop::isInit = false;
//     ESP_ERROR_CHECK(esp_event_loop_delete(&smartBin_loop_handler));
// }//~MyEventLoop()
