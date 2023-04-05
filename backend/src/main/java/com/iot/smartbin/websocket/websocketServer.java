package com.iot.smartbin.websocket;

import com.amazonaws.services.iot.client.AWSIotQos;
import com.amazonaws.services.iot.client.AWSIotTopic;
import com.iot.smartbin.mqtt.MqttPubSubService;
import com.iot.smartbin.mqtt.model.TrashHeightTopic;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Component;

import javax.websocket.*;
import javax.websocket.server.ServerEndpoint;
import java.io.IOException;
import java.util.concurrent.CopyOnWriteArraySet;

@Component
@ServerEndpoint("/websocket/client")
public class websocketServer {
    private Session session;
    private int onlineCount = 0;
    private static final CopyOnWriteArraySet<websocketServer> webSocketSet = new CopyOnWriteArraySet<websocketServer>();
    private static final Logger logger = LoggerFactory.getLogger(websocketServer.class);

    @OnOpen
    public void onOpen(Session session){
        MqttPubSubService mqttPubSubService = new MqttPubSubService();
        AWSIotTopic smartBinInfo = new TrashHeightTopic("SmartBinInfo", AWSIotQos.QOS0, this);
        mqttPubSubService.subscribeMessage(smartBinInfo);
        logger.info("Subscribe Smart Bin Information");

        this.session = session;
        webSocketSet.add(this);
        onlineCount++;
        logger.info("A client join! Client number: " + onlineCount);
        logger.info("The total number of clients is " + onlineCount);
    }
    @OnClose
    public void onClose(){
        onlineCount--;
        webSocketSet.remove(this);
        logger.info("A client left! Client number: " + onlineCount);
        logger.info("The total number of clients is " + onlineCount);
    }
    @OnMessage
    public void onMessage(String message){
        logger.info("Got message from " + session.getId() + " : " + message);

    }
    public void sendMessage(String msg) throws IOException {
        for(websocketServer item: webSocketSet){
            item.session.getBasicRemote().sendText(msg);
        }
    }


}
