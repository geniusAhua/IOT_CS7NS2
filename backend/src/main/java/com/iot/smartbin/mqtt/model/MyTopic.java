package com.iot.smartbin.mqtt.model;

import com.amazonaws.services.iot.client.AWSIotMessage;
import com.amazonaws.services.iot.client.AWSIotQos;
import com.amazonaws.services.iot.client.AWSIotTopic;

/**
 * @author Siuyun Yip
 * @version 1.0
 * @date 2023/2/27 15:54
 */
public class MyTopic extends AWSIotTopic {
    public MyTopic(String topic, AWSIotQos qos) {
        super(topic, qos);
    }

    @Override
    public void onMessage(AWSIotMessage message) {
        // called when a message is received
        System.out.println("Message received");
    }
}
