package com.iot.smartbin.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.iot.smartbin.dao.mapper.BinMapper;
import com.iot.smartbin.dao.pojo.Bin;
import com.iot.smartbin.service.BinInfoService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Service
public class BinInfoServiceImpl extends ServiceImpl<BinMapper, Bin> implements BinInfoService {
    @Autowired
    private BinMapper binMapper;

    @Override
    public Map<String, String> binInfo(Integer id) {
        Map<String, String> map = new HashMap<>();
        QueryWrapper<Bin> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("id", id);

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
