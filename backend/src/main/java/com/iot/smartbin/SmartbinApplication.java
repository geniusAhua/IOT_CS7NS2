package com.iot.smartbin;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.web.socket.config.annotation.EnableWebSocket;
import org.springframework.web.socket.config.annotation.EnableWebSocketMessageBroker;

@SpringBootApplication
@EnableWebSocket
@EnableWebSocketMessageBroker
public class SmartbinApplication {

    public static void main(String[] args) {
        SpringApplication.run(SmartbinApplication.class, args);
    }

}
