#ifndef __MY_MQTT_H__
#define __MY_MQTT_H__

#include <stdio.h>
#include <map>
#include <vector>
#include <functional>
/* MQTT API headers. */
#include "core_mqtt.h"
#include "core_mqtt_state.h"
/* OpenSSL sockets transport implementation. */
#include "network_transport.h"
/*Include backoff algorithm header for retry logic.*/
#include "backoff_algorithm.h"
/* Clock for timer. */
#include "clock.h"
#include "esp_secure_cert_read.h"

#include "esp_event.h"
#include "my_log.h"


class MQTT{
    public:
    static void Init();
    static void Subscribe(std::string topic, std::function<void(const std::string topic, const std::string message)>callback);
    static void Publish(std::string topic, std::string message);
    static void UnsubscribeFromTopic(std::string topic);
    static void UnsubscribeAllTopic();

    private:
    typedef struct{
        const MQTTPacketInfo_t            *pPacketInfo;
        const uint16_t                    packetIdentifier;
        const MQTTDeserializedInfo_t      *pDeserializedInfo;
        const size_t                      pPacketInfo_length = 0;
    } MQTT_callback_t;

    typedef struct{
        MQTTContext_t           mqttContext;
        NetworkContext_t        xNetworkContext;
        bool                    clientSessionPresent;
        bool                    brokerSessionPresent;
    } MQTT_conn_t;

    typedef struct{
        const std::string topic;
        MQTTSubAckStatus_t status;
    } MQTT_subAckStatus_t;

    typedef struct{
        const std::string topicName;
        const std::string msg;
        const std::function<void(const std::string topic, const std::string message)> user_callback;
    } MQTT_incoming_packet_t;

    //save all the Subscribed topic
    static std::map<const std::string, const MQTTSubscribeInfo_t> map_subInfo;
    //save all the callback
    static std::map<std::string, std::function<void(const std::string topic, const std::string message)>> map_subTopic_callback;
    //cacahe the MQTT subscribe message
    static std::map<const uint16_t, MQTT_subAckStatus_t> map_subscribe_topicStatus;
    // save packetId of unsubscribe
    static std::map<const uint16_t, const std::string> map_unsubscribe_topic;
    static std::map<const uint16_t, MQTTPublishInfo_t> map_outgoingPublishPackets;

    //info of the connection for MQTT Context
    static MQTT_conn_t MQTT_info_conn;
    static MQTTStatus_t MQTT_Status;

    //info of the connection for MQTT
    static std::string endpoint;
    static int endpoint_length;
    static int port;

    static const uint16_t BACK_OFF_BASE;
    static const uint16_t MAX_BACK_OFF;
    static const uint32_t MAX_ATTEMPTS;
    static const int      MAX_OUTGOING_PUBLISHES;
    static const int      INCOMING_PUBLISH_BUFFER_SIZE;
    static const int      OUTGOING_PUBLISH_BUFFER_SIZE;

    //other info
    static MyLog MQTTLog;
    static bool isInit;
    static bool isHaveConn;
    static bool isHaveAliveTask;
    static std::string MQTT_keep_alive_task_name;

    static uint32_t process_timeout;

    //Mutex
    static SemaphoreHandle_t mutex_MQTT_info_conn;
    static SemaphoreHandle_t mutex_map_subscribe_topicStatus;
    static SemaphoreHandle_t mutex_map_outgoingPublishPackets;
    static SemaphoreHandle_t mutex_map_subTopic_callback;
    static SemaphoreHandle_t mutex_map_subInfo;
    static StaticSemaphore_t xTlsContextSemaphoreBuffer;


    
    static MQTTPubAckInfo_t pOutgoingPublishRecords[];
    static MQTTPubAckInfo_t pIncomingPublishRecords[];
    static uint8_t buffer[];

    static void MQTT_event_handler( void *handlerArg, esp_event_base_t base, int32_t id, void *event_data );
    static void MQTT_wifi_done_handler( void *handlerArg, esp_event_base_t base, int32_t id, void *event_data );
    static void MQTT_publish_event_handler( void *handlerArg, esp_event_base_t base, int32_t id, void *event_data );
    static void MQTT_publish_back_event_handler( void *handlerArg, esp_event_base_t base, int32_t id, void *event_data );
    static void MQTT_unsubscribe_back_event_handler( void *handlerArg, esp_event_base_t base, int32_t id, void *event_data );

    static void task_sub_check_timeout(uint16_t packetId);

    static void task_run_user_callback(void *parameters);
    static void task_unsub_check_timeout(void *parameters);
    static void task_connect(void *parameters);
    // static void task_user_callback(void *parameters);
    // static void task_pub_check_timeout(void *parameters);
    static void task_MQTT_keep_alive(void *parameters);
    static void task_resubscribe_after_failed(void *parameters);

    static int sub_timeout_check(uint16_t packetId);
    static int resubscribe(const uint16_t identifier);
    static void publish_resend();
    static int disconnect();
    
    static int initializeMqtt(MQTTContext_t * pMqttContext, NetworkContext_t * pNetworkContext);

    static int connectToServerWithBackoffRetries( NetworkContext_t * pNetworkContext, MQTTContext_t * pMqttContext, bool * pClientSessionPresent, bool * pBrokerSessionPresent );

    static int establishMqttSession( MQTTContext_t * pMqttContext, bool createCleanSession, bool * pSessionPresent );
    
    
    static uint32_t generateRandomNumber();
    
    static void coreMQTT_EventCallback( MQTTContext_t * pMqttContext, MQTTPacketInfo_t * pPacketInfo, MQTTDeserializedInfo_t * pDeserializedInfo );

    static void MQTT_subBack_event_handler( void *handlerArg, esp_event_base_t base, int32_t id, void *event_data );
    
};//MQTT

#endif