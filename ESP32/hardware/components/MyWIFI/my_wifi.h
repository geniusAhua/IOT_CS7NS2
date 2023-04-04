#ifndef __MY_WIFI_H__
#define __MY_WIFI_H__

#include <string>
#include "esp_wifi.h"
#include "my_log.h"
#include "my_const.h"

class WiFi{
    private:
    static bool isInit;
    static std::string SSID;
    static std::string PWD;
    static void _connect();
    static void initNVS();
    static void initWifiConnection(const std::string &ssid = WiFi::SSID, const std::string &pwd = WiFi::PWD);
    static void app_task(void *pt);
    static void do_disconnect();
    static void run_on_event(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data);


    friend std::string getSSID();
    friend void setSSID(const std::string ssid);
    friend std::string getPWD();
    friend void setPWD(const std::string pwd);
    friend bool getInit();
    friend void setInit(bool _is);

    public:
    static void connect();
    static void connect(const std::string ssid, const std::string pwd);

};//WIFI



#endif