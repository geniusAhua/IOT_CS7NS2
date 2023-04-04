#include <time.h>
#include <cstring>
#include "my_MQTT.h"
#include "my_event_loop.h"
#include "my_const.h"

/**
 * @brief The length of the outgoing publish records array used by the coreMQTT
 * library to track QoS > 0 packet ACKS for outgoing publishes.
 */
#define OUTGOING_PUBLISH_RECORD_LEN    ( 10U )

/**
 * @brief The length of the incoming publish records array used by the coreMQTT
 * library to track QoS > 0 packet ACKS for incoming publishes.
 */
#define INCOMING_PUBLISH_RECORD_LEN    ( 10U )

/**
 * @brief The MQTT metrics string expected by AWS IoT.
 */
#define METRICS_STRING                      "?SDK= FreeRTOS &Version=" tskKERNEL_VERSION_NUMBER "&Platform= ESP32-WROOM-32E &MQTTLib=" MQTT_LIBRARY_VERSION

/**
 * @brief The length of the MQTT metrics string expected by AWS IoT.
 */
#define METRICS_STRING_LENGTH               ( ( uint16_t ) ( sizeof( METRICS_STRING ) - 1 ) )

/**
 * @brief The base back-off delay (in milliseconds) to use for connection retry attempts.
 */
#define CONNECTION_RETRY_BACKOFF_BASE_MS         ( 500U )

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying connection to server.
 */
#define CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS    ( 5000U )

/**
 * @brief The maximum number of retries for connecting to server.
 */
#define CONNECTION_RETRY_MAX_ATTEMPTS            ( 5U )

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

extern const char client_cert_start[] asm("_binary_Client_crt_start");
extern const char client_cert_end[] asm("_binary_Client_crt_end");
extern const char client_key_start[] asm("_binary_Client_key_start");
extern const char client_key_end[] asm("_binary_Client_key_end");

extern const char root_cert_auth_start[]   asm("_binary_AmazonRootCA1_pem_start");
extern const char root_cert_auth_end[]   asm("_binary_AmazonRootCA1_pem_end");

/**
 * initialize the value of MQTT class
*/
std::map<const std::string, const MQTTSubscribeInfo_t> MQTT::map_subInfo;
std::map<std::string, std::function<void(const std::string topic, const std::string message)>> MQTT::map_subTopic_callback;
std::map<const uint16_t, MQTT::MQTT_subAckStatus_t> MQTT::map_subscribe_topicStatus;
std::map<const uint16_t, const std::string> MQTT::map_unsubscribe_topic;

MQTTStatus_t MQTT::MQTT_Status = MQTTSuccess;
MQTT::MQTT_conn_t MQTT::MQTT_info_conn = {
    .mqttContext = {0},
    .xNetworkContext = {0},
    .clientSessionPresent = false,
    .brokerSessionPresent = false
};

std::string MQTT::endpoint = AWS_IOT_ENDPOINT;
int MQTT::endpoint_length = AWS_IOT_ENDPOINT_LENGTH;
int MQTT::port = AWS_IOT_MQTT_PORT;

const uint16_t MQTT::BACK_OFF_BASE = CONNECTION_RETRY_BACKOFF_BASE_MS;
const uint16_t MQTT::MAX_BACK_OFF = CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS;
const uint32_t MQTT::MAX_ATTEMPTS = CONNECTION_RETRY_MAX_ATTEMPTS;
const int MQTT::MAX_OUTGOING_PUBLISHES = 5;
const int MQTT::INCOMING_PUBLISH_BUFFER_SIZE = AWS_IOT_INCOMING_PUBLISH_BUFFER_SIZE;
const int MQTT::OUTGOING_PUBLISH_BUFFER_SIZE = AWS_IOT_OUTGOING_PUBLISH_BUFFER_SIZE;

MyLog MQTT::MQTTLog(LOG_TAG_MQTT);
bool MQTT::isInit = false;
bool MQTT::isHaveConn = false;
bool MQTT::isHaveAliveTask = false;
std::string MQTT::MQTT_keep_alive_task_name = "MQTT_keep_alive";

uint32_t MQTT::process_timeout = MQTT_PROCESS_LOOP_TIMEOUT_MS;

//Mutex
SemaphoreHandle_t MQTT::mutex_MQTT_info_conn;
SemaphoreHandle_t MQTT::mutex_map_subscribe_topicStatus;
SemaphoreHandle_t MQTT::mutex_map_outgoingPublishPackets;
SemaphoreHandle_t MQTT::mutex_map_subTopic_callback;
SemaphoreHandle_t MQTT::mutex_map_subInfo;
StaticSemaphore_t MQTT::xTlsContextSemaphoreBuffer;

/**
 * @brief Array to keep the outgoing publish messages.
 * These stored outgoing publish messages are kept until a successful ack
 * is received.
 */
std::map<const uint16_t, MQTTPublishInfo_t> MQTT::map_outgoingPublishPackets;

/**
 * @brief Array to track the outgoing publish records for outgoing publishes
 * with QoS > 0.
 *
 * This is passed into #MQTT_InitStatefulQoS to allow for QoS > 0.
 *
 */
MQTTPubAckInfo_t MQTT::pOutgoingPublishRecords[ MQTT::OUTGOING_PUBLISH_BUFFER_SIZE];

/**
 * @brief Array to track the incoming publish records for incoming publishes
 * with QoS > 0.
 *
 * This is passed into #MQTT_InitStatefulQoS to allow for QoS > 0.
 *
 */
MQTTPubAckInfo_t MQTT::pIncomingPublishRecords[ MQTT::INCOMING_PUBLISH_BUFFER_SIZE ];

/**
 * @brief The network buffer must remain valid for the lifetime of the MQTT context.
 */
uint8_t MQTT::buffer[ NETWORK_BUFFER_SIZE ];

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Init the MQTT environment for connection and other functions. And register the handler for each MQTT event.
 */
void MQTT::Init(){
    if(!MQTT::isInit){
        MQTT::isInit = true;
        MQTT::MQTTLog.logI("Start initialize the MQTT moduel.");
        MQTT::mutex_MQTT_info_conn = xSemaphoreCreateMutex();
        MQTT::mutex_map_subscribe_topicStatus = xSemaphoreCreateMutex();
        MQTT::mutex_map_outgoingPublishPackets = xSemaphoreCreateMutex();
        MQTT::mutex_map_subInfo = xSemaphoreCreateMutex();
        MQTT::mutex_map_subTopic_callback = xSemaphoreCreateMutex();
        // MQTT::mutex_map_subTopic = xSemaphoreCreateMutex();
        if(MQTT::mutex_MQTT_info_conn == NULL ||
           MQTT::mutex_map_subscribe_topicStatus == NULL ||
           MQTT::mutex_map_subTopic_callback == NULL ||
           MQTT::mutex_map_subInfo == NULL ||
           MQTT::mutex_map_outgoingPublishPackets == NULL){

            MQTT::MQTTLog.logE("There is no more memory for mutex.");
        }

        struct timespec tp;

        /* Seed pseudo random number generator (provided by ISO C standard library) for
        * use by retry utils library when retrying failed network operations. */

        /* Get current time to seed pseudo random number generator. */
        ( void ) clock_gettime( CLOCK_REALTIME, &tp );
        /* Seed pseudo random number generator with nanoseconds. */
        srand( tp.tv_nsec );

        /* Initialize MQTT library. Initialization of the MQTT library needs to be
        * done only once in this demo. */
        if(xSemaphoreTake(MQTT::mutex_MQTT_info_conn, portMAX_DELAY) == pdTRUE){
            MQTT::initializeMqtt( &(MQTT::MQTT_info_conn.mqttContext), &(MQTT::MQTT_info_conn.xNetworkContext) );
            xSemaphoreGive(MQTT::mutex_MQTT_info_conn);
        }

        xEventGroupSetBits(MyEventLoop::smartBin_event_group(), MQTT_START);

        MyEventLoop::smartBin_handler_register(smartBin_event_t::WIFI_CONFIG_DONE,  MQTT::MQTT_wifi_done_handler,               NULL);
        MyEventLoop::smartBin_handler_register(smartBin_event_t::MQTT_SUB_BACK,     MQTT::MQTT_subBack_event_handler,           NULL);
        MyEventLoop::smartBin_handler_register(smartBin_event_t::MQTT_PUBLISH,      MQTT::MQTT_publish_event_handler,           NULL);
        MyEventLoop::smartBin_handler_register(smartBin_event_t::MQTT_PUBACK,       MQTT::MQTT_publish_back_event_handler,      NULL);
        MyEventLoop::smartBin_handler_register(smartBin_event_t::MQTT_UNSUBACK,     MQTT::MQTT_unsubscribe_back_event_handler,  NULL);
        MyEventLoop::smartBin_handler_register(smartBin_event_t::MQTT_UNKNOWN,      MQTT::MQTT_event_handler,                   NULL);
        
    }
}

