package edu.huaouo.curriculumdesign.service;

import edu.huaouo.curriculumdesign.dao.AdminMapper;
import edu.huaouo.curriculumdesign.model.AdminInfo;
import edu.huaouo.curriculumdesign.model.LocationInfo;
import edu.huaouo.curriculumdesign.model.StudentInfo;
import edu.huaouo.curriculumdesign.model.TeacherInfo;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
public class AdminService {
    private final AdminMapper adminMapper;

    @Autowired
    public AdminService(AdminMapper adminMapper) {
        this.adminMapper = adminMapper;
    }

    // 检查是否管理员
    public boolean checkIfAdmin(String id) {
        return adminMapper.isAdmin(id);
    }

    // 获取学号
    public String getStudentNumber(LocationInfo locationInfo) {
        return Integer.toString(adminMapper.getStudentNumber(locationInfo));
    }

    // 添加学生
    public String addStudent(StudentInfo studentInfo) {
        if (adminMapper.addStudent(studentInfo) == 1) {
            return "SUCCESS";
        } else {
            return "FAILURE";
        }
    }

    // 添加教师
    public String addTeacher(TeacherInfo teacherInfo) {
        if (adminMapper.addTeacher(teacherInfo) == 1) {
            return "SUCCESS";
        } else {
            return "FAILURE";
        }
    }

    // 添加管理员
    public String addAdmin(AdminInfo adminInfo) {
        if (adminMapper.addAdmin(adminInfo) == 1) {
            return "SUCCESS";
        } else {
            return "FAILURE";
        }
    }

    // 删除用户
    public String deleteUser(String id) {
        if (adminMapper.deleteStudent(id) == 1
                || adminMapper.deleteTeacher(id) == 1
                || adminMapper.deleteAdmin(id) == 1) {
            return "SUCCESS";
        } else {
            return "FAILURE";
        }
    }
}
