package com.iot.smartbin.dao.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.iot.smartbin.dao.pojo.User;
import org.apache.ibatis.annotations.Mapper;

@Mapper
public interface UserMapper extends BaseMapper<User> {
}
