package com.example.web.controller;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseBody;

@Controller
public class HelloController {

    @RequestMapping(value = "/", method = RequestMethod.GET)
    @ResponseBody
    public String printWelcome() {
        return "hello world";
    }

    @RequestMapping(value = "/checkpreload.htm", method = RequestMethod.GET)
    @ResponseBody
    public String printSuccess() {
        return "success";
    }
}
