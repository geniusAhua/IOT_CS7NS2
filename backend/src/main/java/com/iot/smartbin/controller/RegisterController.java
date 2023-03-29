package com.iot.smartbin.controller;

import com.iot.smartbin.service.RegisterService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

@RestController
public class RegisterController {
    @Autowired
    private RegisterService registerService;

    @PostMapping("/api/user/account/register")
    public Map<String, String> register(@RequestParam String username, String password, String confirmpassword){
        return registerService.register(username, password, confirmpassword);
    }
}
