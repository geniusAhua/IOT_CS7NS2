package com.iot.smartbin.pojo;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class Bin {
    private Integer id;
    private Integer ownerId;
    private Float weight; // kg
    private Integer height; // cm
    private Float longitude;
    private Float latitude;
    private Boolean isTotallyFull; // cannot press anymore
    private Boolean isFull;
}