/*-----------------------------------------------------------*/

void MQTT::MQTT_wifi_done_handler(void *handlerArg, esp_event_base_t base, int32_t id, void *event_data){
    MQTT::MQTTLog.logI("WIFI has been done. Strat MQTT now.");
    MQTT::isHaveConn = true;

    xEventGroupWaitBits(MyEventLoop::smartBin_event_group(), MQTT_START, pdFALSE, pdFALSE, portMAX_DELAY);
    
    xTaskCreate(MQTT::task_connect, "MQTT_connect", 4096, NULL, 1, NULL);
}

/*-----------------------------------------------------------*/

void MQTT::task_connect(void *parameters){
    if(MQTT::isInit){
        int returnStatus = EXIT_FAILURE;
        while(returnStatus == EXIT_FAILURE){
            if(xSemaphoreTake(MQTT::mutex_MQTT_info_conn, portMAX_DELAY) == pdTRUE){
                returnStatus = MQTT::connectToServerWithBackoffRetries( &(MQTT::MQTT_info_conn.xNetworkContext), &(MQTT::MQTT_info_conn.mqttContext), &(MQTT::MQTT_info_conn.clientSessionPresent), &(MQTT::MQTT_info_conn.brokerSessionPresent) );
                xSemaphoreGive(MQTT::mutex_MQTT_info_conn);
            }
            
            if(returnStatus == EXIT_FAILURE){
                MQTT::MQTTLog.logE("Failed to connect to MQTT broker %s.", MQTT::endpoint.c_str());
            }

            vTaskDelay(pdMS_TO_TICKS(5000));
        }

        /* -- Process all the subscribetion --
         * Here must create a new task to resubscribe. Because this function need to ensure
         * that all the subscribetion has been established successfully, it will wait for the SUBACK event.
         * However, SUBACK will be blocked because this MQTT::MQTT_wifi_done_handler haven't finished,
         * so we should create a new task to run it asynchronous*/
        xTaskCreate(MQTT::task_resubscribe_after_failed, "task_resubscribe_after_failed", 1024 * 4, NULL, 2, NULL);

        /* Update the flag to indicate that an MQTT client session is saved.
         * Once this flag is set, MQTT connect in the following iterations of
         * this demo will be attempted without requesting for a clean session. */
        if(xSemaphoreTake(MQTT::mutex_MQTT_info_conn, portMAX_DELAY) == pdTRUE){
            MQTT::MQTT_info_conn.clientSessionPresent = true;
            xSemaphoreGive(MQTT::mutex_MQTT_info_conn);
        }

        /* Check if session is present and if there are any outgoing publishes
         * that need to resend. This is only valid if the broker is
         * re-establishing a session which was already present. */
        bool brokerSessionPresent = false;
        if(xSemaphoreTake(MQTT::mutex_MQTT_info_conn, portMAX_DELAY) == pdTRUE){
            brokerSessionPresent = MQTT::MQTT_info_conn.brokerSessionPresent;
            xSemaphoreGive(MQTT::mutex_MQTT_info_conn);
        }
        if( brokerSessionPresent == true){
            MQTT::MQTTLog.logI("An MQTT session with broker is re-established. Resending unacked publishes.");
            
            /*Process all the resend of publish messages.*/
            MQTT::publish_resend();
        }
        else{
            MQTT::MQTTLog.logI("A clean MQTT connection is established. \tCleaning up all the stored outgoing publishes.\n");

            /* Clean up the outgoing publishes waiting for ack as this new
            * connection doesn't re-establish an existing session. */
            if(xSemaphoreTake(MQTT::mutex_map_outgoingPublishPackets, portMAX_DELAY) == pdTRUE){
                MQTT::map_outgoingPublishPackets.clear();
                xSemaphoreGive(MQTT::mutex_map_outgoingPublishPackets);
            }
        }

        if(!MQTT::isHaveAliveTask){
            xTaskCreate(MQTT::task_MQTT_keep_alive, "MQTT_keep_alive", 1024 * 3, NULL, 2, NULL);
        }
    }

    vTaskDelete(NULL);
}

/*-----------------------------------------------------------*/

void MQTT::task_MQTT_keep_alive(void *parameters){
    MQTT::MQTTLog.logI("Start MQTT keep alive task.");
    uint8_t attempt = 0;
    const uint8_t max_attempt = 10;
    MQTT::isHaveAliveTask = true;
    do {
        if(xSemaphoreTake(MQTT::mutex_MQTT_info_conn, portMAX_DELAY) == pdTRUE){
            MQTT::MQTT_Status = MQTT_ProcessLoop( &(MQTT::MQTT_info_conn.mqttContext) );
            xSemaphoreGive(MQTT::mutex_MQTT_info_conn);
        }
        // MQTT::MQTTLog.logI("Try to keep MQTT connection alive.");

        if((MQTT::MQTT_Status == MQTTSuccess) || (MQTT::MQTT_Status == MQTTNeedMoreBytes)){
            xEventGroupSetBits(MyEventLoop::smartBin_event_group(), MQTT_INIT);
            attempt = 0;
        }
        else{
            if(attempt < max_attempt){
                MQTT::MQTTLog.logE("Something wrong with MQTT connection. Here is the error code: %s, trying once more...", MQTT_Status_strerror(MQTT::MQTT_Status));
                xEventGroupClearBits(MyEventLoop::smartBin_event_group(), MQTT_INIT);
                attempt++;
            }
            else{
                MQTT::MQTTLog.logE("Faild to keep MQTT connection alive. Here is the error code: %s.", MQTT_Status_strerror(MQTT::MQTT_Status));
                MQTT::isHaveAliveTask = false;
                xTaskCreate(MQTT::task_connect, "MQTT_reconnect", 4096, NULL, 1, NULL);
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(500)); //10s
        }
        vTaskDelay(pdMS_TO_TICKS(100)); //100ms
    } while(MQTT::isHaveConn);
    MQTT::isHaveAliveTask = false;
    vTaskDelete(NULL);
}

/*-----------------------------------------------------------*/

void MQTT::MQTT_event_handler(void *handlerArg, esp_event_base_t base, int32_t id, void *event_data){
    if(base == MyEventLoop::SMARTBIN_BASE){
        switch(static_cast<smartBin_event_t>(id)){
            case smartBin_event_t::MQTT_UNKNOWN:
                MQTT::MQTTLog.logE("UnKnown packet!!!\n\n");
                break;

            default:
                break;
        }
    }
}

/**
 * @brief Function to update variable globalSubAckStatus with status
 * information from Subscribe ACK. Called by eventCallback after processing
 * incoming subscribe echo.
 *
 * @param[in] handlerArg    It's to provide some variables. Here it's NULL.
 * @param[in] base          It's to provide some variables. Here it's SMARTBIN_EVENT_BASE.
 * @param[in] id            It's to provide some variables. Here it's MQTT_SUB_BACK
 * @param[in] *event_data   It's provided by the event caller. Here it's MQTT::MQTT_callback_t type.
 * 
 * @return void
 */
