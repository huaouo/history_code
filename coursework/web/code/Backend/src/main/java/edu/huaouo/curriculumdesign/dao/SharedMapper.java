package edu.huaouo.curriculumdesign.dao;

import edu.huaouo.curriculumdesign.model.*;
import org.apache.ibatis.annotations.*;
import org.apache.ibatis.mapping.StatementType;
import org.springframework.stereotype.Repository;

import java.util.List;

@Mapper
@Repository
public interface SharedMapper {
    @Select("{CALL Login(#{info.id}, #{info.password}, #{message.detail, mode=OUT, jdbcType=VARCHAR})}")
    @Options(statementType = StatementType.CALLABLE)
    void matchIdAndPassword(@Param("info") LoginInfo info, @Param("message") Message message);

    @Select("SELECT DISTINCT yzh_AcademicYear01, yzh_Term01 FROM yangzh_SetUpCourse01;")
    @Results({
            @Result(property = "academicYear", column = "yzh_AcademicYear01"),
            @Result(property = "term", column = "yzh_Term01")
    })
    List<TermInfo> getTermList();

    @Select("SELECT yzh_StudentID01, yzh_StudentName01 " +
            "FROM yangzh_Student01 " +
            "WHERE yzh_MajorName01=#{majorName} " +
            "AND yzh_ClassNumber01=#{classNumber};")
    @Results({
            @Result(property = "studentId", column = "yzh_StudentID01"),
            @Result(property = "studentName", column = "yzh_StudentName01")
    })
    List<ScoreInfo> getClassMembersWithEmptyScore(@Param("majorName") String majorName, @Param("classNumber") int classNumber);

    @Select("SELECT yzh_CourseID01 FROM yangzh_Course01 WHERE yzh_CourseName01=#{name}")
    String getCourseIdByName(String name);

    @Select("SELECT yzh_ClassNumber01, yzh_MajorName01 " +
            "FROM yangzh_Class01;")
    @Results({
            @Result(property = "classNumber", column = "yzh_ClassNumber01"),
            @Result(property = "majorName", column = "yzh_MajorName01")
    })
    List<CourseAndClassInfo> getClassList();
}