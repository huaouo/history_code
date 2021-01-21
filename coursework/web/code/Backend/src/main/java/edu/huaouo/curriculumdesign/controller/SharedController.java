package edu.huaouo.curriculumdesign.controller;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import edu.huaouo.curriculumdesign.model.ChangePasswordInfo;
import edu.huaouo.curriculumdesign.model.LoginInfo;
import edu.huaouo.curriculumdesign.service.SharedService;
import edu.huaouo.curriculumdesign.model.UserType;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

import javax.servlet.http.HttpSession;

@RestController
@RequestMapping("/controller")
public class SharedController {
    private final SharedService sharedService;
    private static final ObjectMapper objectMapper = new ObjectMapper();

    @Autowired
    public SharedController(SharedService sharedService) {
        this.sharedService = sharedService;
    }

    // 登录
    @RequestMapping(value = "/login", method = RequestMethod.POST)
    public String login(@RequestBody LoginInfo loginInfo, HttpSession session) {
        UserType userType = sharedService.login(loginInfo);
        session.setAttribute("userType", userType);
        session.setAttribute("id", loginInfo.getId());
        String ret = "ILLEGAL";
        try {
            ret = objectMapper.writeValueAsString(userType);
        } catch (JsonProcessingException e) {
            e.printStackTrace();
        }
        return ret;
    }

    // 修改密码
    @RequestMapping(value = "/changePassword", method = RequestMethod.POST)
    public String changePassword(@RequestBody ChangePasswordInfo changePasswordInfo, HttpSession session) {
        UserType userType = (UserType) session.getAttribute("userType");
        String id = (String) session.getAttribute("id");
        String ret = "ILLEGAL";
        LoginInfo loginInfo = new LoginInfo(id, changePasswordInfo.getPreviousPassword());
        if (userType != null && id != null
                && sharedService.login(loginInfo) != UserType.ILLEGAL
                && sharedService.modifyPassword(id, userType, changePasswordInfo.getNewPassword())) {
            ret = "SUCCESS";
        }
        return ret;
    }

    // 获取学期列表
    @RequestMapping("/getTermList")
    public String getTermList(HttpSession session) {
        String ret = "ILLEGAL";
        if (session.getAttribute("userType") != UserType.ILLEGAL) {
            try {
                ret = objectMapper.writeValueAsString(sharedService.getTermList());
            } catch (JsonProcessingException e) {
                e.printStackTrace();
            }
        }
        return ret;
    }

    // 获取班级列表
    @RequestMapping("/getClassList")
    public String getClassList(HttpSession session) {
        String ret = "ILLEGAL";
        if (session.getAttribute("userType") != UserType.ILLEGAL) {
            try {
                ret = objectMapper.writeValueAsString(sharedService.getClassList());
            } catch (JsonProcessingException e) {
                e.printStackTrace();
            }
        }
        return ret;
    }
}
