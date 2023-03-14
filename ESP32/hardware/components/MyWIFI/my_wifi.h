#ifndef MY_WIFI_H
#define MY_WIFI_H

#include <string>
#include "esp_wifi.h"
#include "my_log.h"
#include "my_const.h"

class WiFi{
    private:
    const std::string SSID = WIFI_SSID;
    const std::string PWD = WIFI_PWD;

    public:
    static void init_auto();
    static void init_manu(const std::string ssid, const std::string pwd);
    static bool connect(const std::string ssid, const std::string pwd);

};//WIFI

#endif