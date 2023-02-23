package com.iot.smartbin.pojo;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class Consumer {
    private Integer id;
    private String username;
    private String password;
    private String photo;
}
