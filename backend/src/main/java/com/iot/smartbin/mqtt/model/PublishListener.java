package com.iot.smartbin.mqtt.model;

import com.amazonaws.services.iot.client.AWSIotMessage;
import com.amazonaws.services.iot.client.AWSIotQos;

/**
 * @author Siuyun Yip
 * @version 1.0
 * @date 2023/2/27 14:50
 */
public class PublishListener extends AWSIotMessage {

    public PublishListener(String topic, AWSIotQos qos, String payload) {
        super(topic, qos, payload);
    }

    @Override
    public void onSuccess() {
        System.out.println(System.currentTimeMillis() + ": >>>" + getStringPayload());
    }

    @Override
    public void onFailure() {
        System.out.println(System.currentTimeMillis() + ": publish failed for " + getStringPayload() );
    }

    @Override
    public void onTimeout() {
        System.out.println(System.currentTimeMillis() + ": publish timeout for " + getStringPayload());
    }
}
