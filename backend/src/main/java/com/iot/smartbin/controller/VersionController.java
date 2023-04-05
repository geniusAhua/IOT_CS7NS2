package com.iot.smartbin.controller;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.amazonaws.services.s3.AmazonS3;
import com.iot.smartbin.config.AwsS3Config;
import com.iot.smartbin.mqtt.MqttPubSubService;
import com.iot.smartbin.s3.model.VersionControl;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RestController;


/**
 * @author Siuyun Yip
 * @version 1.0
 * @date 2023/4/4 14:37
 */
@RestController
@Slf4j
public class VersionController {
    @Autowired
    private MqttPubSubService mqttService;
    @Autowired
    private AwsS3Config s3Config;

    @PostMapping("/version")
    public String publishNewVersion() {
        AmazonS3 s3Client = s3Config.getS3Client();
        String firmwareUrl = s3Client.getUrl("iot-smartbin-mirror", "hardware.bin").toString();
        VersionControl vc = new VersionControl(firmwareUrl);
        String jsonString = JSON.toJSONString(vc);
        mqttService.publishMessage("control", jsonString);

        return "message Published Successfully";
    }
}
