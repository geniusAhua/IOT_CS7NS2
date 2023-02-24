package com.iot.smartbin.controller;

import com.iot.smartbin.service.BinInfoService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.*;

@RestController
public class BinController {
    @Autowired
    private BinInfoService binInfoService;

    @GetMapping("/api/bin/info/")
    public Map<String, String> getBinDetails(@RequestParam Integer userId){
        return binInfoService.binInfo(userId);
    }

}
