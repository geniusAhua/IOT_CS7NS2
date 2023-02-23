package com.iot.smartbin.service;

import com.iot.smartbin.pojo.Bin;

import java.util.List;

public interface BinInfoService {
    List<Bin> getAllBinsInfo();
    List<Bin> getBinInfoViaUserId(Integer userId);
}
