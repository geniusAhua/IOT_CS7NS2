package com.iot.smartbin.websocket;

import com.iot.smartbin.mqtt.MqttPubSubService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Component;

import javax.websocket.*;
import javax.websocket.server.ServerEndpoint;
import java.io.IOException;
import java.util.concurrent.CopyOnWriteArraySet;


@Component
@ServerEndpoint("/websocket/client")
@Slf4j
public class websocketServer {
    private Session session;
    private int onlineCount = 0;
    private static CopyOnWriteArraySet<websocketServer> webSocketSet = new CopyOnWriteArraySet<websocketServer>();

    @OnOpen
    public void onOpen(Session session){
        MqttPubSubService mqttPubSubService = new MqttPubSubService();
        //TODO get topicName
        mqttPubSubService.subscribeMessage("trash_height");
        log.info("trash_height");
        mqttPubSubService.subscribeMessage("trash_humidity");
        log.info("trash_humidity");
        this.session = session;
        webSocketSet.add(this);
        onlineCount++;
        log.info("A client join! Client number: " + onlineCount);
        log.info("The total number of clients is " + onlineCount);
    }
    @OnClose
    public void onClose(){
        onlineCount--;
        webSocketSet.remove(this);
        log.info("A client left! Client number: " + onlineCount);
        log.info("The total number of clients is " + onlineCount);
    }
    @OnMessage
    public void onMessage(String message){
        log.info("Got message from " + session.getId() + " : " + message);

    }
    public void sendMessage(String msg) throws IOException {
        this.session.getBasicRemote().sendText(msg);
    }


}
