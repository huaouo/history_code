package edu.huaouo.curriculumdesign.model;

public class TermAndClassInfo {
    private String academicYear;
    private char term;
    private String majorName;
    private int classNumber;

    public String getAcademicYear() {
        return academicYear;
    }

    public void setAcademicYear(String academicYear) {
        this.academicYear = academicYear;
    }

    public char getTerm() {
        return term;
    }

    public void setTerm(char term) {
        this.term = term;
    }

    public String getMajorName() {
        return majorName;
    }

    public void setMajorName(String majorName) {
        this.majorName = majorName;
    }

    public int getClassNumber() {
        return classNumber;
    }

    public void setClassNumber(int classNumber) {
        this.classNumber = classNumber;
    }
}
