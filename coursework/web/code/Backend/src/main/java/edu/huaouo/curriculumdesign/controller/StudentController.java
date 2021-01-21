package edu.huaouo.curriculumdesign.controller;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import edu.huaouo.curriculumdesign.model.TermAndClassInfo;
import edu.huaouo.curriculumdesign.model.TermInfo;
import edu.huaouo.curriculumdesign.service.StudentService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

import javax.servlet.http.HttpSession;

@RestController
@RequestMapping("/controller")
public class StudentController {
    private final StudentService studentService;
    private static final ObjectMapper objectMapper = new ObjectMapper();

    @Autowired
    public StudentController(StudentService studentService) {
        this.studentService = studentService;
    }

    // 获取学生信息
    @RequestMapping("/getStudentInfo")
    public String getStudentInfo(HttpSession session) {
        String id = (String) session.getAttribute("id");
        String ret = "ILLEGAL";
        if (studentService.checkIfStudent(id)) {
            try {
                ret = objectMapper.writeValueAsString(studentService.getStudentInfo(id));
            } catch (JsonProcessingException e) {
                e.printStackTrace();
            }
        }
        return ret;
    }

    // 根据学生获取成绩
    @RequestMapping(value = "/getScoreByStudent", method = RequestMethod.POST)
    public String getScoreByStudent(@RequestBody TermInfo termInfo, HttpSession session) {
        String id = (String) session.getAttribute("id");
        String ret = "ILLEGAL";
        if (studentService.checkIfStudent(id)) {
            try {
                ret = objectMapper.writeValueAsString(studentService.getScoreByStudent(termInfo, id));
            } catch (JsonProcessingException e) {
                e.printStackTrace();
            }
        }
        return ret;
    }

    // 获得学业情况
    @RequestMapping("/getAcademicStatus")
    public String getAcademicStatus(HttpSession session) {
        String id = (String) session.getAttribute("id");
        String ret = "ILLEGAL";
        if (studentService.checkIfStudent(id)) {
            try {
                ret = objectMapper.writeValueAsString(studentService.getAcademicStatus(id));
            } catch (JsonProcessingException e) {
                e.printStackTrace();
            }
        }
        return ret;
    }

    // 获取课程列表
    @RequestMapping(value = "/getCourseList", method = RequestMethod.POST)
    public String getCourseList(@RequestBody TermAndClassInfo termAndClassInfo, HttpSession session) {
        String id = (String) session.getAttribute("id");
        String ret = "ILLEGAL";
        if (studentService.checkIfStudent(id)) {
            try {
                ret = objectMapper.writeValueAsString(studentService.getCourseList(termAndClassInfo));
            } catch (JsonProcessingException e) {
                e.printStackTrace();
            }
        }
        return ret;
    }
}
