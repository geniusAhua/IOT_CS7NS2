package com.iot.smartbin.controller.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.iot.smartbin.pojo.Bin;
import org.apache.ibatis.annotations.Mapper;

@Mapper
public interface BinMapper extends BaseMapper<Bin> {
}
