package com.iot.smartbin.service;

import java.util.Map;

public interface BinInfoService {
    Map<String, String> binInfo(Integer userId);
}
