package edu.huaouo.curriculumdesign.controller;

import edu.huaouo.curriculumdesign.model.AdminInfo;
import edu.huaouo.curriculumdesign.model.LocationInfo;
import edu.huaouo.curriculumdesign.model.StudentInfo;
import edu.huaouo.curriculumdesign.model.TeacherInfo;
import edu.huaouo.curriculumdesign.service.AdminService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

import javax.servlet.http.HttpSession;

@RestController
@RequestMapping("/controller")
public class AdminController {

    private final AdminService adminService;

    @Autowired
    public AdminController(AdminService adminService) {
        this.adminService = adminService;
    }

    // 获取学号
    @RequestMapping(value = "/getStudentNumber")
    public String getAllLocationInfo(@RequestBody LocationInfo locationInfo, HttpSession session) {
        String id = (String) session.getAttribute("id");
        String ret = "ILLEGAL";
        if (adminService.checkIfAdmin(id)) {
            ret = adminService.getStudentNumber(locationInfo);
        }
        return ret;
    }

    // 添加学生
    @RequestMapping(value = "/addStudent", method = RequestMethod.POST)
    public String addStudent(@RequestBody StudentInfo studentInfo, HttpSession session) {
        String id = (String) session.getAttribute("id");
        String ret = "ILLEGAL";
        if (adminService.checkIfAdmin(id)) {
            ret = adminService.addStudent(studentInfo);
        }
        return ret;
    }

    // 添加教师
    @RequestMapping(value = "/addTeacher", method = RequestMethod.POST)
    public String addTeacher(@RequestBody TeacherInfo teacherInfo, HttpSession session) {
        String id = (String) session.getAttribute("id");
        String ret = "ILLEGAL";
        if (adminService.checkIfAdmin(id)) {
            ret = adminService.addTeacher(teacherInfo);
        }
        return ret;
    }

    // 添加管理员
    @RequestMapping(value = "/addAdmin", method = RequestMethod.POST)
    public String addAdmin(@RequestBody AdminInfo adminInfo, HttpSession session) {
        String id = (String) session.getAttribute("id");
        String ret = "ILLEGAL";
        if (adminService.checkIfAdmin(id)) {
            ret = adminService.addAdmin(adminInfo);
        }
        return ret;
    }

    // 删除用户
    @RequestMapping(value = "/deleteUser", method = RequestMethod.POST)
    public String deleteUser(String deleteId, HttpSession session) {
        String id = (String) session.getAttribute("id");
        String ret = "ILLEGAL";
        if (adminService.checkIfAdmin(id)) {
            ret = adminService.deleteUser(deleteId);
        }
        return ret;
    }
}
