package com.iot.smartbin.service.impl;

import com.iot.smartbin.dao.BinMapper;
import com.iot.smartbin.pojo.Bin;
import com.iot.smartbin.service.BinInfoService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;
@Service
public class BinInfoServiceImpl implements BinInfoService {
    @Autowired
    private BinMapper binMapper;

    @Override
    public List<Bin> getAllBinsInfo() {
        return binMapper.getAllBinsInfo();
    }

    @Override
    public List<Bin> getBinInfoViaUserId(Integer userId) {
        return binMapper.getBinViaUserId(userId);
    }
}
