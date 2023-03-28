package com.iot.smartbin.mqtt.model;

import com.amazonaws.services.iot.client.AWSIotMessage;
import com.amazonaws.services.iot.client.AWSIotQos;
import com.amazonaws.services.iot.client.AWSIotTopic;
import com.iot.smartbin.websocket.websocketServer;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.io.IOException;
import java.net.http.WebSocket;

/**
 * @author Siuyun Yip
 * @version 1.0
 * @date 2023/2/27 15:59
 */
@Slf4j
public class TrashHeightTopic extends AWSIotTopic {
    private websocketServer websocketServer;
    public TrashHeightTopic(String topic, AWSIotQos qos, websocketServer websocketServer) {
        super(topic, qos);
        this.websocketServer = websocketServer;
    }

    @Override
    public void onMessage(AWSIotMessage message) {
        System.out.println(message.getStringPayload());
        try {
            websocketServer.sendMessage(message.getStringPayload());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
