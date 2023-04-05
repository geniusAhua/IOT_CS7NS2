package com.iot.smartbin.config;

import com.amazonaws.services.iot.client.AWSIotException;
import com.amazonaws.services.iot.client.AWSIotMqttClient;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Configuration;

/**
 * @author Siuyun Yip
 * @version 1.0
 * @date 2023/2/27 14:02
 */
@Configuration
@Slf4j
public class MQTTConfig {

    private String clientEndpoint = null;

    private String clientId = null;

    private String accessKey = null;

    private String secretAccessKey = null;

    private AWSIotMqttClient client;

    public AWSIotMqttClient getClient() {
        if(client == null){
            client = new AWSIotMqttClient(clientEndpoint, clientId, accessKey, secretAccessKey, null);
            try {
                client.connect();
            } catch (AWSIotException e) {
                e.printStackTrace();
            }
        }
        log.info("Connected!");
        return client;
    }
}
