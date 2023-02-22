package com.iot.smartbin.controller;

import com.iot.smartbin.common.ErrorMessage;
import com.iot.smartbin.common.SuccessMessage;
import com.iot.smartbin.service.impl.AdminServiceImpl;
import org.apache.commons.lang3.ObjectUtils.Null;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.bind.annotation.RestController;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpSession;

@RestController
public class AdminController {
    @Autowired
    private AdminServiceImpl adminService;

    @ResponseBody
    @RequestMapping(value = "/admin/login/status", method = RequestMethod.POST)
    public Object loginStatus(HttpServletRequest req, HttpSession session){
        String name = req.getParameter("name");
        String password = req.getParameter("password");
        boolean res = adminService.verifyPassword(name, password);
        if(res) {
            session.setAttribute("name", name);
            return new SuccessMessage<Null>("Login Success").getMessage();
        } else {
            return new ErrorMessage("username or password error").getMessage();
        }
    }
}
