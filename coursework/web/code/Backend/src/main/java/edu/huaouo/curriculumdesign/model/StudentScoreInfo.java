package edu.huaouo.curriculumdesign.model;

public class StudentScoreInfo {
    private String courseName;
    private double score;
    private double credit;
    private char term;

    public String getCourseName() {
        return courseName;
    }

    public void setCourseName(String courseName) {
        this.courseName = courseName;
    }

    public double getScore() {
        return score;
    }

    public void setScore(double score) {
        this.score = score;
    }

    public double getCredit() {
        return credit;
    }

    public void setCredit(double credit) {
        this.credit = credit;
    }

    public char getTerm() {
        return term;
    }

    public void setTerm(char term) {
        this.term = term;
    }
}
