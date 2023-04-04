package com.iot.smartbin.mqtt;

import com.amazonaws.services.iot.client.*;
import com.iot.smartbin.config.MQTTConfig;
import com.iot.smartbin.mqtt.model.PublishListener;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;

/**
 * @author Siuyun Yip
 * @version 1.0
 * @date 2023/2/27 13:58
 */
@Service
@Slf4j
public class MqttPubSubService {
    private static final int TIMEOUT = 3000; // milliseconds
    private static final AWSIotQos QOS0 = AWSIotQos.QOS0;
    private static final AWSIotQos QOS1 = AWSIotQos.QOS1;

    // publish a message to aws iot platform
    public void publishMessage(String topic, Object payload) {
        MQTTConfig mqttConfig = new MQTTConfig();
        AWSIotMqttClient client = mqttConfig.getClient();
        try {
            PublishListener message = new PublishListener(topic, QOS0, payload.toString());
            client.publish(message, TIMEOUT);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // subscribe a topic on aws iot platform
    public void subscribeMessage(AWSIotTopic topic) {
        MQTTConfig mqttConfig = new MQTTConfig();
        AWSIotMqttClient client = mqttConfig.getClient();
        try {
            client.subscribe(topic, TIMEOUT);
        } catch (AWSIotException e) {
            e.printStackTrace();
        }
    }
}
