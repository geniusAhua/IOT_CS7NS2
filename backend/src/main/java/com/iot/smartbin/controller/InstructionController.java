package com.iot.smartbin.controller;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.iot.smartbin.Instruction.model.Instruction;
import com.iot.smartbin.mqtt.MqttPubSubService;
import com.iot.smartbin.mqtt.model.PublishListener;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

@RestController
public class InstructionController {

    @Autowired
    private MqttPubSubService mqttService;

    @RequestMapping(value = "/instruction", method = RequestMethod.POST)
    public void sendInstruction(@RequestParam String type){
        try {
            Instruction instruction = new Instruction(type);
            ObjectMapper mapper = new ObjectMapper();
            mqttService.publishMessage("instruction", mapper.writeValueAsString(instruction));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
