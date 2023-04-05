#include "my_sensors.h"
#include "my_const.h"
#include "nmea_parser.h"
#include "../lib/ultrasonic/ultrasonic.h"



/**
 * @class GPS
 * @note this a sensor class for GPS, just remember GPIO_rx pin need to connect to the tx pin on GPS sensors.
 */

const uart_config_t GPS::uart_config = {
    .baud_rate = 9600,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_APB,
};
const int GPS::RX_BUFFER_SIZE = 1024;
uint32_t GPS::interval = 1000 / portTICK_PERIOD_MS; // 1s
MyLog GPS::GPSLog(LOG_TAG_SENSORS_GPS);

const uint8_t GPS::TIME_ZONE = (+1);
const uint16_t GPS::YEAR_BASE = 2000;   //data in GPS starts from 2000


GPS::GPS(uint32_t gpio_rx) : parameters{.mutex_data = xSemaphoreCreateMutex()}
{

    //this->gpio_tx = gpio_tx == UART_PIN_NO_CHANGE ? GPIO_NUM_1 : gpio_tx;
    //this->gpio_rx = gpio_rx == UART_PIN_NO_CHANGE ? GPIO_NUM_3 : gpio_rx;

    this->GPS_config = {
        .uart = {
            .uart_port = UART_NUM_2, 
            .rx_pin = gpio_rx, 
            .baud_rate = 9600, 
            .data_bits = UART_DATA_8_BITS, 
            .parity = UART_PARITY_DISABLE, 
            .stop_bits = UART_STOP_BITS_1, 
            .event_queue_size = 16
        }
    };
    this->nmea_handler = nmea_parser_init(&this->GPS_config);

    this->parameters.GPS_info = *new GPS_info_t;

    nmea_parser_add_handler(this->nmea_handler, GPS::task_GPS, (void*)&this->parameters);
    
    //this->GPS_config = NMEA_PARSER_CONFIG_DEFAULT();
    //this->nmea_handler = nmea_parser_init(&this->GPS_config);
    

    // // We won't use a buffer for sending data.
    // uart_param_config(UART_NUM_2, &GPS::uart_config);
    // uart_set_pin(UART_NUM_2, gpio_tx, gpio_rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // uart_driver_install(UART_NUM_2, GPS::RX_BUFFER_SIZE * 2, 0, 0, NULL, 0);
    // uart_enable_rx_intr(UART_NUM_2);

    // this->parameters.data = new uint8_t[GPS::RX_BUFFER_SIZE * 1 + 1];
    // memset(this->parameters.data, '\0', GPS::RX_BUFFER_SIZE * 1 + 1);

    // GPS::GPSLog.logI("Start a task to get the data from GPS sensor.");
    // xTaskCreate(&(GPS::task_GPS), TASK_NAME_GPS, 1024 * 4, (void *)&(this->parameters), 1, NULL);
}

void GPS::add_handler(void(*handler)(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)){
    nmea_parser_add_handler(this->nmea_handler, handler, NULL);
}

void GPS::task_GPS(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
     GPS_task_t *parameters = (GPS_task_t *)event_handler_arg;

    gps_t *GPS_info = NULL;
    switch(event_id){
        case GPS_UPDATE:
            GPS_info = (gps_t *)event_data;
            printf("%d/%d/%d %d:%d:%d => \r\n\t\t\t\t\t\tlatitude   = %.09f°N\n",
                 GPS_info->date.year + YEAR_BASE, GPS_info->date.month, GPS_info->date.day,
                 GPS_info->tim.hour + TIME_ZONE, GPS_info->tim.minute, GPS_info->tim.second,
                 GPS_info->latitude);

             if (xSemaphoreTake(parameters->mutex_data, portMAX_DELAY) == pdTRUE){
                 parameters->GPS_info = * new GPS_info_t{
                     .altitude = GPS_info->altitude,
                     .latitude = GPS_info->latitude,
                     .longitude = GPS_info->longitude,
                     .year = GPS_info->date.year + GPS::YEAR_BASE,
                     .month = GPS_info->date.month,
                     .day = GPS_info->date.day,
                     .hour = GPS_info->tim.hour + GPS::TIME_ZONE,
                     .minute = GPS_info->tim.minute,
                     .second = GPS_info->tim.second
                 };

                 xSemaphoreGive(parameters->mutex_data);
             }
            break;
        
        default:
            break;
    }
}

