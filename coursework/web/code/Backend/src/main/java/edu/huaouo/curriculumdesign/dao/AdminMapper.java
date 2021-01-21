package edu.huaouo.curriculumdesign.dao;

import edu.huaouo.curriculumdesign.model.AdminInfo;
import edu.huaouo.curriculumdesign.model.LocationInfo;
import edu.huaouo.curriculumdesign.model.StudentInfo;
import edu.huaouo.curriculumdesign.model.TeacherInfo;
import org.apache.ibatis.annotations.*;
import org.apache.ibatis.mapping.StatementType;
import org.springframework.stereotype.Repository;

@Mapper
@Repository
public interface AdminMapper {
    @Select("SELECT CASE WHEN COUNT(1) > 0 THEN 1 ELSE 0 END FROM yangzh_Admin01 WHERE yzh_AdminID01=#{id}")
    boolean isAdmin(String id);

    @Select("{CALL ModifyAdminPassword(#{id}, #{newPassword})}")
    @Options(statementType = StatementType.CALLABLE)
    void modifyAdminPassword(@Param("id") String id, @Param("newPassword") String newPassword);

    @Select("SELECT COUNT(1) " +
            "FROM yangzh_Student01, yangzh_Region01 " +
            "WHERE yangzh_Student01.yzh_City01=yangzh_Region01.yzh_City01 " +
            "AND yzh_Province01 LIKE CONCAT(#{province},'%') " +
            "AND yangzh_Student01.yzh_City01 LIKE CONCAT(#{city}, '%');")
    int getStudentNumber(LocationInfo locationInfo);

    @Insert("{CALL AddStudent(#{studentId}, #{studentName}, 0, #{sex}, #{age}," +
            "0, #{classNumber}, #{majorName}, #{city}, '123456')}")
    @Options(statementType = StatementType.CALLABLE)
    int addStudent(StudentInfo studentInfo);

    @Insert("{CALL AddTeacher(#{teacherId}, #{teacherName}, #{sex}, #{age}, #{title}, #{phone}, '123456')}")
    @Options(statementType = StatementType.CALLABLE)
    int addTeacher(TeacherInfo teacherInfo);

    @Insert("{CALL AddAdmin(#{id}, '123456')}")
    @Options(statementType = StatementType.CALLABLE)
    int addAdmin(AdminInfo adminInfo);

    @Delete("DELETE FROM yangzh_Student01 WHERE yzh_StudentId01=#{id}")
    int deleteStudent(String id);

    @Delete("DELETE FROM yangzh_Teacher01 WHERE yzh_TeacherId01=#{id}")
    int deleteTeacher(String id);

    @Delete("DELETE FROM yangzh_Admin01 WHERE yzh_AdminId01=#{id}")
    int deleteAdmin(String id);
}
