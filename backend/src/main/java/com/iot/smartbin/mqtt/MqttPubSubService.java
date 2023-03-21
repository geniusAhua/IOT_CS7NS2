package com.iot.smartbin.mqtt;

import com.amazonaws.services.iot.client.AWSIotException;
import com.amazonaws.services.iot.client.AWSIotMqttClient;
import com.amazonaws.services.iot.client.AWSIotQos;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.iot.smartbin.config.MQTTConfig;
import com.iot.smartbin.mqtt.model.PublishListener;
import com.iot.smartbin.mqtt.model.MyTopic;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

/**
 * @author Siuyun Yip
 * @version 1.0
 * @date 2023/2/27 13:58
 */
@Service
public class MqttPubSubService {
    private static final int TIMEOUT = 3000; // milliseconds
    private static final AWSIotQos QOS0 = AWSIotQos.QOS0;
    private static final AWSIotQos QOS1 = AWSIotQos.QOS1;

    @Autowired
    private MQTTConfig mqttConfig;

    // publish a message to aws iot platform
    public void publishMessage(String topic, Object payload) {
        AWSIotMqttClient client = mqttConfig.getClient();
        try {
            ObjectMapper mapper = new ObjectMapper();
            PublishListener message = new PublishListener(topic, QOS0, mapper.writeValueAsString(payload));
            client.publish(message, TIMEOUT);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // subscribe a topic on aws iot platform
    public void subscribeMessage(String topicName) {
        AWSIotMqttClient client = mqttConfig.getClient();

        MyTopic topic = new MyTopic(topicName, QOS0);
        try {
            client.subscribe(topic, TIMEOUT);
        } catch (AWSIotException e) {
            e.printStackTrace();
        }
    }
}