const GPS_info_t GPS::get_location()
{
    GPS_info_t gps_info;
    if (xSemaphoreTake(this->parameters.mutex_data, portMAX_DELAY) == pdTRUE)
    {
        gps_info = this->parameters.GPS_info;

        xSemaphoreGive(this->parameters.mutex_data);
    }

    return gps_info;
}

/**
 * Ultrasonic implementation
 * 
*/
MyLog Ultrasonic::UltrasonicLog(LOG_TAG_SENSORS_ULTRASONIC);

Ultrasonic::Ultrasonic(gpio_num_t trigger = PIN_ULTRASONIC_TRIGGER, gpio_num_t echo = PIN_ULTRASONIC_ECHO) : parameters({.dist = new uint32_t, .mutex_dist = xSemaphoreCreateMutex(), .sensor = {.trigger_pin = trigger, .echo_pin = echo}})
{
    this->trigger = trigger;
    this->echo = echo;
    xTaskCreate(&(Ultrasonic::task_Ultrasonic), TASK_NAME_ULTRASONIC, 1024 * 4, (void*)&this->parameters, 1, NULL);
}

void Ultrasonic::task_Ultrasonic(void *_parameters) 
{
    Ultrasonic_task_t *parameters = (Ultrasonic_task_t*)_parameters;
    esp_err_t err = ESP_FAIL;
    ESP_ERROR_CHECK(ultrasonic_init(&parameters->sensor));
    uint32_t current_tick;
    uint32_t end_tick;
    uint32_t distance;
    uint32_t total_value_distance;
    uint32_t total_count;
    while(1) 
    {
        current_tick = xTaskGetTickCount();
        end_tick = current_tick + pdMS_TO_TICKS(DISTANCE_DETECT_INTERVAL);
        distance = 0;
        total_value_distance = 0;
        total_count = 0;
        
        while(current_tick < end_tick){
            err = ultrasonic_measure_cm(&parameters->sensor, MAX_DISTANCE_CM, &distance);
            total_value_distance += distance;
            ++total_count;
            current_tick = xTaskGetTickCount();
            vTaskDelay(pdMS_TO_TICKS(7));
        }
        //printf("count: %ld\n", total_count);
        if (xSemaphoreTake(parameters->mutex_dist, portMAX_DELAY) == pdTRUE) 
        {
            
            *parameters->dist = total_value_distance / total_count;
            xSemaphoreGive(parameters->mutex_dist);
        }
        
        switch (err)
        {
            case ESP_ERR_ULTRASONIC_PING:
                //Ultrasonic::UltrasonicLog.logE("Cannot ping (device is in invalid state)\n");
                break;
            case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
                Ultrasonic::UltrasonicLog.logE("Ping timeout (no device found)\n");
                break;
            case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
                Ultrasonic::UltrasonicLog.logE("Echo timeout (i.e. distance too big)\n");                    
                break;
            default:
                //Ultrasonic::UltrasonicLog.logI(esp_err_to_name(err));
                // printf("%s\n", esp_err_to_name(res));
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

uint32_t Ultrasonic::get_distance()
{
    // std::string dist = "";
    uint32_t dist = 0;
    if (xSemaphoreTake(this->parameters.mutex_dist, portMAX_DELAY) == pdTRUE)
    {
        dist = *(this->parameters.dist);

        xSemaphoreGive(this->parameters.mutex_dist);
    }
    return dist;
}



/**
 * @class Humiture 
 * 
 */
MyLog Humiture::HumitureLog(LOG_TAG_HUMITURE);

Humiture::Humiture(gpio_num_t pin = PIN_HUMITURE): parameters({.pin = pin, .humidity = new int16_t, .temperature = new int16_t, .sensor_type = DHT_TYPE_DHT11, .mutex_humiture = xSemaphoreCreateMutex()})
{
    xTaskCreate(&(Humiture::task_Humiture), TASK_NAME_HUMITURE, 1024 * 4, (void*)&this->parameters, 1, NULL);
}

void Humiture::task_Humiture(void *_parameters)
{
    Humiture_task_t *parameters = (Humiture_task_t*) _parameters;
    int16_t temperature = 240;
    int16_t humidity = 500;
    esp_err_t err = ESP_FAIL;
    while(1)
    {
        err = dht_read_data(parameters->sensor_type, parameters->pin, &humidity, &temperature);
        Humiture::HumitureLog.logI("%s, ----> humidity: %d, temperature: %d.\n", esp_err_to_name(err), humidity, temperature);

        if (xSemaphoreTake(parameters->mutex_humiture, portMAX_DELAY) == pdTRUE)
        {
            *parameters->humidity = humidity;
            *parameters->temperature = temperature;

            xSemaphoreGive(parameters->mutex_humiture);
        }
        
        
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

HumiAndTemp Humiture::getHumiTemp()
{
    HumiAndTemp *humiture = new HumiAndTemp();
    if (xSemaphoreTake(this->parameters.mutex_humiture, portMAX_DELAY) == pdTRUE)
    {
        delete humiture;
        humiture = new HumiAndTemp(*this->parameters.humidity, *this->parameters.temperature);

        xSemaphoreGive(this->parameters.mutex_humiture);
    }

    
    return *humiture;
}

HumiAndTemp::HumiAndTemp(int16_t humidity, int16_t temperature)
{
    this->humidity = humidity;
    this->temperature = temperature;
}

int16_t HumiAndTemp::getHumidity()
{
    return this->humidity;
}

int16_t HumiAndTemp::getTemperature()
{
    return this->temperature;
}


/**
 * @class Servo
 *  
 */
MyLog Servo::ServoLog(LOG_TAG_SERVO);

Servo::Servo(gpio_num_t pin = PIN_SERVO, ledc_channel_t chan = LEDC_CHANNEL_0) 
{
    this->pin = pin;
    this->servo_cfg = {
        .max_angle = SERVO_MAX_ANGLE,
        .min_width_us = SERVO_MIN_WIDTH,
        .max_width_us = SERVO_MAX_WIDTH,
        .freq = SERVO_FREQ,
        .timer_number = LEDC_TIMER_0,
        .channels = {
            .servo_pin = {
                pin,
            },
            .ch = {
                chan,
            }
        },
        .channel_number = 1,
    };
    iot_servo_init(LEDC_LOW_SPEED_MODE, &servo_cfg);
}

void Servo::task_Servo() 
{
    esp_err_t err_turn = iot_servo_write_angle(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 180);
    Servo::ServoLog.logI(esp_err_to_name(err_turn));
    vTaskDelay(pdMS_TO_TICKS(2000));
    
    esp_err_t err_back = iot_servo_write_angle(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    Servo::ServoLog.logI(esp_err_to_name(err_back));
}

/**
 * @class Led
 * 
 */
Led::Led(gpio_num_t pin, gpio_mode_t mode)
{
    this->pin = pin;
    this->mode = mode;
    this->s_led_state = 0;
    gpio_reset_pin(pin);
    gpio_set_direction(pin, mode);
}

void Led::toggle_led()
{   
    this->s_led_state = !this->s_led_state;
    gpio_set_level(this->pin, this->s_led_state);
}

void Led::led_lightUp(){
    this->s_led_state = 1;
    gpio_set_level(this->pin, this->s_led_state);
}

void Led::led_lightDown(){
    this->s_led_state = 0;
    gpio_set_level(this->pin, this->s_led_state);
}