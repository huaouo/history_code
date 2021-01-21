package edu.huaouo.curriculumdesign.controller;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import edu.huaouo.curriculumdesign.model.CourseAndClassInfo;
import edu.huaouo.curriculumdesign.model.ScoreInfo;
import edu.huaouo.curriculumdesign.model.TermInfo;
import edu.huaouo.curriculumdesign.service.TeacherService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

import javax.servlet.http.HttpSession;
import java.util.List;

@RestController
@RequestMapping("/controller")
public class TeacherController {
    private final TeacherService teacherService;
    private static final ObjectMapper objectMapper = new ObjectMapper();

    @Autowired
    public TeacherController(TeacherService teacherService) {
        this.teacherService = teacherService;
    }

    // 获取教师信息
    @RequestMapping("/getTeacherInfo")
    public String getTeacherInfo(HttpSession session) {
        String id = (String) session.getAttribute("id");
        String ret = "ILLEGAL";
        if (teacherService.checkIfTeacher(id)) {
            try {
                ret = objectMapper.writeValueAsString(teacherService.getTeacherInfo(id));
            } catch (JsonProcessingException e) {
                e.printStackTrace();
            }
        }
        return ret;
    }

    // 获取班级课程信息
    @RequestMapping(value = "/getTeachCourseAndClassListByTerm", method = RequestMethod.POST)
    public String getTeachCourseAndClassListByTerm(@RequestBody TermInfo termInfo, HttpSession session) {
        String id = (String) session.getAttribute("id");
        String ret = "ILLEGAL";
        if (teacherService.checkIfTeacher(id)) {
            try {
                ret = objectMapper.writeValueAsString(teacherService.getTeachCourseAndClassListByTerm(termInfo, id));
            } catch (JsonProcessingException e) {
                e.printStackTrace();
            }
        }
        return ret;
    }

    // 获取成绩(教师排序用)
    @RequestMapping(value = "/getScoreDataTeacherView", method = RequestMethod.POST)
    public String getScoreDataTeacherView(@RequestBody CourseAndClassInfo courseAndClassInfo, HttpSession session) {
        String id = (String) session.getAttribute("id");
        String ret = "ILLEGAL";
        if (teacherService.checkIfTeacher(id)) {
            try {
                ret = objectMapper.writeValueAsString(teacherService.getScoreDataTeacherView(courseAndClassInfo));
            } catch (JsonProcessingException e) {
                e.printStackTrace();
            }
        }
        return ret;
    }

    // 获取成绩
    @RequestMapping(value = "/storeScores", method = RequestMethod.POST)
    public String storeScores(@RequestBody List<ScoreInfo> scoreInfos, HttpSession session) {
        String id = (String) session.getAttribute("id");
        String ret = "ILLEGAL";
        if (teacherService.checkIfTeacher(id)) {
            teacherService.storeScores(scoreInfos);
            ret = "SUCCESS";
        }
        return ret;
    }

    // 提交成绩
    @RequestMapping(value = "/submitScore", method = RequestMethod.POST)
    public String submitScore(@RequestBody ScoreInfo scoreInfo, HttpSession session) {
        String id = (String) session.getAttribute("id");
        String ret = "ILLEGAL";
        if (teacherService.checkIfTeacher(id)) {
            teacherService.submitScore(scoreInfo, id);
            ret = "SUCCESS";
        }
        return ret;
    }

    // 教师提交成绩
    @RequestMapping(value = "/isScoreSubmittedByTeacher", method = RequestMethod.POST)
    public String isScoreSubmittedByTeacher(@RequestBody CourseAndClassInfo courseAndClassInfo,
                                            HttpSession session) {
        String id = (String) session.getAttribute("id");
        String ret = "ILLEGAL";
        if (teacherService.checkIfTeacher(id)) {
            if (teacherService.isScoreSubmittedByTeacher(courseAndClassInfo)) {
                ret = "true";
            } else {
                ret = "false";
            }
        }
        return ret;
    }
}
