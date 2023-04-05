#include <stdio.h>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "my_log.h"
#include "my_wifi.h"
#include "my_const.h"
#include "my_event_loop.h"

std::string WiFi::SSID = WIFI_SSID;
std::string WiFi::PWD = WIFI_PWD;
bool WiFi::isInit = false;
//friend functions
std::string getSSID()                       { return WiFi::SSID; }
std::string getPWD()                        { return WiFi::PWD; }
bool        getInit()                       { return WiFi::isInit; }
void        setSSID(const std::string ssid) { WiFi::SSID = ssid; }
void        setPWD(const std::string pwd)   { WiFi::PWD = pwd; }
void        setInit(bool _is)               { WiFi::isInit = _is; }

MyLog wifiLog(LOG_TAG_WIFI);
static int s_retry_num = 0;
// static const std::string tag_scan = "SCAN";
static const std::string tag_conn = "CONN";
static SemaphoreHandle_t s_semph_get_ip_addrs = NULL;


void WiFi::connect(){
    _connect();
}

void WiFi::connect(const std::string ssid, const std::string pwd){
    if(pwd.length() < 8){
        wifiLog.logE("Given SSID and PWD is not allowed, please check them!\n Suspending for 5s......");
        vTaskDelay(5000/portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(ESP_FAIL);
    }
    setSSID(ssid);
    setPWD(pwd);
    _connect();
}

void WiFi::_connect(){
    if(!getInit()){
        setInit(true);
        //1 initialize the wifi's NVS
        WiFi::initNVS();
        //start an app task
        //Declare a semaphore for blocking the app_task
        s_semph_get_ip_addrs = xSemaphoreCreateBinary();
        if (s_semph_get_ip_addrs == NULL) {
                ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
        }
        wifiLog.logI("Create an App Task.");
        xTaskCreate(WiFi::app_task, TASK_NAME_WIFI_APP, 1024 * 10, NULL, 1, NULL);
        // initialize wifi_configuration and then start wifi
        WiFi::initWifiConnection();
    }

    return;
}

void WiFi::initNVS(){// first step

    wifiLog.logI("0. initiate NVS storage.");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        /* NVS partition was truncated
         * and needs to be erased */
        ESP_ERROR_CHECK(nvs_flash_erase());

        /* Retry nvs_flash_init */
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    

    wifiLog.logI("1. WIFI initiation phase.");
    //1.1 initialize;
    ESP_ERROR_CHECK(esp_netif_init());
    //1.2 creat an event loop;
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    //1.3 create default network interface instance;
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t _wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&_wifi_init_config);
}

void WiFi::initWifiConnection(const std::string &ssid, const std::string &pwd){
    wifiLog.logI("ssid: %s, pwd: %s", ssid.data(), pwd.data());
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = {0},
            .password = {0},
            .scan_method = WIFI_ALL_CHANNEL_SCAN,
            .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
            .threshold = {
                .rssi = -127,
                .authmode = WIFI_AUTH_WPA2_PSK,
            },
        },
    };
    //memset(wifi_config.sta.ssid, 0, sizeof(wifi_config.sta.ssid));
    //memset(wifi_config.sta.password, 0, sizeof(wifi_config.sta.password));
    //std::copy(ssid.begin(), ssid.end(), wifi_config.sta.ssid);
    //std::copy(pwd.begin(), pwd.end(), wifi_config.sta.password);
    strncpy(reinterpret_cast<char*>(wifi_config.sta.ssid), ssid.c_str(), sizeof(wifi_config.sta.ssid) - 1);
    strncpy(reinterpret_cast<char*>(wifi_config.sta.password), pwd.c_str(), sizeof(wifi_config.sta.password) - 1);

    wifiLog.logI("2. WIFI configure phase.");
    //2.1 set wifi mode;
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    wifiLog.logI("3. WIFI starting phase.");
    //3.1 start wifi;
    ESP_ERROR_CHECK(esp_wifi_start());
}

void WiFi::app_task(void *pt){

    esp_event_handler_instance_t instance_any_id, instance_got_ip;

    wifiLog.logI("registe an event handler.");
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &run_on_event, (void *)tag_conn.data(), &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &run_on_event, (void *)tag_conn.data(), &instance_got_ip);

    xEventGroupWaitBits(MyEventLoop::smartBin_event_group(), STA_START, pdFALSE, pdFALSE, portMAX_DELAY);
    wifiLog.logI("Connecting to %s...", WiFi::SSID.data());
    esp_err_t ret = esp_wifi_connect();
    if (ret != ESP_OK) {
        wifiLog.logE("WiFi connect failed! ret:%x", ret);
        ESP_ERROR_CHECK(ret);
        vTaskDelete(NULL);
    }
    wifiLog.logI("Waiting for IP(s)");
    
    //Waiting for an IP address
    if(xSemaphoreTake(s_semph_get_ip_addrs, portMAX_DELAY) == pdTRUE){
        wifiLog.logI("wifi component finished.");
        if (s_retry_num > WIFI_MAX_RETRY_NUM) {
            ESP_ERROR_CHECK(ESP_FAIL);
        }
    }
    vTaskDelete(NULL);
}