void MQTT::MQTT_subBack_event_handler( void *handlerArg, esp_event_base_t base, int32_t id, void *event_data ){
    MQTT_callback_t* info_callback = (MQTT_callback_t*) event_data;
    MQTT::MQTTLog.logW("Get an MQTT subBack event. The packet type is %s, packetID: %d", info_callback->pPacketInfo->type == MQTT_PACKET_TYPE_SUBACK ? "SUBACK" : "UNKNOW", info_callback->packetIdentifier);
    // MQTTPacketInfo_t *pPacketInfo = info_callback->pPacketInfo;
    uint16_t packetId = info_callback->packetIdentifier;

    uint8_t *payload = NULL;
    size_t pSize = 0;
    MQTTStatus_t mqttStatus = MQTT_GetSubAckStatusCodes( info_callback->pPacketInfo, &payload, &pSize );

    // Since the pointers to the payload and payload size are not NULL, and
    // we use the packet info struct passed to the app callback (verified
    // to be valid by the library), this function must return success.
    assert( mqttStatus == MQTTSuccess );
    // The server must send a response code for each topic filter in the
    // original SUBSCRIBE packet.
    MQTT::MQTTLog.logI("Subscribed packet back. the num of pCode is %d", pSize);

    // set the status of the topic
    MQTTSubAckStatus_t status = (MQTTSubAckStatus_t) payload[0];
    if(xSemaphoreTake(MQTT::mutex_map_subscribe_topicStatus, portMAX_DELAY) == pdTRUE){
        MQTT::map_subscribe_topicStatus[packetId].status = status;
        xSemaphoreGive(MQTT::mutex_map_subscribe_topicStatus);
    }

    std::string status_str = "";

    for(int i = 0; i < pSize - 1; i++){
        status_str += std::to_string(payload[i]);
        status_str += " and ";
    }
    status_str += std::to_string(payload[pSize]);


    if(status != MQTTSubAckFailure){
        if(xSemaphoreTake(MQTT::mutex_map_subscribe_topicStatus, portMAX_DELAY) == pdTRUE){
            MQTT::MQTTLog.logI("Subscribed to the MQTT topic < %s > - with maximum QOS: %s", MQTT::map_subscribe_topicStatus[packetId].topic.c_str(), status_str.c_str());

            MQTT::map_subscribe_topicStatus.erase(packetId);

            xSemaphoreGive(MQTT::mutex_map_subscribe_topicStatus);
        }
    }
    
}

/*-----------------------------------------------------------*/

void MQTT::MQTT_publish_event_handler(void *handlerArg, esp_event_base_t base, int32_t eventId, void *event_data){
    if(!MQTT::isHaveConn) {
        MQTT::MQTTLog.logE("There is no connection to the Internet. From MQTT::MQTT_publish_event_handler");
        return;
    }

    MQTT_callback_t *info_callback = (MQTT_callback_t *) event_data;
    MQTTPublishInfo_t *pPublishInfo = info_callback->pDeserializedInfo->pPublishInfo;
    uint16_t packetIdentifier = info_callback->pDeserializedInfo->packetIdentifier;

    assert( pPublishInfo != NULL );

    std::string topicName(pPublishInfo->pTopicName, pPublishInfo->topicNameLength);
    std::string message((const char *)(pPublishInfo->pPayload), pPublishInfo->payloadLength);
    //topicName[pPublishInfo->topicNameLength] = '\0';
    //message[pPublishInfo->payloadLength] = '\0';

    MQTT::MQTTLog.logW("Get an MQTT publish event. The packet topic is < %s >, packetID: %d", topicName.c_str(), packetIdentifier);

    if(xSemaphoreTake(MQTT::mutex_map_subTopic_callback, portMAX_DELAY) == pdTRUE){
        if(map_subTopic_callback.contains(topicName)){
            MQTT::MQTTLog.logI("Incoming Publish Topic Name: < %.*s >. Incoming Publish QOS: %d\t"
                    "Incoming Publish message Packet Id is %u.\t"
                    "Incoming Publish Message : %.*s.\n\n",
                    topicName.length(),
                    topicName.c_str(),
                    pPublishInfo->qos,
                    packetIdentifier,
                    message.length(),
                    message.c_str()
                    );

            // call the callback function provided by the user
            MQTT_incoming_packet_t *task_info = new MQTT_incoming_packet_t{
                .topicName = topicName,
                .msg = message,
                .user_callback = MQTT::map_subTopic_callback[topicName]
            };

            xSemaphoreGive(MQTT::mutex_map_subTopic_callback);

            xTaskCreate(MQTT::task_run_user_callback, "Incoming callback", 1024 * 4, (void*)task_info, 1, NULL);
            // MQTT::map_subTopic_callback[topicName](topicName, message);

        }
        else{
            xSemaphoreGive(MQTT::mutex_map_subTopic_callback);
            MQTT::MQTTLog.logE("The Incoming Publish Topic Name: < %.*s > is not subscribed.\n\n",
                    topicName.length(),
                    topicName.c_str()
                    );
        }
    }

}

void MQTT::task_run_user_callback(void *parameters){
    MQTT_incoming_packet_t *task_info = (MQTT_incoming_packet_t *)parameters;

    task_info->user_callback(task_info->topicName, task_info->msg);

    delete task_info;
    vTaskDelete(NULL);
}

void MQTT::MQTT_publish_back_event_handler(void *handlerArg, esp_event_base_t base, int32_t eventId, void *event_data){
    MQTT_callback_t *info_callback = (MQTT_callback_t *) event_data;
    uint16_t packetId = info_callback->packetIdentifier;

    if(xSemaphoreTake(MQTT::mutex_map_outgoingPublishPackets, portMAX_DELAY) == pdTRUE){
        std::string topicName = MQTT::map_outgoingPublishPackets[packetId].pTopicName;
        topicName[MQTT::map_outgoingPublishPackets[packetId].topicNameLength];

        MQTT::MQTTLog.logI("Publish ack received for packet id %u, topic: < %s >\n", packetId, topicName.c_str());

        xSemaphoreGive(MQTT::mutex_map_outgoingPublishPackets);
    }
    
    /* Cleanup publish packet when a publish ack is received. */
    MQTT::MQTTLog.logI( "Cleaned up outgoing publish packet with packet id %u.\n\n", packetId);
    if(xSemaphoreTake(MQTT::mutex_map_outgoingPublishPackets, portMAX_DELAY) == pdTRUE){
        MQTT::map_outgoingPublishPackets.erase(packetId);
        xSemaphoreGive(MQTT::mutex_map_outgoingPublishPackets);
    }
    // MQTT::cleanupOutgoingPublishWithPacketID( packetId );
    return;
}

/*-----------------------------------------------------------*/

void MQTT::MQTT_unsubscribe_back_event_handler(void *handlerArg, esp_event_base_t base, int32_t eventId, void *event_data){
    MQTT_callback_t *info_callback = (MQTT_callback_t *) event_data;
    uint16_t packetId = info_callback->packetIdentifier;
    std::string topic = MQTT::map_unsubscribe_topic[packetId];

    MQTT::MQTTLog.logI("Unsubscribed from the topic < %s >.\n\n", topic.c_str());
    //after unsubscribed, remove all topics and callbacks from the map
    if(topic[0] != '|'){
        MQTT::map_subInfo.erase(topic);
        MQTT::map_subTopic_callback.erase(topic);
        MQTT::map_unsubscribe_topic.erase(packetId);
    }
    else{
        MQTT::map_subInfo.clear();
        MQTT::map_subTopic_callback.clear();
        MQTT::map_unsubscribe_topic.erase(packetId);
    }
}

/**
 * @brief Initializes the MQTT library.
 *
 * @param[in] pMqttContext MQTT context pointer.
 * @param[in] pNetworkContext The network context pointer.
 *
 * @return EXIT_SUCCESS if the MQTT library is initialized;
 * EXIT_FAILURE otherwise.
 */
