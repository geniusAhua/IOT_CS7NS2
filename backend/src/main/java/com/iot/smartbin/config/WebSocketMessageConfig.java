package com.iot.smartbin.config;

import org.springframework.context.annotation.Configuration;
import org.springframework.messaging.simp.config.MessageBrokerRegistry;
import org.springframework.web.socket.config.annotation.EnableWebSocketMessageBroker;
import org.springframework.web.socket.config.annotation.StompEndpointRegistry;
import org.springframework.web.socket.config.annotation.WebSocketMessageBrokerConfigurer;

/**
 * @author Siuyun Yip
 * @version 1.0
 * @date 2023/2/27 11:22
 */
@Configuration
@EnableWebSocketMessageBroker
public class WebSocketMessageConfig implements WebSocketMessageBrokerConfigurer {
    @Override
    public void registerStompEndpoints(final StompEndpointRegistry registry) {
        // setting the endpoint for connection
        registry.addEndpoint("/smartbin-backend").withSockJS();
    }

    @Override
    public void configureMessageBroker(final MessageBrokerRegistry registry) {
        // requestMapping with prefix 'app'
        registry.setApplicationDestinationPrefixes("/app");
        // message sent to 'address' should start with topic
        registry.enableSimpleBroker("/topic");
    }
}
