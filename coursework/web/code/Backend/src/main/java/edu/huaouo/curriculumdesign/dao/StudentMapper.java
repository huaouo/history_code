package edu.huaouo.curriculumdesign.dao;

import edu.huaouo.curriculumdesign.model.*;
import org.apache.ibatis.annotations.*;
import org.apache.ibatis.mapping.StatementType;
import org.springframework.stereotype.Repository;

import java.util.List;

@Mapper
@Repository
public interface StudentMapper {
    @Select("SELECT TOP 1 yzh_StudentID01, yzh_StudentName01, yzh_Credit01, yzh_Sex01, yzh_Age01," +
            "yzh_GPA01, yzh_ClassNumber01, yzh_MajorName01, yangzh_Student01.yzh_City01, yzh_Province01 " +
            "FROM yangzh_Student01, yangzh_Region01 " +
            "WHERE yzh_StudentID01=#{id} AND yangzh_Student01.yzh_City01=yangzh_Region01.yzh_City01;"
    )
    @Results({
            @Result(property = "studentId", column = "yzh_StudentID01"),
            @Result(property = "studentName", column = "yzh_StudentName01"),
            @Result(property = "credit", column = "yzh_Credit01"),
            @Result(property = "sex", column = "yzh_Sex01"),
            @Result(property = "age", column = "yzh_Age01"),
            @Result(property = "gpa", column = "yzh_GPA01"),
            @Result(property = "classNumber", column = "yzh_ClassNumber01"),
            @Result(property = "majorName", column = "yzh_MajorName01"),
            @Result(property = "city", column = "yzh_City01"),
            @Result(property = "province", column = "yzh_Province01")

    })
    StudentInfo getStudentInfo(String id);

    @Select("SELECT CASE WHEN COUNT(1) > 0 THEN 1 ELSE 0 END FROM yangzh_Student01 WHERE yzh_StudentID01=#{id}")
    boolean isStudent(String id);

    @Select("{CALL ModifyStudentPassword(#{id}, #{newPassword})}")
    @Options(statementType = StatementType.CALLABLE)
    void modifyStudentPassword(@Param("id") String id, @Param("newPassword") String newPassword);

    @Select("SELECT DISTINCT yzh_CourseName01, yzh_Score01, yzh_Credit01, yzh_Term01 " +
            "FROM yangzh_Score01, yangzh_Course01, yangzh_SetUpCourse01 " +
            "WHERE yangzh_Score01.yzh_CourseId01=yangzh_Course01.yzh_CourseId01 " +
            "AND yangzh_SetupCourse01.yzh_CourseId01=yangzh_Course01.yzh_CourseId01 " +
            "AND yzh_StudentId01=#{id} " +
            "AND yzh_AcademicYear01=#{termInfo.academicYear};"
    )
    @Results({
            @Result(property = "courseName", column = "yzh_CourseName01"),
            @Result(property = "score", column = "yzh_Score01"),
            @Result(property = "credit", column = "yzh_Credit01"),
            @Result(property = "term", column = "yzh_Term01")
    })
    List<StudentScoreInfo> getScoreByStudent(@Param("termInfo") TermInfo termInfo, @Param("id") String id);


    @Select("SELECT DISTINCT yzh_CourseName01, yzh_Score01, yzh_Credit01 " +
            "FROM yangzh_Score01, yangzh_Course01 " +
            "WHERE yangzh_Score01.yzh_CourseId01=yangzh_Course01.yzh_CourseId01 " +
            "AND yzh_StudentId01=#{id};"
    )
    @Results({
            @Result(property = "courseName", column = "yzh_CourseName01"),
            @Result(property = "score", column = "yzh_Score01"),
            @Result(property = "credit", column = "yzh_Credit01")
    })
    List<StudentScoreInfo> getAcademicStatus(String id);


    @Select("SELECT yzh_CourseName01, yzh_Time01, yzh_Requirement01, yzh_Credit01 " +
            "FROM yangzh_Course01, yangzh_SetUpCourse01 " +
            "WHERE yangzh_Course01.yzh_CourseId01=yangzh_SetUpCourse01.yzh_CourseId01 " +
            "AND yzh_ClassNumber01=#{classNumber} " +
            "AND yzh_MajorName01=#{majorName} " +
            "AND yzh_AcademicYear01=#{academicYear} " +
            "AND yzh_Term01=#{term};")
    @Results({
            @Result(property = "courseName", column = "yzh_CourseName01"),
            @Result(property = "time", column = "yzh_Time01"),
            @Result(property = "requirement", column = "yzh_Requirement01"),
            @Result(property = "credit", column = "yzh_Credit01")
    })
    List<CourseInfo> getCourseList(TermAndClassInfo termAndClassInfo);
}
