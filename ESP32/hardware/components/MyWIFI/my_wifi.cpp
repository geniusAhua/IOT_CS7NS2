#include <stdio.h>
#include <cstring>
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "my_log.h"
#include "my_wifi.h"
#include "my_const.h"

static bool isInit = false;
MyLog wifiLog(LOG_TAG_WIFI);

void WiFi::init_auto(){
    wifiLog.logI("0. 初始化NVS存储.");
    nvs_flash_init();

    wifiLog.logI("1. WIFI 初始化阶段.");
    //1.1 initialize;
    esp_netif_init();

    //1.2 creat an event loop;
    esp_event_loop_create_default();

    //1.3 o create default network interface instance;
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_config);
    
    //1.4 creat your app task;

    wifiLog.logI("2. WIFI 初始化阶段.");
    //2.1 set wifi mode;
    esp_wifi_set_mode(TRASH_BIN_WIFI_MODE);

    wifiLog.logI("3. WIFI 启动阶段.");
    //3.1 start wifi;
    esp_wifi_start();

    wifiLog.logI("4. WIFI 扫描.");
    //4.1 set country code;
    wifi_country_t country_config = {
        .cc = "IE",
        .schan = 1,
        .nchan = 13,
        .policy = WIFI_COUNTRY_POLICY_AUTO,
    };
    esp_wifi_set_country(&country_config);

    //4.2 start scaning;
    // typedef struct {
    //     uint8_t *ssid;               /**< SSID of AP */
    //     uint8_t *bssid;              /**< MAC address of AP */
    //     uint8_t channel;             /**< channel, scan the specific channel */
    //     bool show_hidden;            /**< enable to scan AP whose SSID is hidden */
    //     wifi_scan_type_t scan_type;  /**< scan type, active or passive */
    //     wifi_scan_time_t scan_time;  /**< scan time per channel */
    // } wifi_scan_config_t;
    wifi_scan_config_t scan_config = {
        .show_hidden = true
    };
    esp_wifi_scan_start(&scan_config, true);

    uint16_t ap_num = 0;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_num));
    wifiLog.logI("AP count: %d", ap_num);

    uint16_t max_aps = 20;
    wifi_ap_record_t ap_records[max_aps];
    memset(ap_records, 0, sizeof(ap_records));
}