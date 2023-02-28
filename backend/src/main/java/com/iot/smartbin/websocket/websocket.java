package com.iot.smartbin.websocket;

import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Component;

import javax.websocket.*;
import javax.websocket.server.PathParam;
import javax.websocket.server.ServerEndpoint;
import java.net.http.WebSocket;
import java.util.ArrayList;
import java.util.List;



@Component
@ServerEndpoint("/websocket")
@Slf4j
public class websocket {
    private Session session;
    private int onlineCount = 0;
    private static List<WebSocket> clients = new ArrayList<>();

    @OnOpen
    public void onOpen(Session session){
        this.session = session;
        clients.add((WebSocket) this);
        onlineCount++;
        log.info("A client join! Client number: " + onlineCount);
    }
    @OnClose
    public void onClose(){
        onlineCount--;
        clients.remove(this);
        log.info("A client disconnect! Client number: " + onlineCount);
    }
    public static void sendMessage(String message) {

        for(WebSocket webSocket: clients){
            try{
                CharSequence mes = message;
                webSocket.sendText(mes, false);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

}
