package com.iot.smartbin.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.iot.smartbin.controller.mapper.BinMapper;
import com.iot.smartbin.pojo.Bin;
import com.iot.smartbin.service.BinInfoService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Service
public class BinInfoServiceImpl implements BinInfoService {
    @Autowired
    private BinMapper binMapper;

    @Override
    public Map<String, String> binInfo(Integer userId) {
        Map<String, String> map = new HashMap<>();
        QueryWrapper<Bin> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("userId", userId);
        List<Bin> bins = binMapper.selectList(queryWrapper);
        for(Bin bin : bins){
            map.put("id", bin.getId().toString());
            map.put("weight", bin.getWeight().toString());
            map.put("height", bin.getHeight().toString());
            map.put("longitude", bin.getLongitude().toString());
            map.put("latitude", bin.getLatitude().toString());
            map.put("isFull", bin.getIsFull().toString());
        }
        return map;
    }
}
