package com.iot.smartbin.service;

import java.util.Map;

public interface RegisterService {
    public Map<String, String> register(String username, String password, String confirmPassword);
}