int MQTT::initializeMqtt(MQTTContext_t * pMqttContext, NetworkContext_t * pNetworkContext){
    int returnStatus = EXIT_SUCCESS;
    MQTTStatus_t mqttStatus;
    MQTTFixedBuffer_t networkBuffer;
    TransportInterface_t transport = { 0 };

    assert( pMqttContext != NULL );
    assert( pNetworkContext != NULL );

    /* Fill in TransportInterface send and receive function pointers.
     * For this demo, TCP sockets are used to send and receive data
     * from network. Network context is SSL context for OpenSSL.*/
    transport.pNetworkContext = pNetworkContext;
    transport.send = espTlsTransportSend;
    transport.recv = espTlsTransportRecv;
    transport.writev = NULL;

    /* Fill the values for network buffer. */
    networkBuffer.pBuffer = buffer;
    networkBuffer.size = NETWORK_BUFFER_SIZE;

    /* Initialize MQTT library. */
    mqttStatus = MQTT_Init( pMqttContext,
                            &transport,
                            Clock_GetTimeMs,
                            &MQTT::coreMQTT_EventCallback,
                            &networkBuffer );

    if( mqttStatus != MQTTSuccess )
    {
        returnStatus = EXIT_FAILURE;
        LogError( ( "MQTT_Init failed: Status = %s.", MQTT_Status_strerror( mqttStatus ) ) );
    }
    else
    {
        mqttStatus = MQTT_InitStatefulQoS( pMqttContext,
                                           pOutgoingPublishRecords,
                                           OUTGOING_PUBLISH_RECORD_LEN,
                                           MQTT::pIncomingPublishRecords,
                                           INCOMING_PUBLISH_RECORD_LEN );

        if( mqttStatus != MQTTSuccess )
        {
            returnStatus = EXIT_FAILURE;
            LogError( ( "MQTT_InitStatefulQoS failed: Status = %s.", MQTT_Status_strerror( mqttStatus ) ) );
        }
    }

    return returnStatus;
}

/**
 * @brief The application callback function for getting the incoming publish
 * and incoming acks reported from MQTT library.
 *
 * @param[in] pMqttContext MQTT context pointer.
 * @param[in] pPacketInfo Packet Info pointer for the incoming packet.
 * @param[in] pDeserializedInfo Deserialized information from the incoming packet.
 */
void MQTT::coreMQTT_EventCallback( MQTTContext_t * pMqttContext, MQTTPacketInfo_t * pPacketInfo, MQTTDeserializedInfo_t * pDeserializedInfo ){
    uint16_t packetIdentifier;

    assert( pMqttContext != NULL );
    assert( pPacketInfo != NULL );
    assert( pDeserializedInfo != NULL );

    /* Suppress unused parameter warning when asserts are disabled in build. */
    ( void ) pMqttContext;

    packetIdentifier = pDeserializedInfo->packetIdentifier;

    MQTT_callback_t *info_callback = new MQTT::MQTT_callback_t{
        .pPacketInfo = new MQTTPacketInfo_t(*pPacketInfo),
        .packetIdentifier = * new uint16_t(packetIdentifier),
        .pPacketInfo_length = * new size_t(pPacketInfo->headerLength + 1 + pPacketInfo->remainingLength + 1),
    };

    /* Handle incoming publish. The lower 4 bits of the publish packet
     * type is used for the dup, QoS, and retain flags. Hence masking
     * out the lower bits to check if the packet is publish. */
    if( ( info_callback->pPacketInfo->type & 0xF0U ) == MQTT_PACKET_TYPE_PUBLISH )
    {
        info_callback->pDeserializedInfo = pDeserializedInfo;
        MyEventLoop::post_event_to(smartBin_event_t::MQTT_PUBLISH,
                                   (void*)info_callback, 
                                   sizeof(info_callback)
                                   + info_callback->pDeserializedInfo->pPublishInfo->payloadLength + 1
                                   + info_callback->pDeserializedInfo->pPublishInfo->topicNameLength + 1
                                   + info_callback->pPacketInfo_length,
                                   portMAX_DELAY);
        // assert( pDeserializedInfo->pPublishInfo != NULL );
        // /* Handle incoming publish. */
        // MQTT::handleIncomingPublish( pDeserializedInfo->pPublishInfo, packetIdentifier );
    }
    else
    {
        /* Handle other packets. */
        switch( info_callback->pPacketInfo->type )
        {
            case MQTT_PACKET_TYPE_SUBACK:
                MyEventLoop::post_event_to(smartBin_event_t::MQTT_SUB_BACK,
                                           (void *)info_callback,
                                           sizeof(info_callback)
                                           + info_callback->pPacketInfo_length,
                                           portMAX_DELAY);
                break;

            case MQTT_PACKET_TYPE_UNSUBACK:
                /**
                 * @todo
                */
                MyEventLoop::post_event_to(smartBin_event_t::MQTT_UNSUBACK,
                                           (void*)info_callback,
                                           sizeof(info_callback)
                                           + info_callback->pPacketInfo_length,
                                           portMAX_DELAY );
                break;

            case MQTT_PACKET_TYPE_PINGRESP:

                /* Nothing to be done from application as library handles
                 * PINGRESP. */
                LogWarn( ( "PINGRESP should not be handled by the application "
                           "callback when using MQTT_ProcessLoop.\n\n" ) );
                break;

            case MQTT_PACKET_TYPE_PUBACK:
                MyEventLoop::post_event_to(smartBin_event_t::MQTT_PUBACK,
                                           (void*)info_callback,
                                           sizeof(info_callback)
                                           + info_callback->pPacketInfo_length,
                                           portMAX_DELAY);
                break;

            /* Any other packet type is invalid. */
            default:
                MyEventLoop::post_event_to(smartBin_event_t::MQTT_UNKNOWN,
                                           NULL,
                                           0,
                                           portMAX_DELAY);
                break;
        }
    }

    delete info_callback;
}

/**
 * @brief Connect to MQTT broker with reconnection retries.
 *
 * If connection fails, retry is attempted after a timeout.
 * Timeout value will exponentially increase until maximum
 * timeout value is reached or the number of attempts are exhausted.
 *
 * @param[out] pNetworkContext The output parameter to return the created network context.
 * @param[out] pMqttContext The output to return the created MQTT context.
 * @param[in,out] pClientSessionPresent Pointer to flag indicating if an
 * MQTT session is present in the client.
 * @param[out] pBrokerSessionPresent Session was already present in the broker or not.
 * Session present response is obtained from the CONNACK from broker.
 *
 * @return EXIT_FAILURE on failure; EXIT_SUCCESS on successful connection.
 */
