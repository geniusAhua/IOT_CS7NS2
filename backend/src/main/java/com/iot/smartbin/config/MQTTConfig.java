package com.iot.smartbin.config;

import com.amazonaws.services.iot.client.AWSIotException;
import com.amazonaws.services.iot.client.AWSIotMqttClient;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Configuration;

/**
 * @author Siuyun Yip
 * @version 1.0
 * @date 2023/2/27 14:02
 */
@Configuration
public class MQTTConfig {
    @Value("${clientEndpoint}")
    private String clientEndpoint;

    @Value("${clientId}")
    private String clientId;

    @Value("${accessKey}")
    private String accessKey;

    @Value("${secretAccessKey}")
    private String secretAccessKey;

    private AWSIotMqttClient client;

    public AWSIotMqttClient getClient() {
        if (client == null) {
            client = new AWSIotMqttClient(clientEndpoint, clientId, accessKey, secretAccessKey, null);
            try {
                client.connect();
            } catch (AWSIotException e) {
                e.printStackTrace();
            }

            System.out.println("Connected!");
        }

        return client;
    }
}
