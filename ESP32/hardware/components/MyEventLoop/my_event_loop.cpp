#include "my_event_loop.h"
#include "my_const.h"
#include "my_log.h"

//protperties of MyEventLoop
bool MyEventLoop::isInit = false;
esp_event_base_t MyEventLoop::SMARTBIN_BASE = SMARTBIN_EVENT_BASE;
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
    .queue_size = 5,
    .task_name = "SmartBin_Event_Loop",
    .task_priority = uxTaskPriorityGet(NULL),
    .task_stack_size = 1024*5,                  //5KB
    .task_core_id = tskNO_AFFINITY,
};

//variable
MyLog eventLoopLog(LOG_TAG_EVENT_LOOP);


void MyEventLoop::init(){
    eventLoopLog.logD("Create an Event Loop for SmartBin.");
    ESP_ERROR_CHECK(esp_event_loop_create(&smartBin_loop_args, &smartBin_loop_handler));
    vTaskDelete(NULL);
}//MyEventLoop::init()

void MyEventLoop::post_event_to(smartBin_event_t eventId, const void *eventData, size_t eventDataSize, TickType_t ticksToWait){
    ESP_ERROR_CHECK(esp_event_post_to(MyEventLoop::smartBin_loop_handler, MyEventLoop::SMARTBIN_BASE, static_cast<int32_t>(eventId), eventData, eventDataSize, ticksToWait));
}

void MyEventLoop::smartBin_handler_register(smartBin_event_t eventId, esp_event_handler_t eventHandler, void *eventHandlerArgs){
    ESP_ERROR_CHECK(esp_event_handler_register_with(MyEventLoop::smartBin_loop_handler, MyEventLoop::SMARTBIN_BASE, static_cast<int32_t>(eventId), eventHandler, eventHandlerArgs));
}

MyEventLoop::~MyEventLoop(){
    //Delete
    ESP_ERROR_CHECK(esp_event_loop_delete(&smartBin_loop_handler));
}//~MyEventLoop()