int MQTT::connectToServerWithBackoffRetries( NetworkContext_t * pNetworkContext, MQTTContext_t * pMqttContext, bool * pClientSessionPresent, bool * pBrokerSessionPresent ){
    int returnStatus = EXIT_SUCCESS;
    BackoffAlgorithmStatus_t backoffAlgStatus = BackoffAlgorithmSuccess;
    TlsTransportStatus_t tlsStatus = TLS_TRANSPORT_SUCCESS;
    BackoffAlgorithmContext_t reconnectParams;
    bool createCleanSession;

    pNetworkContext->pcHostname = MQTT::endpoint.c_str();
    pNetworkContext->xPort = MQTT::port;
    pNetworkContext->pxTls = NULL;
    pNetworkContext->xTlsContextSemaphore = xSemaphoreCreateMutexStatic(&xTlsContextSemaphoreBuffer);

    pNetworkContext->disableSni = 0;
    uint16_t nextRetryBackOff;

    /* Initialize credentials for establishing TLS session. */
    pNetworkContext->pcServerRootCA = root_cert_auth_start;
    pNetworkContext->pcServerRootCASize = root_cert_auth_end - root_cert_auth_start;
    pNetworkContext->pcClientCert = client_cert_start;
    pNetworkContext->pcClientCertSize = client_cert_end - client_cert_start;
    pNetworkContext->pcClientKey = client_key_start;
    pNetworkContext->pcClientKeySize = client_key_end - client_key_start;

    /* AWS IoT requires devices to send the Server Name Indication (SNI)
     * extension to the Transport Layer Security (TLS) protocol and provide
     * the complete endpoint address in the host_name field. Details about
     * SNI for AWS IoT can be found in the link below.
     * https://docs.aws.amazon.com/iot/latest/developerguide/transport-security.html */
    /* Initialize reconnect attempts and interval */
    BackoffAlgorithm_InitializeParams( &reconnectParams,
                                       CONNECTION_RETRY_BACKOFF_BASE_MS,
                                       CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS,
                                       CONNECTION_RETRY_MAX_ATTEMPTS );

    /* Attempt to connect to MQTT broker. If connection fails, retry after
     * a timeout. Timeout value will exponentially increase until maximum
     * attempts are reached.
     */
    do
    {
        /* Establish a TLS session with the MQTT broker. This example connects
         * to the MQTT broker as specified in MQTT::endpoint and MQTT::port
         * at the demo config header. */
        MQTT::MQTTLog.logI("Establishing a TLS session to %.*s:%d.",
                   MQTT::endpoint_length,
                   MQTT::endpoint.c_str(),
                   MQTT::port );
        tlsStatus = xTlsConnect ( pNetworkContext );

        if( tlsStatus == TLS_TRANSPORT_SUCCESS )
        {
            /* A clean MQTT session needs to be created, if there is no session saved
             * in this MQTT client. */
            createCleanSession = ( *pClientSessionPresent == true ) ? false : true;

            /* Sends an MQTT Connect packet using the established TLS session,
             * then waits for connection acknowledgment (CONNACK) packet. */
            returnStatus = MQTT::establishMqttSession( pMqttContext, createCleanSession, pBrokerSessionPresent );

            if( returnStatus == EXIT_FAILURE )
            {
                /* End TLS session, then close TCP connection. */
                // cleanupESPSecureMgrCerts( pNetworkContext );
                ( void ) xTlsDisconnect( pNetworkContext );
            }
        }

        if( returnStatus == EXIT_FAILURE )
        {
            /* Generate a random number and get back-off value (in milliseconds) for the next connection retry. */
            backoffAlgStatus = BackoffAlgorithm_GetNextBackoff( &reconnectParams, MQTT::generateRandomNumber(), &nextRetryBackOff );

            if( backoffAlgStatus == BackoffAlgorithmRetriesExhausted )
            {
                MQTT::MQTTLog.logE( "Connection to the broker failed, all attempts exhausted." );
                returnStatus = EXIT_FAILURE;
            }
            else if( backoffAlgStatus == BackoffAlgorithmSuccess )
            {
                MQTT::MQTTLog.logW( "Connection to the broker failed. Retrying connection "
                           "after %hu ms backoff.",
                           ( unsigned short ) nextRetryBackOff );
                Clock_SleepMs( nextRetryBackOff );
            }
        }
    } while( ( returnStatus == EXIT_FAILURE ) && ( backoffAlgStatus == BackoffAlgorithmSuccess ) );

    return returnStatus;
}

/*-----------------------------------------------------------*/

void MQTT::Publish(std::string topic, std::string message){
    xEventGroupWaitBits(MyEventLoop::smartBin_event_group(), MQTT_INIT, pdFALSE, pdFALSE, portMAX_DELAY);
    uint16_t current_id = 0;

    assert( &(MQTT::MQTT_info_conn.mqttContext) != NULL );

    // // /* Get the next free index for the outgoing publish. All QoS1 outgoing
    // //  * publishes are stored until a PUBACK is received. These messages are
    // //  * stored for supporting a resend if a network connection is broken before
    // //  * receiving a PUBACK. */
    // index = getNextFreeIndexForOutgoingPublishes();

    // MQTT::MQTTLog.logI("Get index: %d", index);

    MQTTStatus_t mqttStatus = MQTTSuccess;
    if(xSemaphoreTake(MQTT::mutex_map_outgoingPublishPackets, portMAX_DELAY) == pdTRUE){
        if( MQTT::map_outgoingPublishPackets.size() < MQTT::MAX_OUTGOING_PUBLISHES){
            xSemaphoreGive(MQTT::mutex_map_outgoingPublishPackets);
            MQTTPublishInfo_t pubInfo = {
                .qos = AWS_IOT_MQTT_QOS,
                .retain = false,
                .pTopicName = topic.data(),
                .topicNameLength = (uint16_t) topic.length(),
                .pPayload = message.data(),
                .payloadLength = message.length()
            };

            /* Get a new packet id. */
            uint16_t packetId = 0;
            if(xSemaphoreTake(MQTT::mutex_MQTT_info_conn, portMAX_DELAY) == pdTRUE){
                packetId = MQTT_GetPacketId( &(MQTT::MQTT_info_conn.mqttContext) );
                
                mqttStatus = MQTT_Publish( &(MQTT::MQTT_info_conn.mqttContext), &pubInfo, packetId );
                xSemaphoreGive(MQTT::mutex_MQTT_info_conn);
                
                current_id = packetId;
                if(xSemaphoreTake(MQTT::mutex_map_outgoingPublishPackets, portMAX_DELAY) == pdTRUE){

                    MQTT::map_outgoingPublishPackets.insert(std::pair<uint16_t, MQTTPublishInfo_t>(packetId, pubInfo));
                    
                    xSemaphoreGive(MQTT::mutex_map_outgoingPublishPackets);
                }


            }
        }
        else {
            xSemaphoreGive(MQTT::mutex_map_outgoingPublishPackets);
            mqttStatus = MQTTSendFailed;
        }
    }

    if( mqttStatus != MQTTSuccess ) {
        MQTT::MQTTLog.logE("Failed to publish message. From MQTT::Publish, with error code %s\n\n", MQTT_Status_strerror(mqttStatus));

        // MQTT::cleanupOutgoingPublishAt( current_id );
        if(xSemaphoreTake(MQTT::mutex_map_outgoingPublishPackets, portMAX_DELAY) == pdTRUE){
            MQTT::map_outgoingPublishPackets.erase(current_id);
            xSemaphoreGive(MQTT::mutex_map_outgoingPublishPackets);
        }
    }
    else{
        MQTT::MQTTLog.logI("Publish message to topic < %s > to broker with packet ID: %u\n\n", topic.c_str(), current_id);

    }

    return;
}

/*-----------------------------------------------------------*/

