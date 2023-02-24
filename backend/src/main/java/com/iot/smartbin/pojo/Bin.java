package com.iot.smartbin.pojo;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableId;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class Bin {
    @TableId(type = IdType.AUTO)
    private Integer id;
    private Integer userId;
    private Float weight; // kg
    private Integer height; // cm
    private Float longitude;
    private Float latitude;
    private Boolean isFull;
}