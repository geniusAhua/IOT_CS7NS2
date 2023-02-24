package com.iot.smartbin.controller.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.iot.smartbin.pojo.User;
import org.apache.ibatis.annotations.Mapper;

@Mapper
public interface UserMapper extends BaseMapper<User> {
}