void MQTT::publish_resend(){
    MQTTStatus_t mqttStatus = MQTTSuccess;
    uint8_t index = 0U;
    MQTTStateCursor_t cursor = MQTT_STATE_CURSOR_INITIALIZER;
    uint16_t packetIdToResend = MQTT_PACKET_ID_INVALID;
    bool foundPacketId = false;

    assert( &MQTT::MQTT_info_conn.mqttContext != NULL );

    /* MQTT_PublishToResend() provides a packet ID of the next PUBLISH packet
     * that should be resent. In accordance with the MQTT v3.1.1 spec,
     * MQTT_PublishToResend() preserves the ordering of when the original
     * PUBLISH packets were sent. The outgoingPublishPackets array is searched
     * through for the associated packet ID. If the application requires
     * increased efficiency in the look up of the packet ID, then a hashmap of
     * packetId key and PublishPacket_t values may be used instead. */
    if(xSemaphoreTake(MQTT::mutex_MQTT_info_conn, portMAX_DELAY) == pdTRUE){
        packetIdToResend = MQTT_PublishToResend( &MQTT::MQTT_info_conn.mqttContext, &cursor );
        xSemaphoreGive(MQTT::mutex_MQTT_info_conn);
    }

    while( packetIdToResend != MQTT_PACKET_ID_INVALID ){
        foundPacketId = MQTT::map_outgoingPublishPackets.contains(packetIdToResend);

        if(foundPacketId){
            MQTT::MQTTLog.logI("Sending duplicate PUBLISH with packet id %u.", packetIdToResend);

            if(xSemaphoreTake(MQTT::mutex_MQTT_info_conn, portMAX_DELAY) == pdTRUE){
                
                mqttStatus = MQTT_Publish( &MQTT::MQTT_info_conn.mqttContext,
                                            &MQTT::map_outgoingPublishPackets[packetIdToResend],
                                            packetIdToResend );


                if( mqttStatus != MQTTSuccess ){
                    MQTT::MQTTLog.logE("Sending duplicate PUBLISH for packet id %u failed with status %s.", packetIdToResend, MQTT_Status_strerror( mqttStatus ));
                    xSemaphoreGive(MQTT::mutex_MQTT_info_conn);
                    break;
                }
                else{
                    MQTT::MQTTLog.logI("Sent duplicate PUBLISH successfully for packet id %u.\n\n", packetIdToResend);
                }

                /* Get the next packetID to be resent. */
                packetIdToResend = MQTT_PublishToResend( &MQTT::MQTT_info_conn.mqttContext, &cursor );
                xSemaphoreGive(MQTT::mutex_MQTT_info_conn);
            }
        }
        else{
            MQTT::MQTTLog.logE("Packet id %u requires resend, but was not found in outgoingPublishPackets.", packetIdToResend);
            break;
        }
    }

    return;
}

/*-----------------------------------------------------------*/

void MQTT::Subscribe(std::string topic, std::function<void(const std::string topic, const std::string message)>callback){
    xEventGroupWaitBits(MyEventLoop::smartBin_event_group(), MQTT_INIT, pdFALSE, pdFALSE, portMAX_DELAY);

    assert( &(MQTT::MQTT_info_conn.mqttContext) != NULL );
    
    if(xSemaphoreTake(MQTT::mutex_map_subInfo, portMAX_DELAY) == pdTRUE){
        if(MQTT::map_subInfo.count(topic) == 0 && MQTT::map_subInfo.size() < AWS_IOT_MQTT_MAX_SUBSCRIPTIONS){

            MQTTStatus_t mqttStatus;
            uint16_t packetId = 0;

            MQTTSubscribeInfo_t pSubscription = {
                .qos = AWS_IOT_MQTT_QOS,
                .pTopicFilter = topic.c_str(),
                .topicFilterLength = (uint16_t)topic.length()
            };

            MQTT::map_subInfo.insert(std::pair<const std::string, const MQTTSubscribeInfo_t>(topic, pSubscription));
            xSemaphoreGive(MQTT::mutex_map_subInfo);

            if(xSemaphoreTake(MQTT::mutex_MQTT_info_conn, portMAX_DELAY) == pdTRUE){
                packetId = MQTT_GetPacketId( &(MQTT::MQTT_info_conn.mqttContext) );

                
                MQTT::MQTTLog.logI("Subscribing to the MQTT topic < %s >.", topic.c_str());
                mqttStatus = MQTT_Subscribe( &(MQTT::MQTT_info_conn.mqttContext),
                                                        &pSubscription,
                                                        1,
                                                        packetId );

                xSemaphoreGive(MQTT::mutex_MQTT_info_conn);

                MQTT::MQTT_subAckStatus_t status = {
                    .topic = topic,
                    .status = MQTTSubAckFailure
                };

                if(xSemaphoreTake(MQTT::mutex_map_subscribe_topicStatus, portMAX_DELAY) == pdTRUE){
                    MQTT::map_subscribe_topicStatus.insert(std::pair<const uint16_t, MQTT::MQTT_subAckStatus_t>(packetId, status));
                    xSemaphoreGive(MQTT::mutex_map_subscribe_topicStatus);
                }

                if( mqttStatus != MQTTSuccess ){
                    MQTT::MQTTLog.logE( "Failed to SUBSCRIBE to MQTT topic < %s > with error = %s\n\n", topic.c_str(), MQTT_Status_strerror( mqttStatus ) );
                }
                else{
                    MQTT::MQTTLog.logI( "SUBSCRIBE sent for the MQTT topic: < %s > to broker.", topic.c_str() );

                    if(xSemaphoreTake(MQTT::mutex_map_subTopic_callback, portMAX_DELAY) == pdTRUE){
                        MQTT::map_subTopic_callback.insert(std::pair<std::string, std::function<void(const std::string topic, const std::string message)>>(topic, callback));
                        xSemaphoreGive(MQTT::mutex_map_subTopic_callback);
                    }
                    MQTT::task_sub_check_timeout(packetId);

                }
                
                MQTT::MQTTLog.logI("Subscribe to < %s > done.\n\n", topic.c_str());
            }
            
        }
        else{
            xSemaphoreGive(MQTT::mutex_map_subInfo);
            MQTT::MQTTLog.logE("The topic has been subscribed or Maximum number of subscriptions reached. You can just subscribe to %d topics!!\n\n", AWS_IOT_MQTT_MAX_SUBSCRIPTIONS);
        }
    }
    

    return;

}

/*-----------------------------------------------------------*/

void MQTT::UnsubscribeFromTopic(std::string topic){
    xEventGroupWaitBits(MyEventLoop::smartBin_event_group(), MQTT_INIT, pdFALSE, pdFALSE, portMAX_DELAY);

    assert( &(MQTT::MQTT_info_conn.mqttContext) != NULL );

    if(MQTT::map_subTopic_callback.count(topic) > 0){

        MQTTSubscribeInfo_t pSubscription = MQTT::map_subInfo[topic];
        uint16_t packetId = 0;
        
        if(xSemaphoreTake(MQTT::mutex_MQTT_info_conn, portMAX_DELAY) == pdTRUE){
            packetId = MQTT_GetPacketId( &(MQTT::MQTT_info_conn.mqttContext) );

            if(MQTT::map_subInfo.size() > 0){
                MQTT::MQTTLog.logI("Unsubscribing to the MQTT topic < %s >.", topic.c_str());
                MQTTStatus_t mqttStatus = MQTT_Unsubscribe( &(MQTT::MQTT_info_conn.mqttContext),
                                                        &pSubscription,
                                                        1,
                                                        packetId );
                if( mqttStatus != MQTTSuccess ){
                    MQTT::MQTTLog.logE( "Failed to UNSUBSCRIBE to MQTT topic < %s > with error = %s\n\n", topic.c_str(), MQTT_Status_strerror( mqttStatus ) );
                }
                else{
                    MQTT::MQTTLog.logI( "UNSUBSCRIBE sent for the MQTT topic: < %s > to broker.\n", topic.c_str() );
                    MQTT::map_unsubscribe_topic.insert(std::pair<const uint16_t, const std::string>(packetId, topic));
                }
            }

            xSemaphoreGive(MQTT::mutex_MQTT_info_conn);
        }
    }
    else{
        MQTT::MQTTLog.logE("The topic has not been subscribed.\n\n");
    }

    return;
}

/*-----------------------------------------------------------*/

