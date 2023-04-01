#include "my_sensors.h"
#include "my_const.h"
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
const int GPS::RX_BUFFER = 144;
uint32_t GPS::interval = 1000 / portTICK_PERIOD_MS; // 1s
MyLog GPS::GPSLog(LOG_TAG_SENSORS_GPS);


void IRAM_ATTR GPS::uart_rx_intr_handler(void *arg){
    uart_event_t event;
    uint8_t *data = NULL;
    
}


GPS::GPS(int gpio_rx, int gpio_tx) : parameters{.mutex_data = xSemaphoreCreateMutex()}
{

    this->gpio_tx = gpio_tx == UART_PIN_NO_CHANGE ? GPIO_NUM_1 : gpio_tx;
    this->gpio_rx = gpio_rx == UART_PIN_NO_CHANGE ? GPIO_NUM_3 : gpio_rx;

    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_2, GPS::RX_BUFFER * 1, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_2, &GPS::uart_config);
    uart_set_pin(UART_NUM_2, gpio_tx, gpio_rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_enable_rx_intr(UART_NUM_2);

    this->parameters.data = new uint8_t[GPS::RX_BUFFER * 1 + 1];
    memset(this->parameters.data, '\0', GPS::RX_BUFFER * 1 + 1);

    GPS::GPSLog.logI("Start a task to get the data from GPS sensor.");
    xTaskCreate(&(GPS::task_GPS), TASK_NAME_GPS, 1024 * 4, (void *)&(this->parameters), 1, NULL);
}

void GPS::task_GPS(void *_parameters)
{
    GPS_task_t *parameters = (GPS_task_t *)_parameters;
    while (1)
    {
        if (xSemaphoreTake((*parameters).mutex_data, portMAX_DELAY) == pdTRUE)
        {
            const int rxBytes = uart_read_bytes(UART_NUM_2, parameters->data, GPS::RX_BUFFER * 1, GPS::interval);
            if (rxBytes > 0)
            {
                parameters->data[rxBytes] = 0;
                // GPS::GPSLog.logI("Get data: \n%s", parameters->data);
            }

            xSemaphoreGive(parameters->mutex_data);
        }
    }
}

std::string GPS::get_location()
{
    std::string position = "";
    if (xSemaphoreTake(this->parameters.mutex_data, portMAX_DELAY) == pdTRUE)
    {
        position = (char *)this->parameters.data;

        xSemaphoreGive(this->parameters.mutex_data);
    }
    return position;
}

/**
 * Ultrasonic implementation
 * 
*/
MyLog Ultrasonic::UltrasonicLog(LOG_TAG_SENSORS_ULTRASONIC);

Ultrasonic::Ultrasonic(gpio_num_t trigger = PIN_ULTRASONIC_TRIGGER, gpio_num_t echo = PIN_ULTRASONIC_ECHO) : parameters({.dist = new float, .mutex_dist = xSemaphoreCreateMutex(), .sensor = {.trigger_pin = trigger, .echo_pin = echo}})
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
    while(1) 
    {

        if (xSemaphoreTake(parameters->mutex_dist, portMAX_DELAY) == pdTRUE) 
        {
            
            err = ultrasonic_measure(&parameters->sensor, MAX_DISTANCE_CM, parameters->dist);
            
            xSemaphoreGive(parameters->mutex_dist);
        }
        
        switch (err)
        {
            case ESP_ERR_ULTRASONIC_PING:
                Ultrasonic::UltrasonicLog.logI("Cannot ping (device is in invalid state)\n");
                break;
            case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
                Ultrasonic::UltrasonicLog.logI("Ping timeout (no device found)\n");
                break;
            case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
                Ultrasonic::UltrasonicLog.logI("Echo timeout (i.e. distance too big)\n");                    
                break;
            default:
                Ultrasonic::UltrasonicLog.logI(esp_err_to_name(err));
                // printf("%s\n", esp_err_to_name(res));
        }

        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

std::string Ultrasonic::get_distance()
{
    // std::string dist = "";
    std::string dist = "";
    if (xSemaphoreTake(this->parameters.mutex_dist, portMAX_DELAY) == pdTRUE)
    {
        dist = std::to_string(*(this->parameters.dist));

        xSemaphoreGive(this->parameters.mutex_dist);
    }
    return dist;
}

MyLog Humiture::HumitureLog(LOG_TAG_HUMITURE);

Humiture::Humiture(gpio_num_t pin = PIN_HUMITURE): parameters({.pin = pin, .humidity = new int16_t, .temperature = new int16_t, .sensor_type = DHT_TYPE_DHT11, .mutex_humiture = xSemaphoreCreateMutex()})
{
    xTaskCreate(&(Humiture::task_Humiture), TASK_NAME_HUMITURE, 1024 * 4, (void*)&this->parameters, 1, NULL);
}

void Humiture::task_Humiture(void *_parameters)
{
    Humiture_task_t *parameters = (Humiture_task_t*) _parameters;
    esp_err_t err = ESP_FAIL;
    while(1)
    {
        if (xSemaphoreTake(parameters->mutex_humiture, portMAX_DELAY) == pdTRUE)
        {
            err = dht_read_data(parameters->sensor_type, parameters->pin, parameters->humidity, parameters->temperature);

            xSemaphoreGive(parameters->mutex_humiture);
        }
        
        Humiture::HumitureLog.logI(esp_err_to_name(err));
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

// Servo implementation
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
    esp_err_t err_turn = iot_servo_write_angle(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 186);
    vTaskDelay(pdMS_TO_TICKS(2000));
    Servo::ServoLog.logI(esp_err_to_name(err_turn));
    esp_err_t err_back = iot_servo_write_angle(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    vTaskDelay(pdMS_TO_TICKS(2000));
    Servo::ServoLog.logI(esp_err_to_name(err_back));
}