void WiFi::run_on_event(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data){
    char* _tag = (char*) handler_arg;
    if(_tag == tag_conn){
        if(base == WIFI_EVENT){
            switch (id)
            {
                case WIFI_EVENT_STA_START:
                    xEventGroupSetBits(MyEventLoop::smartBin_event_group(), STA_START);
                    break;
                case WIFI_EVENT_STA_DISCONNECTED:
                    do_disconnect();
                    break;
                default:
                    break;
            }
        }
        else if(base == IP_EVENT){
            ip_event_got_ip_t* ip_config = (ip_event_got_ip_t *) event_data;
            switch(id){
                case IP_EVENT_STA_GOT_IP:
                    wifiLog.logI("Got IP event, IP address:" IPSTR, IP2STR(&ip_config->ip_info.ip));
                    MyEventLoop::post_event_to(smartBin_event_t::WIFI_CONFIG_DONE, NULL, 0, portMAX_DELAY);
                    s_retry_num = 0;
                    if (s_semph_get_ip_addrs) {
                        xSemaphoreGive(s_semph_get_ip_addrs);
                    } else {
                        wifiLog.logI("- IPv4 address: " IPSTR ",", IP2STR(&ip_config->ip_info.ip));
                    }
                    break;
                default:
                    break;
            }
        }
    }
    return;
}

void WiFi::do_disconnect(){
    s_retry_num++;
    if (s_retry_num > WIFI_MAX_RETRY_NUM) {
        wifiLog.logE("WiFi Connect failed %d times, stop reconnect.", s_retry_num);
        /* let example_wifi_sta_do_connect() return */
        if (s_semph_get_ip_addrs) {
            //release a semaphor
            xSemaphoreGive(s_semph_get_ip_addrs);
        }
        return;
    }
    wifiLog.logI("Wi-Fi disconnected, trying to reconnect...");
    //retry
    esp_err_t err = esp_wifi_connect();
    if (err == ESP_ERR_WIFI_NOT_STARTED) {
        return;
    }
    ESP_ERROR_CHECK(err);
}


// wifiLog.logI("4. WIFI 扫描.");
//     //4.1 set country code;
//     wifi_country_t country_config = {
//         .cc = "IE",
//         .schan = 1,
//         .nchan = 13,
//         .policy = WIFI_COUNTRY_POLICY_AUTO,
//     };
//     esp_wifi_set_country(&country_config);

//     //4.2 start scaning;
//     // typedef struct {
//     //     uint8_t *ssid;               /**< SSID of AP */
//     //     uint8_t *bssid;              /**< MAC address of AP */
//     //     uint8_t channel;             /**< channel, scan the specific channel */
//     //     bool show_hidden;            /**< enable to scan AP whose SSID is hidden */
//     //     wifi_scan_type_t scan_type;  /**< scan type, active or passive */
//     //     wifi_scan_time_t scan_time;  /**< scan time per channel */
//     // } wifi_scan_config_t;
//     wifi_scan_config_t scan_config = {
//         .ssid = NULL,
//         .bssid = NULL,
//         .channel = 0,
//         .show_hidden = false,
//         .scan_type = WIFI_SCAN_TYPE_ACTIVE,
//     };
//     esp_wifi_scan_start(&scan_config, true);

//     uint16_t ap_num = 0;
//     // get total count of AP that esp32 can scan for
//     ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_num));
//     wifiLog.logD("AP total count: %3d", ap_num);

//     // initialize and allocate the memory for ap_records with the maximum count that you want esp32 to show
//     uint16_t max_aps = 20;
//     wifi_ap_record_t ap_records[max_aps];
//     memset(ap_records, 0, sizeof(ap_records));
//     // aps_count is set to tell esp32 the maximum APs can be shown, due to this variable will be changed,
//     // we need to define a new variable for this function
//     uint16_t aps_count = max_aps;
//     ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&aps_count, ap_records));
//     wifiLog.logI("AP count: %3d", aps_count);
//     printf("  %-30s  |  %-10s  |  %-10s  |  %-18s\n", "SSID", "Channel", "Strength", "MAC Address");
//     printf("-------------------------------------------------------------------------------\n");
//     for(int i = 0; i < aps_count; i++){
//         printf("  %-30s  |  %-10d  |  %-10d  |  %02X.%02X.%02X.%02X.%02X.%02X\n", ap_records[i].ssid, ap_records[i].primary, ap_records[i].rssi, ap_records[i].bssid[0], ap_records[i].bssid[1], ap_records[i].bssid[2], ap_records[i].bssid[3], ap_records[i].bssid[4], ap_records[i].bssid[5]);
//     }