void MQTT::UnsubscribeAllTopic(){

    xEventGroupWaitBits(MyEventLoop::smartBin_event_group(), MQTT_INIT, pdFALSE, pdFALSE, portMAX_DELAY);
    assert( &(MQTT::MQTT_info_conn.mqttContext) != NULL );

    if(MQTT::map_subInfo.size() > 0){
        MQTTSubscribeInfo_t pSubscription[MQTT::map_subInfo.size()];
        uint16_t packetId = 0;
        std::string unsub_topic_str = "|";

        int i = 0;
        for(auto it = map_subInfo.begin(); it != map_subInfo.end(); it++){
            pSubscription[i] = it->second;
            unsub_topic_str += it->first + "|";
            i++;
        }

        if(xSemaphoreTake(MQTT::mutex_MQTT_info_conn, portMAX_DELAY) == pdTRUE){
            packetId = MQTT_GetPacketId( &(MQTT::MQTT_info_conn.mqttContext) );
            

            MQTT::MQTTLog.logI("Unsubscribing to all MQTT topics.");
            MQTTStatus_t mqttStatus = MQTT_Unsubscribe( &(MQTT::MQTT_info_conn.mqttContext),
                                                    pSubscription,
                                                    map_subInfo.size(),
                                                    packetId );
            
            if( mqttStatus != MQTTSuccess ){
                MQTT::MQTTLog.logE( "Failed to UNSUBSCRIBE to all MQTT topics with error = %s\n\n", MQTT_Status_strerror( mqttStatus ) );
            }
            else{
                MQTT::MQTTLog.logI( "UNSUBSCRIBE sent for all MQTT topics to broker. Topics: < %s >", unsub_topic_str.c_str() );
                MQTT::map_unsubscribe_topic.insert(std::pair<const uint16_t, const std::string>(packetId, unsub_topic_str));
            }

            xSemaphoreGive(MQTT::mutex_MQTT_info_conn);
        }
    }
    else{
        MQTT::MQTTLog.logE("No topic has been subscribed.\n\n");
    }

    return;
}

/*-----------------------------------------------------------*/

void MQTT::task_sub_check_timeout(uint16_t packetId){
    
    // uint16_t *packetId = (uint16_t*) parameters;
    int returnStatus = EXIT_FAILURE;

    returnStatus = MQTT::sub_timeout_check(packetId);

    bool targetExist = false;
    if(xSemaphoreTake(MQTT::mutex_map_subscribe_topicStatus, portMAX_DELAY) == pdTRUE){
        targetExist = MQTT::map_subscribe_topicStatus.contains(packetId);
        xSemaphoreGive(MQTT::mutex_map_subscribe_topicStatus);
    }

    if((returnStatus == EXIT_SUCCESS) && (targetExist)){
        /* If server rejected the subscription request, attempt to resubscribe to topic.
         * Attempts are made according to the exponential backoff retry strategy
         * implemented in retryUtils. */
        if(xSemaphoreTake(MQTT::mutex_map_subscribe_topicStatus, portMAX_DELAY) == pdTRUE){
            MQTT::MQTTLog.logW("Server rejected subscription request. Attempting to resubscribe to topic < %s >\n\n", MQTT::map_subscribe_topicStatus[packetId].topic.c_str());
            xSemaphoreGive(MQTT::mutex_map_subscribe_topicStatus);
        }

        returnStatus = MQTT::resubscribe(packetId);
    }

    if(returnStatus == EXIT_FAILURE){
        MQTT::MQTTLog.logE("ERROR: Failed to subscribe to topic < %s >\n\n", MQTT::map_subscribe_topicStatus[packetId].topic.c_str());
    }

    
    return;
}

/*-----------------------------------------------------------*/

int MQTT::sub_timeout_check(uint16_t packetId){
    uint32_t ulMQTTProcessLoopEntryTime;
    uint32_t ulMQTTProcessLoopTimeoutTime;
    uint32_t ulCurrentTime = 0;

    int returnStatus = EXIT_FAILURE;

    bool targetExist = false;

    MQTT::MQTTLog.logI("check timeout for packetId: %d", packetId);

    if(xSemaphoreTake(MQTT::mutex_MQTT_info_conn, portMAX_DELAY) == pdTRUE){
        ulCurrentTime = MQTT::MQTT_info_conn.mqttContext.getTime();
        xSemaphoreGive(MQTT::mutex_MQTT_info_conn);
    }
    ulMQTTProcessLoopEntryTime = ulCurrentTime;
    ulMQTTProcessLoopTimeoutTime = ulCurrentTime + MQTT::process_timeout;

    if(xSemaphoreTake(MQTT::mutex_map_subscribe_topicStatus, portMAX_DELAY) == pdTRUE){
        targetExist = MQTT::map_subscribe_topicStatus.contains(packetId);
        xSemaphoreGive(MQTT::mutex_map_subscribe_topicStatus);
    }

    while( (targetExist) && (ulCurrentTime < ulMQTTProcessLoopTimeoutTime) && (MQTT::MQTT_Status == MQTTSuccess || MQTT::MQTT_Status == MQTTNeedMoreBytes)){

        if(xSemaphoreTake(MQTT::mutex_map_subscribe_topicStatus, portMAX_DELAY) == pdTRUE){
            targetExist = MQTT::map_subscribe_topicStatus.contains(packetId);
            xSemaphoreGive(MQTT::mutex_map_subscribe_topicStatus);
        }
        if(xSemaphoreTake(MQTT::mutex_MQTT_info_conn, portMAX_DELAY) == pdTRUE){
            ulCurrentTime = MQTT::MQTT_info_conn.mqttContext.getTime();
            xSemaphoreGive(MQTT::mutex_MQTT_info_conn);
        }
    }

    if(((MQTT::MQTT_Status != MQTTSuccess) && (MQTT::MQTT_Status != MQTTNeedMoreBytes)) || (targetExist)){
        MQTT::MQTTLog.logE("MQTT_ProcessLoop run by < %s > failed to receive ACK packet: Expected ACK Packet ID = %d, LoopDuration = %" PRIu32 " returned with status = %s", 
        MQTT::map_subscribe_topicStatus[packetId].topic.c_str(),
        packetId,
        (ulCurrentTime - ulMQTTProcessLoopEntryTime),
        MQTT_Status_strerror( MQTT::MQTT_Status ) );
    }
    else returnStatus = EXIT_SUCCESS;

    return returnStatus;
}

/*-----------------------------------------------------------*/

void MQTT::task_resubscribe_after_failed(void *parameters){
    xEventGroupWaitBits(MyEventLoop::smartBin_event_group(), MQTT_INIT, pdFALSE, pdFALSE, portMAX_DELAY);
    
    if(MQTT::map_subInfo.size() > 0){
        MQTTSubscribeInfo_t *pSubscription = new MQTTSubscribeInfo_t[MQTT::map_subInfo.size()];;
        MQTTStatus_t mqttStatus = MQTTSuccess;
        std::string topic_str = "|";
        int i = 0;

        for(auto it = MQTT::map_subInfo.begin(); it != MQTT::map_subInfo.end(); it++){

            topic_str += it->first + "|";
            /* Here we must create a new instance for pSubscription, and can't just use MQTT::map_subInfo[topic]
             * directly. Because the pointer of MQTT::map_subInfo[topic] may can not be used when out of the 
             * definition area, it will cause some wrong thing and you can't get any error or wrong information
             * in this situation.
             **/
            pSubscription[i++] = *new MQTTSubscribeInfo_t{
                .qos = AWS_IOT_MQTT_QOS,
                .pTopicFilter = it->first.c_str(),
                .topicFilterLength = (uint16_t)it->first.length()
            };
        }
        

        if(xSemaphoreTake(MQTT::mutex_MQTT_info_conn, portMAX_DELAY) == pdTRUE){
            uint16_t packetId = MQTT_GetPacketId( &(MQTT::MQTT_info_conn.mqttContext) );
            
            MQTT::MQTTLog.logI("Resubscribing to the MQTT topic < %s > with packet ID: %d", topic_str.c_str(), packetId);
            mqttStatus = MQTT_Subscribe( &(MQTT::MQTT_info_conn.mqttContext),
                                                    &pSubscription[0],
                                                    map_subInfo.size(),
                                                    packetId );

            xSemaphoreGive(MQTT::mutex_MQTT_info_conn);
            

            if(xSemaphoreTake(MQTT::mutex_map_subscribe_topicStatus, portMAX_DELAY) == pdTRUE){
                MQTT::map_subscribe_topicStatus.insert(std::pair<const uint16_t, MQTT::MQTT_subAckStatus_t>(packetId, MQTT::MQTT_subAckStatus_t{.topic = topic_str, .status = MQTTSubAckFailure}));
                xSemaphoreGive(MQTT::mutex_map_subscribe_topicStatus);
            }

            if( mqttStatus != MQTTSuccess ){
                MQTT::MQTTLog.logE( "Failed to SUBSCRIBE to MQTT topic < %s > with error = %s", topic_str.c_str(), MQTT_Status_strerror( mqttStatus ) );
            }
            else{
                MQTT::MQTTLog.logI( "SUBSCRIBE sent for the MQTT topic: < %s > to broker with packet ID: %d.\n", topic_str.c_str(), packetId );

                MQTT::task_sub_check_timeout(packetId);
            }
            
            MQTT::MQTTLog.logI("Resubscribe to < %s > done.\n\n", topic_str.c_str());
        }

        delete[] pSubscription;
        
    }
    
    vTaskDelete(NULL);
}

