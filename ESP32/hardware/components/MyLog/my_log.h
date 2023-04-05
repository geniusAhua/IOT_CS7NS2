#ifndef __MY_LOG_H__
#define __MY_LOG_H__

#include <cstdarg>
#include <string>
#include "esp_log.h"

class MyLog{
    private:
    int num = 0;
    const std::string _tag;
    esp_log_level_t _level;

    public:
    MyLog(const std::string tag);

    ~MyLog();

    void set_level(esp_log_level_t level);

    esp_log_level_t get_level();

    static void set_Mylog_level(esp_log_level_t level);
    
    void logV(const std::string format, ...);

    void logD(const std::string format, ...);

    void logI(const std::string format, ...);

    void logW(const std::string format, ...);

    void logE(const std::string format, ...);
        
};//MyLog


#endif