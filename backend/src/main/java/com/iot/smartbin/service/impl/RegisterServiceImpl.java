package com.iot.smartbin.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.iot.smartbin.dao.mapper.UserMapper;
import com.iot.smartbin.dao.pojo.User;
import com.iot.smartbin.service.RegisterService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Service
public class RegisterServiceImpl implements RegisterService {
    @Autowired
    private UserMapper userMapper;

    @Override
    public Map<String, String> register(String username, String password, String confirmedPassword) {
        Map<String, String> map = new HashMap<>();
        if (username == null) {
            map.put("error_message", "username shouldn't be blank");
            return map;
        }
        if (password == null || confirmedPassword == null) {
            map.put("error_message", "password shouldn't be blank");
            return map;
        }

        username = username.trim();
        if (username.length() == 0) {
            map.put("error_message", "username shouldn't be blank");
            return map;
        }

        if (password.length() == 0 || confirmedPassword.length() == 0) {
            map.put("error_message", "password shouldn't be blank");
            return map;
        }

        if (username.length() > 100) {
            map.put("error_message", "Length of username should less than 100");
            return map;
        }

        if (password.length() > 100 || confirmedPassword.length() > 100) {
            map.put("error_message", "Length of password should less than 100");
            return map;
        }

        if (!password.equals(confirmedPassword)) {
            map.put("error_message", "password and confirm password are not same");
            return map;
        }

        QueryWrapper<User> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("username", username);
        List<User> users = userMapper.selectList(queryWrapper);
        if (!users.isEmpty()) {
            map.put("error_message", "The username is exist");
            return map;
        }

        String photo = "https://cdn.acwing.com/media/user/profile/photo/1_lg_844c66b332.jpg";
        User user = new User(null, username, password, photo);
        userMapper.insert(user);

        map.put("error_message", "success");
        return map;
    }
}