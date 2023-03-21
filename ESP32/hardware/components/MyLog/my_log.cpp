#include <cstring>
#include "my_log.h"
#include "my_const.h"

using namespace std;

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
static esp_log_level_t log_level = OUTPUT_LEVEL;
static const string _TAG = "MyLog";
static const int buf_size = 256;

MyLog::MyLog(const string tag):_tag(tag.length() > 50 ? "" : tag), _level(log_level){
    if(_tag == ""){
        ESP_LOGE(_TAG.data(), "The tag is too long!(char[50]) OR You set an empty tag for tag which is not allowed!");
    }
    num++;
    esp_log_level_set(this->_tag.data(), this->_level);
}

MyLog::~MyLog(){
    delete this;
}

void MyLog::set_level(esp_log_level_t level){
    if(esp_log_level_get(this->_tag.data()) != level) {
        esp_log_level_set(this->_tag.data(), level);
    }
    this->_level = level;
}

esp_log_level_t MyLog::get_level(){
    this->_level = esp_log_level_get(this->_tag.data());
    return this->_level;
}

/**
 * This is a static function. it is to set the level for all MyLog instance.
*/
void MyLog::set_Mylog_level(esp_log_level_t level){
    log_level = level;
    esp_log_level_set("*", level);
}

void MyLog::logV(const string format, ...){
    char fm[buf_size];
    va_list args;
    va_start(args, format);
    vsnprintf(fm, sizeof(fm) - 1, format.data(), args);
    va_end(args);
    ESP_LOGV(this->_tag.data(), "%s", fm);
    
}

void MyLog::logD(const string format, ...){
    char fm[buf_size];
    va_list args;
    va_start(args, format);
    vsnprintf(fm, sizeof(fm) - 1, format.data(), args);
    va_end(args);
    ESP_LOGD(this->_tag.data(), "%s", fm);
}

void MyLog::logI(const string format, ...){
    char fm[buf_size];
    va_list args;
    va_start(args, format);
    vsnprintf(fm, sizeof(fm) - 1, format.data(), args);
    va_end(args);
    ESP_LOGI(this->_tag.data(), "%s", fm);
}

void MyLog::logE(const string format, ...){
    char fm[buf_size];
    va_list args;
    va_start(args, format);
    vsnprintf(fm, sizeof(fm) - 1, format.data(), args);
    va_end(args);
    ESP_LOGE(this->_tag.data(), "%s", fm);
}

void MyLog::logW(const string format, ...){
    char fm[buf_size];
    va_list args;
    va_start(args, format);
    vsnprintf(fm, sizeof(fm) - 1, format.data(), args);
    va_end(args);
    ESP_LOGW(this->_tag.data(), "%s", fm);
}