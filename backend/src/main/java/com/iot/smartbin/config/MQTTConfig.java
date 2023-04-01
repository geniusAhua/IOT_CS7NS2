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
//    @Value("${clientEndpoint}")
    private String clientEndpoint = "a3ravxmuj0c4e2-ats.iot.eu-west-1.amazonaws.com";

//    @Value("${clientId}")
    private String clientId = "smartbin-backend";

//    @Value("${accessKey}")
    private String accessKey = "AKIA26U2W4737NMDR4E5";

//    @Value("${secretAccessKey}")
    private String secretAccessKey = "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";

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
