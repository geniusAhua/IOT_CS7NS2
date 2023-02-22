package com.iot.smartbin.controller;

import com.iot.smartbin.common.SuccessMessage;
import com.iot.smartbin.pojo.Bin;
import com.iot.smartbin.service.impl.BinInfoServiceImpl;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

import javax.servlet.http.HttpServletRequest;
import java.util.*;

@RestController
public class BinController {
    @Autowired
    private BinInfoServiceImpl binInfoService;

    @RequestMapping(value = "/bin/details", method = RequestMethod.GET)
    public Object getBinDetails(HttpServletRequest req){
        String user_id = req.getParameter("userId").trim();
        List<Bin> list = binInfoService.getBinInfoViaUserId(Integer.parseInt(user_id));
        List<Map<String, String>> ans = new ArrayList<>();
        for(Bin bin : list){
            Map<String, String> map = new HashMap<>();
            map.put("weight", bin.getWeight().toString());
            map.put("height", bin.getHeight().toString());
            map.put("isFull", bin.getIsFull().toString());
            map.put("longitude", bin.getLongitude().toString());
            map.put("latitude", bin.getLatitude().toString());
        }
        return new SuccessMessage<List<Map<String, String>>>("details of the bin", ans).getMessage();
    }

}
