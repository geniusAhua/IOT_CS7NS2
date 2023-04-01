package com.iot.smartbin.controller;

import com.iot.smartbin.mqtt.MqttPubSubService;
import com.iot.smartbin.msg.BinInfoMsg;
import com.iot.smartbin.service.BinInfoService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import javax.websocket.server.ServerEndpoint;
import java.util.*;

@RestController
public class BinController {
    @Autowired
    private BinInfoService binInfoService;

    @Autowired
    private MqttPubSubService mqttService;

    @GetMapping("/api/bin/info")
    public Map<String, String> getBinDetails(@RequestParam Integer id){
        return binInfoService.binInfo(id);
    }


    // example of publishing a message to aws iot platform
    @PostMapping("/publish")
    public String publishBinMessage() {
        mqttService.publishMessage("trash_weight", new BinInfoMsg(20));

        return "message Published Successfully";
    }

}
