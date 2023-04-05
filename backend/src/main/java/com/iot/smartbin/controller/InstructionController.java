package com.iot.smartbin.controller;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.iot.smartbin.Instruction.model.Instruction;
import com.iot.smartbin.mqtt.MqttPubSubService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

@RestController
public class InstructionController {

    @Autowired
    private MqttPubSubService mqttService;

    @RequestMapping(value = "/api/instruction", method = RequestMethod.POST)
    public void sendInstruction(@RequestParam Integer type){
        try {
            Instruction instruction = new Instruction(type);
            ObjectMapper mapper = new ObjectMapper();
            mqttService.publishMessage("instruction", mapper.writeValueAsString(instruction));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
