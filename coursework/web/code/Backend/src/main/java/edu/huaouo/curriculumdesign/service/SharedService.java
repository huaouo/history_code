package edu.huaouo.curriculumdesign.service;

import edu.huaouo.curriculumdesign.dao.AdminMapper;
import edu.huaouo.curriculumdesign.dao.SharedMapper;
import edu.huaouo.curriculumdesign.dao.StudentMapper;
import edu.huaouo.curriculumdesign.dao.TeacherMapper;
import edu.huaouo.curriculumdesign.model.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class SharedService {
    private final SharedMapper sharedMapper;
    private final AdminMapper adminMapper;
    private final TeacherMapper teacherMapper;
    private final StudentMapper studentMapper;

    @Autowired
    public SharedService(SharedMapper sharedMapper, AdminMapper adminMapper,
                         TeacherMapper teacherMapper, StudentMapper studentMapper) {
        this.sharedMapper = sharedMapper;
        this.adminMapper = adminMapper;
        this.teacherMapper = teacherMapper;
        this.studentMapper = studentMapper;
    }

    // 检查用户名、密码是否和数据库中记录相匹配
    public boolean matchIdAndPassword(LoginInfo info) {
        Message message = new Message();
        sharedMapper.matchIdAndPassword(info, message);
        return message.getDetail().equals("Success");
    }

    // 进行登录
    public UserType login(LoginInfo info) {
        if (matchIdAndPassword(info)) {
            if (adminMapper.isAdmin(info.getId())) {
                return UserType.ADMIN;
            } else if (teacherMapper.isTeacher(info.getId())) {
                return UserType.TEACHER;
            } else {
                return UserType.STUDENT;
            }
        } else {
            return UserType.ILLEGAL;
        }
    }

    // 修改密码
    public boolean modifyPassword(String id, UserType type, String newPassword) {
        switch (type) {
            case ADMIN:
                adminMapper.modifyAdminPassword(id, newPassword);
                return true;
            case TEACHER:
                teacherMapper.modifyTeacherPassword(id, newPassword);
                return true;
            case STUDENT:
                studentMapper.modifyStudentPassword(id, newPassword);
                return true;
            default:
                return false;
        }
    }

    // 获取学期列表
    public List<TermInfo> getTermList() {
        return sharedMapper.getTermList();
    }

    // 获取班级列表
    public List<CourseAndClassInfo> getClassList() {
        return sharedMapper.getClassList();
    }
}
