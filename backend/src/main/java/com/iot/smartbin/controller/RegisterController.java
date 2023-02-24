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
    public Map<String, String> register(@RequestParam Map<String, String> req){
        String username = req.get("username");
        String password = req.get("password");
        String confirmPassword = req.get("confirmPassword");
        return registerService.register(username, password, confirmPassword);
    }
}