/**
 * @brief   It'll be called when client failed to subscribe some topics.\n 
 * @param  identifier       My Param doc
 * @return int 
 */
int MQTT::resubscribe(const uint16_t identifier){

    xEventGroupWaitBits(MyEventLoop::smartBin_event_group(), MQTT_INIT, pdFALSE, pdFALSE, portMAX_DELAY);
    MQTTStatus_t mqttStatus = MQTTSuccess;
    BackoffAlgorithmStatus_t backoffAlgStatus = BackoffAlgorithmSuccess;
    BackoffAlgorithmContext_t retryParams;
    uint16_t nextRetryBackoff = 0u;

    int returnStatus = EXIT_SUCCESS;

    assert( &MQTT::MQTT_info_conn.mqttContext != NULL );

    /* Initialize retry attempts and interval. */
    BackoffAlgorithm_InitializeParams( &retryParams, MQTT::BACK_OFF_BASE, MQTT::MAX_BACK_OFF, MQTT::MAX_ATTEMPTS );

    do{
        /* Send SUBSCRIBE packet.
         * Note: resuing the value specified in globalSubscribePacketIdentifier is acceptable here
         * because this function is entered only after the receipt of a SUBACK, at which point
         * its associated packet id is free to use. */
        std::string re_topic = MQTT::map_subscribe_topicStatus[identifier].topic;
        if(xSemaphoreTake(MQTT::mutex_MQTT_info_conn, portMAX_DELAY) == pdTRUE){
            mqttStatus = MQTT_Subscribe( &(MQTT::MQTT_info_conn.mqttContext), &(MQTT::map_subInfo[re_topic]), 1, identifier );
            xSemaphoreGive(MQTT::mutex_MQTT_info_conn);
        }

        if( mqttStatus != MQTTSuccess ){
            MQTT::MQTTLog.logE("Failed to resend SUBSCRIBE packet to broker with error = %s.", MQTT_Status_strerror( mqttStatus ) );

            break;
        }

        MQTT::MQTTLog.logI( "SUBSCRIBE sent for topic < %s > to broker.\n\n", re_topic.c_str() );

        /* Process incoming packet */
        returnStatus = MQTT::sub_timeout_check(identifier);

        if( returnStatus == EXIT_FAILURE ) break;

        /* Check if recent subscription request has been rejected. 
         * MQTT::map_subscribe_topicStatus[identifier].status is updated
         * in eventCallback to reflect the status of the SUBACK sent by the broker. It represents
         * either the QoS level granted by the server upon subscription, or acknowledgement of
         * server rejection of the subscription request. */
        if( MQTT::map_subscribe_topicStatus[identifier].status == MQTTSubAckFailure ){
            /* Generate a random number and get back-off value (in milliseconds) for the next retry attempt. */
            backoffAlgStatus = BackoffAlgorithm_GetNextBackoff( &retryParams, MQTT::generateRandomNumber(), &nextRetryBackoff );

            if( backoffAlgStatus == BackoffAlgorithmRetriesExhausted ){
                MQTT::MQTTLog.logE( "Subscription to topic failed, all attempts exhausted." );
                returnStatus = EXIT_FAILURE;
            }
            else if( backoffAlgStatus == BackoffAlgorithmSuccess ){
                MQTT::MQTTLog.logW( "Retry attempt %hu out of maximum retry attempts %hu.",
                                    ( unsigned short ) ( retryParams.attemptsDone + 1 ),
                                    ( unsigned short ) MQTT::MAX_ATTEMPTS );

                MQTT::MQTTLog.logW( "Server rejected subscription request. Retrying connection after %hu ms backoff.", ( unsigned short ) nextRetryBackoff );

                Clock_SleepMs( nextRetryBackoff );
            }
        }
    } while( ( MQTT::map_subscribe_topicStatus[identifier].status == MQTTSubAckFailure ) && ( backoffAlgStatus == BackoffAlgorithmSuccess ) );

    return returnStatus;
}

/**
 * <pre>
 * @brief Sends an MQTT CONNECT packet over the already connected TCP socket.
 * @param[in]  pMqttContext     MQTT context pointer.
 * @param[in]  createCleanSessionMy Creates a new MQTT session if true.
 * If false, tries to establish the existing session if there was session
 * already present in broker.
 * @param[out]  pSessionPresent  Session was already present in the broker or not.
 * Session present response is obtained from the CONNACK from broker.
 * @return int EXIT_SUCCESS if an MQTT session is established;
 * EXIT_FAILURE otherwise.
 * </pre>
 */
int MQTT::establishMqttSession( MQTTContext_t * pMqttContext, bool createCleanSession, bool * pSessionPresent ){
    int returnStatus = EXIT_SUCCESS;
    MQTTStatus_t mqttStatus;
    MQTTConnectInfo_t connectInfo = { 0 };

    assert( pMqttContext != NULL );
    assert( pSessionPresent != NULL );

    /* Establish MQTT session by sending a CONNECT packet. */

    /* If #createCleanSession is true, start with a clean session
     * i.e. direct the MQTT broker to discard any previous session data.
     * If #createCleanSession is false, directs the broker to attempt to
     * reestablish a session which was already present. */
    connectInfo.cleanSession = createCleanSession;

    /* The client identifier is used to uniquely identify this MQTT client to
     * the MQTT broker. In a production device the identifier can be something
     * unique, such as a device serial number. */
    connectInfo.pClientIdentifier = AWS_IOT_CLIENT_ID;
    connectInfo.clientIdentifierLength = AWS_IOT_CLIENT_ID_LENGTH;

    /* The maximum time interval in seconds which is allowed to elapse
     * between two Control Packets.
     * It is the responsibility of the Client to ensure that the interval between
     * Control Packets being sent does not exceed the this Keep Alive value. In the
     * absence of sending any other Control Packets, the Client MUST send a
     * PINGREQ Packet. */
    connectInfo.keepAliveSeconds = AWS_IOT_KEEP_ALIVE_INTERVAL;

    
    connectInfo.pUserName = METRICS_STRING;
    connectInfo.userNameLength = METRICS_STRING_LENGTH;
    /* Password for authentication is not used. */
    connectInfo.pPassword = NULL;
    connectInfo.passwordLength = 0U;

    /* Send MQTT CONNECT packet to broker. */
    mqttStatus = MQTT_Connect( pMqttContext, &connectInfo, NULL, AWS_IOT_CONNACK_RECV_TIMEOUT_MS, pSessionPresent );

    if( mqttStatus != MQTTSuccess )
    {
        returnStatus = EXIT_FAILURE;
        MQTT::MQTTLog.logE( "Connection with MQTT broker failed with status %s.",
                    MQTT_Status_strerror( mqttStatus ) );
    }
    else
    {
        MQTT::MQTTLog.logI( "MQTT connection successfully established with broker.\n\n" );
    }

    return returnStatus;
}

/**
 * @brief The random number generator to use for exponential backoff with
 * jitter retry logic.
 *
 * @return { uint32_t } The generated random number.
 */
uint32_t MQTT::generateRandomNumber(){
    return( (uint32_t)rand() );
}