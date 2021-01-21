package edu.huaouo.curriculumdesign.service;

import edu.huaouo.curriculumdesign.dao.StudentMapper;
import edu.huaouo.curriculumdesign.model.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class StudentService {
    private final StudentMapper studentMapper;

    @Autowired
    public StudentService(StudentMapper studentMapper) {
        this.studentMapper = studentMapper;
    }

    // 获取学生信息
    public StudentInfo getStudentInfo(String id) {
        return studentMapper.getStudentInfo(id);
    }

    // 检查是否学生
    public boolean checkIfStudent(String id) {
        return studentMapper.isStudent(id);
    }

    // 根据学生获得成绩
    public List<StudentScoreInfo> getScoreByStudent(TermInfo termInfo, String id) {
        return studentMapper.getScoreByStudent(termInfo, id);
    }

    // 获得学业状态
    public List<StudentScoreInfo> getAcademicStatus(String id) {
        return studentMapper.getAcademicStatus(id);
    }

    // 获取课程列表
    public List<CourseInfo> getCourseList(TermAndClassInfo termAndClassInfo) {
        return studentMapper.getCourseList(termAndClassInfo);
    }
}
