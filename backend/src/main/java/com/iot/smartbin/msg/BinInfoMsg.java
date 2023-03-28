package com.iot.smartbin.msg;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * @author Siuyun Yip
 * @version 1.0
 * @date 2023/2/27 11:30
 */
@Data
@AllArgsConstructor
@NoArgsConstructor
public class BinInfoMsg {

    private int height;

    @Override
    public String toString() {
        return "height: " + height;
    }
}
