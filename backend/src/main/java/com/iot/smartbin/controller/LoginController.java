package com.iot.smartbin.controller;

import com.iot.smartbin.service.LoginService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
public class LoginController {
    @Autowired
    private LoginService loginService;

    @PostMapping(("/api/user/account/login"))
    public Map<String, String> login(@RequestParam String username, String password){
        return loginService.login(username, password);
    }
}
