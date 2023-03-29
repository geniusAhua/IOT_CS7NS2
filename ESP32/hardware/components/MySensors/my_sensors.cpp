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

GPS::GPS(int gpio_rx, int gpio_tx) : parameters{.mutex_data = xSemaphoreCreateMutex()}
{

    this->gpio_tx = gpio_tx == UART_PIN_NO_CHANGE ? GPIO_NUM_1 : gpio_tx;
    this->gpio_rx = gpio_rx == UART_PIN_NO_CHANGE ? GPIO_NUM_3 : gpio_rx;

    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_2, GPS::RX_BUFFER * 1, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_2, &GPS::uart_config);
    uart_set_pin(UART_NUM_2, gpio_tx, gpio_rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

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

// Ultrasonic implementation
MyLog Ultrasonic::UltrasonicLog(LOG_TAG_SENSORS_GPS);

Ultrasonic::Ultrasonic(gpio_num_t trigger, gpio_num_t echo) : parameters{.mutex_dist = xSemaphoreCreateMutex()}
{
    this->trigger = PIN_ULTRASONIC_TRIGGER;
    this->echo = PIN_ULTRASONIC_ECHO;
    sensor = {
        .trigger_pin = trigger,
        .echo_pin = echo
    };
    ultrasonic_init(&sensor);
    xTaskCreate(&(Ultrasonic::task_Ultrasonic), TASK_NAME_ULTRASONIC, configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);
}

void Ultrasonic::task_Ultrasonic(void *_parameters) 
{
    Ultrasonic_task_t *parameters = (Ultrasonic_task_t*)_parameters;
    while(1) {
        if (xSemaphoreTake((*parameters).mutex_dist, portMAX_DELAY) == pdTRUE) {
            
            esp_err_t res = ultrasonic_measure(&sensor, MAX_DISTANCE_CM, (*parameters).dist);
            switch (res)
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
                    Ultrasonic::UltrasonicLog.logI(esp_err_to_name(res));
                    // printf("%s\n", esp_err_to_name(res));
            }
            xSemaphoreGive((*parameters).mutex_dist);
        }
    }
}

std::string Ultrasonic::get_distance()
{
    std::string dist = "";
    if (xSemaphoreTake(this->parameters.mutex_dist, portMAX_DELAY) == pdTRUE)
    {
        dist = std::to_string((*this->parameters.dist));

        xSemaphoreGive(this->parameters.mutex_dist);
    }
    return dist;
}