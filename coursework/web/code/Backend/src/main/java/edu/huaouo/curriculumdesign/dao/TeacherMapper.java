package edu.huaouo.curriculumdesign.dao;

import edu.huaouo.curriculumdesign.model.CourseAndClassInfo;
import edu.huaouo.curriculumdesign.model.ScoreInfo;
import edu.huaouo.curriculumdesign.model.TeacherInfo;
import edu.huaouo.curriculumdesign.model.TermInfo;
import org.apache.ibatis.annotations.*;
import org.apache.ibatis.mapping.StatementType;
import org.springframework.stereotype.Repository;

import java.util.List;

@Mapper
@Repository
public interface TeacherMapper {
    @Select("SELECT CASE WHEN COUNT(1) > 0 THEN 1 ELSE 0 END FROM yangzh_Teacher01 WHERE yzh_TeacherID01=#{id}")
    boolean isTeacher(String id);

    @Select("{CALL ModifyTeacherPassword(#{id}, #{newPassword})}")
    @Options(statementType = StatementType.CALLABLE)
    void modifyTeacherPassword(@Param("id") String id, @Param("newPassword") String newPassword);

    @Select("SELECT TOP 1 yzh_TeacherID01, yzh_TeacherName01, yzh_Sex01, yzh_Age01," +
            "yzh_Title01, yzh_Phone01 " +
            "FROM yangzh_Teacher01 " +
            "WHERE yzh_TeacherID01=#{id};")
    @Results({
            @Result(property = "teacherId", column = "yzh_TeacherID01"),
            @Result(property = "teacherName", column = "yzh_TeacherName01"),
            @Result(property = "sex", column = "yzh_Sex01"),
            @Result(property = "age", column = "yzh_Age01"),
            @Result(property = "title", column = "yzh_Title01"),
            @Result(property = "phone", column = "yzh_Phone01")

    })
    TeacherInfo getTeacherInfo(String id);

    @Select("SELECT yzh_CourseName01, yzh_ClassNumber01, yzh_MajorName01 " +
            "FROM yangzh_SetUpCourse01, yangzh_Course01 " +
            "WHERE yangzh_SetUpCourse01.yzh_CourseID01=yangzh_Course01.yzh_CourseID01 " +
            "AND yzh_AcademicYear01=#{termInfo.academicYear} " +
            "AND yzh_Term01=#{termInfo.term} " +
            "AND yzh_TeacherID01=#{id};")
    @Results({
            @Result(property = "courseName", column = "yzh_CourseName01"),
            @Result(property = "classNumber", column = "yzh_ClassNumber01"),
            @Result(property = "majorName", column = "yzh_MajorName01")
    })
    List<CourseAndClassInfo> getTeachCourseAndClassListByTerm(@Param("termInfo") TermInfo termInfo, @Param("id") String id);


    @Select("SELECT yangzh_Course01.yzh_CourseId01, yangzh_Student01.yzh_StudentId01, " +
            "yzh_StudentName01, yzh_Score01 " +
            "FROM yangzh_Student01, yangzh_Score01, yangzh_Course01 " +
            "WHERE yangzh_Score01.yzh_CourseId01=yangzh_Course01.yzh_CourseId01 " +
            "AND yangzh_Score01.yzh_StudentId01=yangzh_Student01.yzh_StudentId01 " +
            "AND yzh_CourseName01=#{courseName} " +
            "AND yzh_ClassNumber01=#{classNumber} " +
            "AND yzh_MajorName01=#{majorName} " +
            "ORDER BY yangzh_Student01.yzh_StudentId01 ASC;")
    @Results({
            @Result(property = "courseId", column = "yzh_CourseId01"),
            @Result(property = "studentId", column = "yzh_StudentId01"),
            @Result(property = "studentName", column = "yzh_StudentName01"),
            @Result(property = "score", column = "yzh_Score01")
    })
    List<ScoreInfo> getScoreDataTeacherView(CourseAndClassInfo courseAndClassInfo);

    @Update("UPDATE yangzh_Score01 SET yzh_Score01=#{score} " +
            "WHERE yzh_StudentID01= #{studentId} AND yzh_CourseID01=#{courseId} " +
            "IF @@ROWCOUNT=0 " +
            "INSERT INTO yangzh_Score01 VALUES (#{studentId}, #{courseId}, #{score});")
    void storeScore(ScoreInfo scoreInfo);

    @Update("UPDATE yangzh_SetUpCourse01 SET yzh_ScorePublic01=1 " +
            "WHERE yzh_CourseID01=#{scoreInfo.courseId} " +
            "AND yzh_TeacherID01=#{teacherId} " +
            "AND EXISTS " +
            "(SELECT * " +
            "FROM yangzh_Student01 " +
            "WHERE yzh_StudentID01=#{scoreInfo.studentId} " +
            "AND yangzh_SetUpCourse01.yzh_ClassNumber01=yangzh_Student01.yzh_ClassNumber01 " +
            "AND yangzh_SetUpCourse01.yzh_MajorName01=yangzh_Student01.yzh_MajorName01);")
    void submitScore(@Param("scoreInfo") ScoreInfo scoreInfo, @Param("teacherId") String teacherId);

    @Select("SELECT COUNT(1) " +
            "FROM yangzh_SetUpCourse01, yangzh_Course01 " +
            "WHERE yangzh_SetupCourse01.yzh_CourseId01=yangzh_Course01.yzh_CourseId01 " +
            "AND yzh_MajorName01=#{majorName} " +
            "AND yzh_ClassNumber01=#{classNumber} " +
            "AND yzh_CourseName01=#{courseName} " +
            "AND yzh_ScorePublic01=1;")
    int isScoreSubmittedByTeacher(CourseAndClassInfo courseAndClassInfo);
}