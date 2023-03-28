package com.iot.smartbin.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;

import com.iot.smartbin.dao.mapper.UserMapper;
import com.iot.smartbin.dao.pojo.User;
import com.iot.smartbin.service.LoginService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

@Service
public class LoginServiceImpl implements LoginService {
    @Autowired
    private UserMapper userMapper;

    @Override
    public Map<String, String> login(String username, String password) {
        Map<String, String> map = new HashMap<>();
        QueryWrapper<User> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("username", username);
        List<User> users = userMapper.selectList(queryWrapper);
        if(users.isEmpty()){
            map.put("error_message", "The user is not exist");
            return map;
        } else if(!Objects.equals(users.get(0).getPassword(), password)){
            map.put("error_message", "Incorrect password");
            return map;
        }
        map.put("error_message", "success");
        return map;
    }
}
