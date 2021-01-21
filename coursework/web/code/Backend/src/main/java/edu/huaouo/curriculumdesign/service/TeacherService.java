package edu.huaouo.curriculumdesign.service;

import edu.huaouo.curriculumdesign.dao.SharedMapper;
import edu.huaouo.curriculumdesign.dao.TeacherMapper;
import edu.huaouo.curriculumdesign.model.CourseAndClassInfo;
import edu.huaouo.curriculumdesign.model.ScoreInfo;
import edu.huaouo.curriculumdesign.model.TeacherInfo;
import edu.huaouo.curriculumdesign.model.TermInfo;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class TeacherService {
    private final TeacherMapper teacherMapper;
    private final SharedMapper sharedMapper;

    @Autowired
    public TeacherService(TeacherMapper teacherMapper, SharedMapper sharedMapper) {
        this.teacherMapper = teacherMapper;
        this.sharedMapper = sharedMapper;
    }

    // 获取教师信息
    public TeacherInfo getTeacherInfo(String id) {
        return teacherMapper.getTeacherInfo(id);
    }

    // 检查是否是教师
    public boolean checkIfTeacher(String id) {
        return teacherMapper.isTeacher(id);
    }

    // 获取课程和班级
    public List<CourseAndClassInfo> getTeachCourseAndClassListByTerm(TermInfo termInfo, String id) {
        return teacherMapper.getTeachCourseAndClassListByTerm(termInfo, id);
    }

    // 获取成绩
    public List<ScoreInfo> getScoreDataTeacherView(CourseAndClassInfo courseAndClassInfo) {
        List<ScoreInfo> emptyScore = sharedMapper.getClassMembersWithEmptyScore(
                courseAndClassInfo.getMajorName(),
                courseAndClassInfo.getClassNumber());
        List<ScoreInfo> validScore = teacherMapper.getScoreDataTeacherView(courseAndClassInfo);
        String courseId = sharedMapper.getCourseIdByName(courseAndClassInfo.getCourseName());
        if (emptyScore.size() == validScore.size()) {
            return validScore;
        } else {
            for (ScoreInfo eScore : emptyScore) {
                eScore.setCourseId(courseId);
            }
            for (ScoreInfo vScore : validScore) {
                for (ScoreInfo eScore: emptyScore) {
                    if (vScore.getStudentId().equals(eScore.getStudentId())) {
                        eScore.setScore(vScore.getScore());
                        break;
                    }
                }
            }
            return emptyScore;
        }
    }

    // 暂存成绩
    public void storeScores(List<ScoreInfo> scoreInfos) {
        for (ScoreInfo i : scoreInfos) {
            teacherMapper.storeScore(i);
        }
    }

    // 提交成绩
    public void submitScore(ScoreInfo scoreInfo, String teacherId) {
        teacherMapper.submitScore(scoreInfo, teacherId);
    }

    // 检查成绩是否被提交
    public boolean isScoreSubmittedByTeacher(CourseAndClassInfo courseAndClassInfo) {
        return teacherMapper.isScoreSubmittedByTeacher(courseAndClassInfo) == 1;
    }
}
