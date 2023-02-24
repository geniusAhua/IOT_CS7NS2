package com.iot.smartbin;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.annotation.ComponentScan;
@ComponentScan("com.iot.smartbin.controller.mapper.BinMapper")
@SpringBootApplication
public class SmartbinApplication {

    public static void main(String[] args) {
        SpringApplication.run(SmartbinApplication.class, args);
    }

}
