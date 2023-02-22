package com.iot.smartbin.dao;

import com.iot.smartbin.pojo.Bin;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface BinMapper {
    List<Bin> getAllBinsInfo();
    List<Bin> getBinViaUserId(Integer userId);
}
