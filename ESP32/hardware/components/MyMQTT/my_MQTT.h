#ifndef __MY_MQTT_H__
#define __MY_MQTT_H__

#include <stdio.h>
#include <map>
#include <vector>

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
    static void Init(int argc, char ** argv);
    static void Subscribe(std::string topic, void (*callback)(const std::string topic, const std::string message));
    static void Publish(std::string topic, std::string message);
    static void UnsubscribeFromTopic(std::string topic);
    static void UnsubscribeAllTopic();
    static int disconnectMQTTSession();

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

    //save all the Subscribed topic
    static std::map<const std::string, const MQTTSubscribeInfo_t> map_subInfo;
    //save all the callback
    static std::map<std::string, void(*)(const std::string topic, const std::string message)> map_subTopic_callback;
    //cacahe the MQTT subscribe message
    static std::map<const uint16_t, MQTT_subAckStatus_t> map_subscribe_topicStatus;
    // save packetId of unsubscribe
    static std::map<const uint16_t, const std::string> map_unsubscribe_topic;

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
    static StaticSemaphore_t xTlsContextSemaphoreBuffer;

    static void MQTT_event_handler( void *handlerArg, esp_event_base_t base, int32_t id, void *event_data );
    static void MQTT_wifi_done_handler( void *handlerArg, esp_event_base_t base, int32_t id, void *event_data );
    static void MQTT_wifi_disconnect_handler( void *handlerArg, esp_event_base_t base, int32_t id, void *event_data );
    static void MQTT_publish_event_handler( void *handlerArg, esp_event_base_t base, int32_t id, void *event_data );
    static void MQTT_publish_back_event_handler( void *handlerArg, esp_event_base_t base, int32_t id, void *event_data );
    static void MQTT_unsubscribe_back_event_handler( void *handlerArg, esp_event_base_t base, int32_t id, void *event_data );

    static void task_sub_check_timeout(uint16_t packetId);
    static void task_unsub_check_timeout(void *parameters);
    static void task_connect(void *parameters);
    // static void task_user_callback(void *parameters);
    // static void task_pub_check_timeout(void *parameters);
    static void task_MQTT_keep_alive(void *parameters);
    static void task_resubscribe_after_failed(void *parameters);

    static int sub_timeout_check(uint16_t packetId);
    static int resubscribe(const uint16_t identifier);
    static void publish_resend();
    static int waitForPacketAck_resub_failed(uint16_t packetId);
    static int disconnect();

    /**
     * @brief Array to keep the outgoing publish messages.
     * These stored outgoing publish messages are kept until a successful ack
     * is received.
     */
    static std::map<const uint16_t, MQTTPublishInfo_t> map_outgoingPublishPackets;

    /**
     * @brief The network buffer must remain valid for the lifetime of the MQTT context.
     */
    static uint8_t buffer[];

    /**
     * @brief Array to track the outgoing publish records for outgoing publishes
     * with QoS > 0.
     *
     * This is passed into #MQTT_InitStatefulQoS to allow for QoS > 0.
     *
     */
    static MQTTPubAckInfo_t pOutgoingPublishRecords[];

    /**
     * @brief Array to track the incoming publish records for incoming publishes
     * with QoS > 0.
     *
     * This is passed into #MQTT_InitStatefulQoS to allow for QoS > 0.
     *
     */
    static MQTTPubAckInfo_t pIncomingPublishRecords[];

    /**
     * @brief Status of latest Subscribe ACK;
     * it is updated every time the callback function processes a Subscribe ACK
     * and accounts for subscription to a single topic.
     */
    static MQTTSubAckStatus_t globalSubAckStatus;//

    /**
     * @brief Packet Identifier generated when Subscribe request was sent to the broker;
     * it is used to match received Subscribe ACK to the transmitted subscribe.
     */
    static uint16_t globalSubscribePacketIdentifier;//

    /**
     * @brief Packet Identifier updated when an ACK packet is received.
     *
     * It is used to match an expected ACK for a transmitted packet.
     */
    static uint16_t globalAckPacketIdentifier;//

    /**
     * @brief Packet Identifier generated when Unsubscribe request was sent to the broker;
     * it is used to match received Unsubscribe ACK to the transmitted unsubscribe
     * request.
     */
    static uint16_t globalUnsubscribePacketIdentifier;//

    /**
    * @brief Initializes the MQTT library.
    *
    * @param[in] pMqttContext MQTT context pointer.
    * @param[in] pNetworkContext The network context pointer.
    *
    * @return EXIT_SUCCESS if the MQTT library is initialized;
    * EXIT_FAILURE otherwise.
    */
    static int initializeMqtt(MQTTContext_t * pMqttContext, NetworkContext_t * pNetworkContext);

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
    static int connectToServerWithBackoffRetries( NetworkContext_t * pNetworkContext, MQTTContext_t * pMqttContext, bool * pClientSessionPresent, bool * pBrokerSessionPresent );

    /**
     * @brief Sends an MQTT CONNECT packet over the already connected TCP socket.
     *
     * @param[in] pMqttContext MQTT context pointer.
     * @param[in] createCleanSession Creates a new MQTT session if true.
     * If false, tries to establish the existing session if there was session
     * already present in broker.
     * @param[out] pSessionPresent Session was already present in the broker or not.
     * Session present response is obtained from the CONNACK from broker.
     *
     * @return EXIT_SUCCESS if an MQTT session is established;
     * EXIT_FAILURE otherwise.
     */
    static int establishMqttSession( MQTTContext_t * pMqttContext, bool createCleanSession, bool * pSessionPresent );
    
    /**
     * @brief The random number generator to use for exponential backoff with
     * jitter retry logic.
     *
     * @return The generated random number.
     */
    static uint32_t generateRandomNumber();

    /**
     * @brief The application callback function for getting the incoming publish
     * and incoming acks reported from MQTT library.
     *
     * @param[in] pMqttContext MQTT context pointer.
     * @param[in] pPacketInfo Packet Info pointer for the incoming packet.
     * @param[in] pDeserializedInfo Deserialized information from the incoming packet.
     */
    static void coreMQTT_EventCallback( MQTTContext_t * pMqttContext, MQTTPacketInfo_t * pPacketInfo, MQTTDeserializedInfo_t * pDeserializedInfo );

    /**
     * @brief The function to handle the incoming publishes.
     *
     * @param[in] pPublishInfo Pointer to publish info of the incoming publish.
     * @param[in] packetIdentifier Packet identifier of the incoming publish.
     */
    static void handleIncomingPublish( MQTTPublishInfo_t * pPublishInfo, uint16_t packetIdentifier );

    /**
     * @brief Function to update variable globalSubAckStatus with status
     * information from Subscribe ACK. Called by eventCallback after processing
     * incoming subscribe echo.
     *
     * @param[in] Server response to the subscription request.
     */
    static void MQTT_subBack_event_handler( void *handlerArg, esp_event_base_t base, int32_t id, void *event_data );

    // /**
    //  * @brief Function to get the free index at which an outgoing publish
    //  * can be stored.
    //  *
    //  * @return The output parameter to return the index at which an
    //  * outgoing publish message can be stored.
    //  */
    // static int getNextFreeIndexForOutgoingPublishes();
    
};//MQTT

#endif