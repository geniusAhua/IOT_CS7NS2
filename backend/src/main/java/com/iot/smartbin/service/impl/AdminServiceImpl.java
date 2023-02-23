package com.iot.smartbin.service.impl;

import com.iot.smartbin.dao.AdminMapper;
import com.iot.smartbin.service.AdminService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
public class AdminServiceImpl implements AdminService {
    @Autowired
    private AdminMapper adminMapper;

    @Override
    public boolean verifyPassword(String name, String password) {
        return adminMapper.verifyPassword(name, password) > 0;
    }
}
