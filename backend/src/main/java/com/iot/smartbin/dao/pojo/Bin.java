package com.iot.smartbin.dao.pojo;

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
    private Float weight;
    private Integer height;
    private Float longitude;
    private Float Latitude;
    private Boolean isFull;
}
