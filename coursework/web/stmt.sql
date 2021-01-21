USE master;
DROP DATABASE IF EXISTS yangzhenhuaMIS01;
CREATE DATABASE yangzhenhuaMIS01;
GO ;

USE yangzhenhuaMIS01;
CREATE TABLE yangzh_Region01 (
  yzh_City01     NVARCHAR(15) PRIMARY KEY,
  yzh_Province01 NVARCHAR(15) NOT NULL
);

CREATE TABLE yangzh_Major01 (
  yzh_MajorName01 NVARCHAR(10) PRIMARY KEY
);

CREATE TABLE yangzh_Class01 (
  yzh_ClassNumber01 INT          NOT NULL,
  yzh_MajorName01   NVARCHAR(10) NOT NULL REFERENCES yangzh_Major01 (yzh_MajorName01),
  PRIMARY KEY (yzh_ClassNumber01, yzh_MajorName01)
);

CREATE TABLE yangzh_Student01 (
  yzh_StudentID01    NVARCHAR(20) PRIMARY KEY,
  yzh_StudentName01  NVARCHAR(20) NOT NULL,
  yzh_Credit01       FLOAT        NOT NULL,
  yzh_Sex01          NCHAR        NOT NULL CHECK (yzh_Sex01 IN ('��', 'Ů')),
  yzh_Age01          INT          NOT NULL,
  yzh_GPA01          FLOAT        NOT NULL,
  yzh_ClassNumber01  INT,
  yzh_MajorName01    NVARCHAR(10),
  yzh_City01         NVARCHAR(15) NOT NULL REFERENCES yangzh_Region01 (yzh_City01),
  yzh_PasswordHash01 NVARCHAR(64) NOT NULL,
  FOREIGN KEY (yzh_ClassNumber01, yzh_MajorName01) REFERENCES yangzh_Class01 (yzh_ClassNumber01, yzh_MajorName01)
);

CREATE TABLE yangzh_Course01 (
  yzh_CourseID01    NVARCHAR(10) PRIMARY KEY,
  yzh_CourseName01  NVARCHAR(25) NOT NULL,
  yzh_Time01        INT          NOT NULL,
  yzh_Requirement01 NCHAR(2) CHECK (yzh_Requirement01 in ('����', '����')),
  yzh_Credit01      FLOAT        NOT NULL,
);

CREATE TABLE yangzh_Teacher01 (
  yzh_TeacherID01    NVARCHAR(20) PRIMARY KEY,
  yzh_TeacherName01  NVARCHAR(20) NOT NULL,
  yzh_Sex01          NCHAR        NOT NULL CHECK (yzh_Sex01 IN ('��', 'Ů')),
  yzh_Age01          INT          NOT NULL,
  yzh_Title01        NVARCHAR(3) CHECK (yzh_Title01 IN ('����', '��ʦ', '������', '����')),
  yzh_Phone01        NVARCHAR(15),
  yzh_PasswordHash01 NVARCHAR(64) NOT NULL
);

CREATE TABLE yangzh_Score01 (
  yzh_StudentID01 NVARCHAR(20) REFERENCES yangzh_Student01 (yzh_StudentID01),
  yzh_CourseID01  NVARCHAR(10) REFERENCES yangzh_Course01 (yzh_CourseID01),
  yzh_Score01     FLOAT,
  PRIMARY KEY (yzh_StudentID01, yzh_CourseID01)
);

CREATE TABLE yangzh_SetUpCourse01 (
  yzh_ClassNumber01  INT,
  yzh_MajorName01    NVARCHAR(10),
  yzh_CourseID01     NVARCHAR(10) REFERENCES yangzh_Course01 (yzh_CourseID01),
  yzh_TeacherID01    NVARCHAR(20) REFERENCES yangzh_Teacher01 (yzh_TeacherID01),
  yzh_AcademicYear01 NVARCHAR(10),
  yzh_Term01         NCHAR,
  yzh_ScorePublic01  BIT,
  FOREIGN KEY (yzh_ClassNumber01, yzh_MajorName01) REFERENCES yangzh_Class01 (yzh_ClassNumber01, yzh_MajorName01),
  PRIMARY KEY (yzh_ClassNumber01, yzh_MajorName01, yzh_CourseID01, yzh_TeacherID01, yzh_AcademicYear01, yzh_Term01)
);

CREATE TABLE yangzh_Admin01 (
  yzh_AdminID01      NVARCHAR(20) PRIMARY KEY,
  yzh_PasswordHash01 NVARCHAR(64) NOT NULL
);

GO ;

CREATE VIEW yangzh_Auth01 (yzh_ID01, yzh_PasswordHash01) AS
  SELECT
    yzh_StudentID01,
    yzh_PasswordHash01
  FROM yangzh_Student01
  UNION
  SELECT
    yzh_TeacherID01,
    yzh_PasswordHash01
  FROM yangzh_Teacher01
  UNION
  SELECT
    yzh_AdminID01,
    yzh_PasswordHash01
  FROM yangzh_Admin01;

GO ;

CREATE INDEX StudentIndex
  ON yangzh_Student01(yzh_Credit01, yzh_GPA01);

GO;

CREATE INDEX CourseIndex
  ON yangzh_Course01(yzh_CourseName01, yzh_Credit01);

GO;

CREATE INDEX ScoreIndex
  ON yangzh_Score01(yzh_Score01);

GO;

CREATE PROCEDURE AddStudent
    @StudentID   NVARCHAR(20),
    @StudentName NVARCHAR(20),
    @Credit      FLOAT,
    @Sex         NCHAR,
    @Age         INT,
    @GPA         FLOAT,
    @ClassNumber INT,
    @MajorName   NVARCHAR(10),
    @City        NVARCHAR(15),
    @Password    NVARCHAR(50)
AS
  BEGIN
    INSERT INTO yangzh_Student01
    VALUES (@StudentID, @StudentName, @Credit, @Sex, @Age,
            @GPA, @ClassNumber, @MajorName, @City, HASHBYTES('SHA2_512', @Password))
  END

GO ;

CREATE PROCEDURE AddTeacher
    @TeacherID   NVARCHAR(20),
    @TeacherName NVARCHAR(20),
    @Sex         NCHAR,
    @Age         INT,
    @Title       NVARCHAR(3),
    @Phone       NVARCHAR(15),
    @Password    NVARCHAR(50)
AS
  BEGIN
    INSERT INTO yangzh_Teacher01
    VALUES (@TeacherID, @TeacherName, @Sex, @Age, @Title,
            @Phone, HASHBYTES('SHA2_512', @Password))
  END

GO ;

CREATE PROCEDURE AddAdmin
    @AdminID  NVARCHAR(20),
    @Password NVARCHAR(50)
AS
  BEGIN
    INSERT INTO yangzh_Admin01
    VALUES (@AdminID, HASHBYTES('SHA2_512', @Password))
  END

Go ;

CREATE PROCEDURE Login
    @ID              NVARCHAR(20),
    @Password        NVARCHAR(50),
    @ResponseMessage NVARCHAR(250) = '' OUTPUT
AS
  BEGIN
    SET NOCOUNT ON
    DECLARE @userID NVARCHAR(20)
    IF EXISTS(SELECT TOP 1 yzh_ID01
              FROM yangzh_Auth01
              WHERE yzh_ID01 = @ID)
      BEGIN
        SET @userID = (SELECT yzh_ID01
                       FROM yangzh_Auth01
                       WHERE yzh_ID01 = @ID AND
                             yzh_PasswordHash01 = HASHBYTES('SHA2_512', @Password))
        IF (@userID IS NULL)
          SET @ResponseMessage = 'Incorrect password'
        ELSE
          SET @ResponseMessage = 'Success'
      END
    ELSE
      SET @ResponseMessage = 'Invalid login'
  END

GO ;

CREATE PROCEDURE ModifyStudentPassword
    @StudentID NVARCHAR(20),
    @Password  NVARCHAR(50)
AS
  BEGIN
    UPDATE yangzh_Student01
    SET yzh_PasswordHash01 = HASHBYTES('SHA2_512', @Password)
    WHERE yzh_StudentID01 = @StudentID
  END

GO ;

CREATE PROCEDURE ModifyTeacherPassword
    @TeacherID NVARCHAR(20),
    @Password  NVARCHAR(50)
AS
  BEGIN
    UPDATE yangzh_Teacher01
    SET yzh_PasswordHash01 = HASHBYTES('SHA2_512', @Password)
    WHERE yzh_TeacherID01 = @TeacherID
  END

GO ;

CREATE PROCEDURE ModifyAdminPassword
    @AdminID  NVARCHAR(20),
    @Password NVARCHAR(50)
AS
  BEGIN
    UPDATE yangzh_Admin01
    SET yzh_PasswordHash01 = HASHBYTES('SHA2_512', @Password)
    WHERE yzh_AdminID01 = @AdminID
  END

GO ;

CREATE TRIGGER UpdateCredit
  ON yangzh_Score01
  FOR INSERT, UPDATE
AS
  BEGIN
    IF (SELECT COUNT(*)
        FROM inserted) != 0
      UPDATE yangzh_Student01
      SET yzh_Credit01 = (
        SELECT SUM(yzh_Credit01)
        FROM yangzh_Course01, yangzh_Score01
        WHERE yangzh_Course01.yzh_CourseID01 = yangzh_Score01.yzh_CourseID01
              AND yangzh_Score01.yzh_StudentID01 = (SELECT yzh_StudentID01
                                                    FROM inserted)
      )
  END

GO ;

CREATE TRIGGER UpdateGPA
  ON yangzh_Score01
  FOR INSERT, UPDATE
AS
  BEGIN
    IF (SELECT COUNT(*)
        FROM inserted) != 0
      UPDATE yangzh_Student01
      SET yzh_GPA01 = (
                        SELECT SUM(yzh_Credit01 * (0.1 * yzh_Score01 - 5))
                        FROM yangzh_Course01, yangzh_Score01
                        WHERE yangzh_Course01.yzh_CourseID01 = yangzh_Score01.yzh_CourseID01
                              AND yangzh_Score01.yzh_StudentID01 = (SELECT yzh_StudentID01
                                                                    FROM inserted)
                      ) / (
                        SELECT SUM(yzh_Credit01)
                        FROM yangzh_Course01, yangzh_Score01
                        WHERE yangzh_Course01.yzh_CourseID01 = yangzh_Score01.yzh_CourseID01
                              AND yangzh_Score01.yzh_StudentID01 = (SELECT yzh_StudentID01
                                                                    FROM inserted)
                      )
  END
GO ;
INSERT INTO yangzh_Region01 VALUES ('�Ϻ���', '�Ϻ���');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�ٲ���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ɽ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�������������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�º���徰����������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('ŭ��������������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ɽ׳������������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ͨ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�ն���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��������������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��Ϫ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ӹ���������������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��˫���ɴ���������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�������������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�����첼��', '���ɹ�������');
INSERT INTO yangzh_Region01 VALUES ('�ں���', '���ɹ�������');
INSERT INTO yangzh_Region01 VALUES ('�˰���', '���ɹ�������');
INSERT INTO yangzh_Region01 VALUES ('��ͷ��', '���ɹ�������');
INSERT INTO yangzh_Region01 VALUES ('���ױ�����', '���ɹ�������');
INSERT INTO yangzh_Region01 VALUES ('���ͺ�����', '���ɹ�������');
INSERT INTO yangzh_Region01 VALUES ('�����׶���', '���ɹ�������');
INSERT INTO yangzh_Region01 VALUES ('�����', '���ɹ�������');
INSERT INTO yangzh_Region01 VALUES ('ͨ����', '���ɹ�������');
INSERT INTO yangzh_Region01 VALUES ('������˹��', '���ɹ�������');
INSERT INTO yangzh_Region01 VALUES ('���ֹ�����', '���ɹ�������');
INSERT INTO yangzh_Region01 VALUES ('��������', '���ɹ�������');
INSERT INTO yangzh_Region01 VALUES ('������', '������');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ƽ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�ӱ߳�����������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ԭ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�׳���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ɽ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��Դ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('ͨ����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ɽ��', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('�ڽ���', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ɽ����������', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('�ϳ���', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('�˱���', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('��Ԫ��', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('�㰲��', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('�ɶ���', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('��֦����', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('���β���������', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('üɽ��', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('�Թ���', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('���Ӳ���Ǽ��������', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('�Ű���', '�Ĵ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('�����', '�����');
INSERT INTO yangzh_Region01 VALUES ('������', '���Ļ���������');
INSERT INTO yangzh_Region01 VALUES ('������', '���Ļ���������');
INSERT INTO yangzh_Region01 VALUES ('��ԭ��', '���Ļ���������');
INSERT INTO yangzh_Region01 VALUES ('ʯ��ɽ��', '���Ļ���������');
INSERT INTO yangzh_Region01 VALUES ('������', '���Ļ���������');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�Ϸ���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�ߺ���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('ͭ����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ɽ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ɽ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��Ӫ��', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ׯ��', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('̩����', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('�Ͳ���', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('Ϋ����', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('��̨��', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('�ĳ���', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('�ൺ��', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('�ٷ���', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ͬ��', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('̫ԭ��', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('˷����', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('�˳���', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('��Ȫ��', 'ɽ��ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ݸ��', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ɽ��', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('�Ƹ���', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ɽ��', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('÷����', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ͷ��', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('��β��', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('��Դ��', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('��Զ��', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('տ����', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('�麣��', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('ï����', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('�ع���', '�㶫ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����׳��������');
INSERT INTO yangzh_Region01 VALUES ('������', '����׳��������');
INSERT INTO yangzh_Region01 VALUES ('������', '����׳��������');
INSERT INTO yangzh_Region01 VALUES ('������', '����׳��������');
INSERT INTO yangzh_Region01 VALUES ('������', '����׳��������');
INSERT INTO yangzh_Region01 VALUES ('������', '����׳��������');
INSERT INTO yangzh_Region01 VALUES ('������', '����׳��������');
INSERT INTO yangzh_Region01 VALUES ('�ӳ���', '����׳��������');
INSERT INTO yangzh_Region01 VALUES ('������', '����׳��������');
INSERT INTO yangzh_Region01 VALUES ('��ɫ��', '����׳��������');
INSERT INTO yangzh_Region01 VALUES ('�����', '����׳��������');
INSERT INTO yangzh_Region01 VALUES ('������', '����׳��������');
INSERT INTO yangzh_Region01 VALUES ('������', '����׳��������');
INSERT INTO yangzh_Region01 VALUES ('���Ǹ���', '����׳��������');
INSERT INTO yangzh_Region01 VALUES ('��³ľ����', '�½�ά���������');
INSERT INTO yangzh_Region01 VALUES ('���������������', '�½�ά���������');
INSERT INTO yangzh_Region01 VALUES ('�������տ¶�����������', '�½�ά���������');
INSERT INTO yangzh_Region01 VALUES ('����������', '�½�ά���������');
INSERT INTO yangzh_Region01 VALUES ('���������ɹ�������', '�½�ά���������');
INSERT INTO yangzh_Region01 VALUES ('��³����', '�½�ά���������');
INSERT INTO yangzh_Region01 VALUES ('�������', '�½�ά���������');
INSERT INTO yangzh_Region01 VALUES ('������', '�½�ά���������');
INSERT INTO yangzh_Region01 VALUES ('��ʲ����', '�½�ά���������');
INSERT INTO yangzh_Region01 VALUES ('���ǵ���', '�½�ά���������');
INSERT INTO yangzh_Region01 VALUES ('���������ɹ�������', '�½�ά���������');
INSERT INTO yangzh_Region01 VALUES ('��������������', '�½�ά���������');
INSERT INTO yangzh_Region01 VALUES ('������ֱϽ�ؼ�������λ', '�½�ά���������');
INSERT INTO yangzh_Region01 VALUES ('�����յ���', '�½�ά���������');
INSERT INTO yangzh_Region01 VALUES ('����̩����', '�½�ά���������');
INSERT INTO yangzh_Region01 VALUES ('�Ͼ���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ͨ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��Ǩ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('̩����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�γ���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('���Ƹ���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�Ž���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�ϲ���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�˴���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('Ƽ����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('ӥ̶��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�ӱ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ɽ��', '�ӱ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('�ȷ���', '�ӱ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('�żҿ���', '�ӱ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('�е���', '�ӱ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�ӱ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('ʯ��ׯ��', '�ӱ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('�ػʵ���', '�ӱ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ˮ��', '�ӱ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('��̨��', '�ӱ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�ӱ�ʡ');
INSERT INTO yangzh_Region01 VALUES ('����Ͽ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�ܿ���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('ƽ��ɽ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('֣����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('פ�����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�ױ���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ˮ��', '�㽭ʡ');
INSERT INTO yangzh_Region01 VALUES ('̨����', '�㽭ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�㽭ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�㽭ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�㽭ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�㽭ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�㽭ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�㽭ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ɽ��', '�㽭ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�㽭ʡ');
INSERT INTO yangzh_Region01 VALUES ('����', '�㽭ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ɳ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('ʮ����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('Т����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�˲���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ʩ����������������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�人��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�Ƹ���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ʯ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�żҽ���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��̶��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��������������������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('¦����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ɳ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�����ر�������', '�����ر�������');
INSERT INTO yangzh_Region01 VALUES ('���Ļ���������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ˮ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('ƽ����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��Ҵ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('���ϲ���������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��Ȫ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('¤����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ƽ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('Ȫ����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('ɽ����', '����������');
INSERT INTO yangzh_Region01 VALUES ('������', '����������');
INSERT INTO yangzh_Region01 VALUES ('�տ�����', '����������');
INSERT INTO yangzh_Region01 VALUES ('������', '����������');
INSERT INTO yangzh_Region01 VALUES ('��֥��', '����������');
INSERT INTO yangzh_Region01 VALUES ('��������', '����������');
INSERT INTO yangzh_Region01 VALUES ('�������', '����������');
INSERT INTO yangzh_Region01 VALUES ('����ˮ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��˳��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�Ͻ���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('ͭ����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('ǭ�������嶱��������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('ǭ�ϲ���������������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('ǭ���ϲ���������������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��˳��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��Ϫ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�̽���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('Ӫ����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��«����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ɽ��', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '������');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�Ӱ���', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('μ����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('ͭ����', '����ʡ');
INSERT INTO yangzh_Region01 VALUES ('�������������', '�ຣʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�ຣʡ');
INSERT INTO yangzh_Region01 VALUES ('��������������', '�ຣʡ');
INSERT INTO yangzh_Region01 VALUES ('���ϲ���������', '�ຣʡ');
INSERT INTO yangzh_Region01 VALUES ('�����ɹ������������', '�ຣʡ');
INSERT INTO yangzh_Region01 VALUES ('��������������', '�ຣʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '�ຣʡ');
INSERT INTO yangzh_Region01 VALUES ('���ϲ���������', '�ຣʡ');
INSERT INTO yangzh_Region01 VALUES ('����ر�������', '����ر�������');
INSERT INTO yangzh_Region01 VALUES ('��̨����', '������ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '������ʡ');
INSERT INTO yangzh_Region01 VALUES ('��ľ˹��', '������ʡ');
INSERT INTO yangzh_Region01 VALUES ('˫Ѽɽ��', '������ʡ');
INSERT INTO yangzh_Region01 VALUES ('��������', '������ʡ');
INSERT INTO yangzh_Region01 VALUES ('���˰������', '������ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '������ʡ');
INSERT INTO yangzh_Region01 VALUES ('ĵ������', '������ʡ');
INSERT INTO yangzh_Region01 VALUES ('�绯��', '������ʡ');
INSERT INTO yangzh_Region01 VALUES ('������', '������ʡ');
INSERT INTO yangzh_Region01 VALUES ('�׸���', '������ʡ');
INSERT INTO yangzh_Region01 VALUES ('�ں���', '������ʡ');
INSERT INTO yangzh_Region01 VALUES ('���������', '������ʡ');

INSERT INTO yangzh_Major01 VALUES ('�������');
INSERT INTO yangzh_Major01 VALUES ('����ѧ');
INSERT INTO yangzh_Major01 VALUES ('�������ѧ�뼼��');

INSERT INTO yangzh_Class01 VALUES (1603, '�������');
INSERT INTO yangzh_Class01 VALUES (1604, '�������');
INSERT INTO yangzh_Class01 VALUES (1602, '����ѧ');
INSERT INTO yangzh_Class01 VALUES (1701, '�������ѧ�뼼��');

EXEC AddStudent '201626810301', '����', 0, '��', 18, 0, 1603, '�������', '��ͷ��', '123456';
EXEC AddStudent '201626810302', '�˿���', 0, '��', 20, 0, 1603, '�������', '��ƽ��', '123456';
EXEC AddStudent '201626810303', '���', 0, '��', 20, 0, 1603, '�������', '������', '123456';
EXEC AddStudent '201626810304', '����ɺ', 0, 'Ů', 18, 0, 1603, '�������', '�ӱ߳�����������', '123456';
EXEC AddStudent '201626810305', '������', 0, 'Ů', 22, 0, 1603, '�������', '���ϲ���������', '123456';
EXEC AddStudent '201626810306', '������', 0, 'Ů', 21, 0, 1603, '�������', '����Ͽ��', '123456';
EXEC AddStudent '201626810307', '������', 0, 'Ů', 20, 0, 1603, '�������', '������', '123456';
EXEC AddStudent '201626810308', '�����', 0, 'Ů', 20, 0, 1603, '�������', '������', '123456';
EXEC AddStudent '201626810309', '������', 0, '��', 21, 0, 1603, '�������', '������', '123456';
EXEC AddStudent '201626810310', '������', 0, '��', 18, 0, 1603, '�������', '��Ϫ��', '123456';
EXEC AddStudent '201626810311', '������', 0, '��', 18, 0, 1603, '�������', '�ȷ���', '123456';
EXEC AddStudent '201626810312', '��ٻ��', 0, 'Ů', 22, 0, 1603, '�������', '������', '123456';
EXEC AddStudent '201626810313', '�ָ�Դ', 0, '��', 19, 0, 1603, '�������', '��������', '123456';
EXEC AddStudent '201626810314', '������', 0, 'Ů', 20, 0, 1603, '�������', '������', '123456';
EXEC AddStudent '201626810315', '��ʫɺ', 0, 'Ů', 21, 0, 1603, '�������', '������', '123456';
EXEC AddStudent '201626810316', '���չ�', 0, '��', 22, 0, 1603, '�������', 'ǭ���ϲ���������������', '123456';
EXEC AddStudent '201626810317', '������', 0, 'Ů', 18, 0, 1603, '�������', '������', '123456';
EXEC AddStudent '201626810318', '�˹���', 0, 'Ů', 18, 0, 1603, '�������', '��ľ˹��', '123456';
EXEC AddStudent '201626810319', '����', 0, '��', 21, 0, 1603, '�������', '���ϲ���������', '123456';
EXEC AddStudent '201626810320', '�˹�½', 0, '��', 22, 0, 1603, '�������', '��ʯ��', '123456';
EXEC AddStudent '201626810321', '������', 0, 'Ů', 21, 0, 1603, '�������', '�ױ���', '123456';
EXEC AddStudent '201626810322', '��ΰ��', 0, '��', 18, 0, 1603, '�������', '����������', '123456';
EXEC AddStudent '201626810323', '���', 0, 'Ů', 19, 0, 1603, '�������', '������', '123456';
EXEC AddStudent '201626810324', '�����', 0, '��', 21, 0, 1603, '�������', '��«����', '123456';
EXEC AddStudent '201626810325', '����ɺ', 0, 'Ů', 21, 0, 1603, '�������', '�����', '123456';
EXEC AddStudent '201626810326', '�ʾ���', 0, '��', 21, 0, 1603, '�������', '������', '123456';
EXEC AddStudent '201626810327', '����ǿ', 0, 'Ů', 21, 0, 1603, '�������', '������', '123456';
EXEC AddStudent '201626810328', '��ӳƼ', 0, 'Ů', 20, 0, 1603, '�������', '������', '123456';
EXEC AddStudent '201626810329', '�����', 0, 'Ů', 19, 0, 1603, '�������', '��˳��', '123456';
EXEC AddStudent '201626810330', '������', 0, 'Ů', 20, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201626810401', '�ʴȷ�', 0, 'Ů', 22, 0, 1604, '�������', '�ߺ���', '123456';
EXEC AddStudent '201626810402', '������', 0, 'Ů', 18, 0, 1604, '�������', '��ɽ��', '123456';
EXEC AddStudent '201626810403', '������', 0, '��', 18, 0, 1604, '�������', '�Ϻ���', '123456';
EXEC AddStudent '201626810404', '��־Ȩ', 0, '��', 19, 0, 1604, '�������', '¤����', '123456';
EXEC AddStudent '201626810405', '����', 0, 'Ů', 21, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201626810406', '������', 0, '��', 19, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201626810407', '������', 0, '��', 20, 0, 1604, '�������', '��˳��', '123456';
EXEC AddStudent '201626810408', '�ƿ�', 0, '��', 20, 0, 1604, '�������', '��������', '123456';
EXEC AddStudent '201626810409', '������', 0, 'Ů', 18, 0, 1604, '�������', '��֦����', '123456';
EXEC AddStudent '201626810410', '������', 0, 'Ů', 19, 0, 1604, '�������', 'ͭ����', '123456';
EXEC AddStudent '201626810411', '��¾�', 0, '��', 18, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201626810412', '������', 0, 'Ů', 19, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201626810413', '��ΰ��', 0, '��', 18, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201626810414', '��־��', 0, '��', 19, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201626810415', '������', 0, 'Ů', 20, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201626810416', '���', 0, 'Ů', 18, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201626810417', '�˻ݳ�', 0, '��', 19, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201626810418', '��ѩ��', 0, '��', 19, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201626810419', '������', 0, '��', 20, 0, 1604, '�������', 'μ����', '123456';
EXEC AddStudent '201626810420', '����ϲ', 0, 'Ů', 22, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201626810421', '��ʯӢ', 0, 'Ů', 18, 0, 1604, '�������', '�������', '123456';
EXEC AddStudent '201626810422', '������', 0, 'Ů', 19, 0, 1604, '�������', '̨����', '123456';
EXEC AddStudent '201626810423', '������', 0, 'Ů', 22, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201626810424', '֣����', 0, 'Ů', 20, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201626810425', '��־��', 0, '��', 21, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201626810426', '������', 0, 'Ů', 22, 0, 1604, '�������', 'ĵ������', '123456';
EXEC AddStudent '201626810427', '������', 0, 'Ů', 18, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201626810428', '��С��', 0, 'Ů', 20, 0, 1604, '�������', '���������ɹ�������', '123456';
EXEC AddStudent '201626810429', '��ΰ��', 0, '��', 18, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201626810430', '��ïƽ', 0, '��', 22, 0, 1604, '�������', '������', '123456';
EXEC AddStudent '201602070201', '����', 0, '��', 19, 0, 1602, '����ѧ', '�Ƹ���', '123456';
EXEC AddStudent '201602070202', '��ף��', 0, '��', 19, 0, 1602, '����ѧ', 'Ϋ����', '123456';
EXEC AddStudent '201602070203', '����Ӣ', 0, 'Ů', 20, 0, 1602, '����ѧ', '���ǵ���', '123456';
EXEC AddStudent '201602070204', '��־��', 0, '��', 22, 0, 1602, '����ѧ', '������', '123456';
EXEC AddStudent '201602070205', '������', 0, '��', 20, 0, 1602, '����ѧ', '������', '123456';
EXEC AddStudent '201602070206', '������', 0, '��', 18, 0, 1602, '����ѧ', '�Ű���', '123456';
EXEC AddStudent '201602070207', '��ѩ��', 0, '��', 22, 0, 1602, '����ѧ', '������', '123456';
EXEC AddStudent '201602070208', '���̾�', 0, '��', 18, 0, 1602, '����ѧ', '��ɽ��', '123456';
EXEC AddStudent '201602070209', '���Ǵ�', 0, '��', 21, 0, 1602, '����ѧ', '��ͬ��', '123456';
EXEC AddStudent '201602070210', '������', 0, 'Ů', 22, 0, 1602, '����ѧ', '������', '123456';
EXEC AddStudent '201602070211', '����ϼ', 0, 'Ů', 22, 0, 1602, '����ѧ', '������', '123456';
EXEC AddStudent '201602070212', '��־��', 0, '��', 22, 0, 1602, '����ѧ', '������', '123456';
EXEC AddStudent '201602070213', '������', 0, '��', 20, 0, 1602, '����ѧ', '������', '123456';
EXEC AddStudent '201602070214', '����', 0, 'Ů', 22, 0, 1602, '����ѧ', '������', '123456';
EXEC AddStudent '201602070215', '������', 0, '��', 21, 0, 1602, '����ѧ', '��ɽ��', '123456';
EXEC AddStudent '201602070216', '���¾�', 0, '��', 20, 0, 1602, '����ѧ', '�����', '123456';
EXEC AddStudent '201602070217', '�����', 0, 'Ů', 20, 0, 1602, '����ѧ', 'ǭ���ϲ���������������', '123456';
EXEC AddStudent '201602070218', '������', 0, 'Ů', 19, 0, 1602, '����ѧ', '������', '123456';
EXEC AddStudent '201602070219', '�����', 0, 'Ů', 19, 0, 1602, '����ѧ', 'Ȫ����', '123456';
EXEC AddStudent '201602070220', '����ƽ', 0, 'Ů', 20, 0, 1602, '����ѧ', '������', '123456';
EXEC AddStudent '201706060101', '�����', 0, '��', 18, 0, 1701, '�������ѧ�뼼��', '������', '123456';
EXEC AddStudent '201706060102', '���', 0, 'Ů', 18, 0, 1701, '�������ѧ�뼼��', '�������������', '123456';
EXEC AddStudent '201706060103', '���Ķ�', 0, '��', 18, 0, 1701, '�������ѧ�뼼��', '�˱���', '123456';
EXEC AddStudent '201706060104', '��Х��', 0, '��', 22, 0, 1701, '�������ѧ�뼼��', '������', '123456';
EXEC AddStudent '201706060105', '��Զ��', 0, '��', 19, 0, 1701, '�������ѧ�뼼��', '��ݸ��', '123456';
EXEC AddStudent '201706060106', '���', 0, '��', 22, 0, 1701, '�������ѧ�뼼��', '��������������', '123456';
EXEC AddStudent '201706060107', '��־��', 0, '��', 22, 0, 1701, '�������ѧ�뼼��', '������', '123456';
EXEC AddStudent '201706060108', '������', 0, 'Ů', 20, 0, 1701, '�������ѧ�뼼��', '���Ļ���������', '123456';
EXEC AddStudent '201706060109', '�����', 0, 'Ů', 21, 0, 1701, '�������ѧ�뼼��', '��ԭ��', '123456';
EXEC AddStudent '201706060110', '������', 0, '��', 22, 0, 1701, '�������ѧ�뼼��', '������', '123456';
EXEC AddStudent '201706060111', '���޷�', 0, 'Ů', 20, 0, 1701, '�������ѧ�뼼��', '������', '123456';
EXEC AddStudent '201706060112', '��ʤ��', 0, '��', 19, 0, 1701, '�������ѧ�뼼��', '������', '123456';
EXEC AddStudent '201706060113', '���Ļ�', 0, 'Ů', 21, 0, 1701, '�������ѧ�뼼��', '������', '123456';
EXEC AddStudent '201706060114', '����ͨ', 0, '��', 21, 0, 1701, '�������ѧ�뼼��', '������', '123456';
EXEC AddStudent '201706060115', '�Ź���', 0, '��', 21, 0, 1701, '�������ѧ�뼼��', '������', '123456';
EXEC AddStudent '201706060116', '���ɷ�', 0, '��', 21, 0, 1701, '�������ѧ�뼼��', '��ɽ����������', '123456';
EXEC AddStudent '201706060117', '����', 0, '��', 21, 0, 1701, '�������ѧ�뼼��', '������', '123456';
EXEC AddStudent '201706060118', '������', 0, '��', 19, 0, 1701, '�������ѧ�뼼��', '������', '123456';
EXEC AddStudent '201706060119', '��ΰ��', 0, '��', 20, 0, 1701, '�������ѧ�뼼��', 'פ�����', '123456';
EXEC AddStudent '201706060120', '������', 0, '��', 18, 0, 1701, '�������ѧ�뼼��', '������', '123456';

INSERT INTO yangzh_Course01 VALUES ('C01', '��������', 64, '����', 4);
INSERT INTO yangzh_Course01 VALUES ('C02', '˼���������ʵ��', 32, '����', 2);
INSERT INTO yangzh_Course01 VALUES ('C03', '�ߵ���ѧ', 64, '����', 4);
INSERT INTO yangzh_Course01 VALUES ('C04', '���Դ���', 32, '����', 2);
INSERT INTO yangzh_Course01 VALUES ('C05', '��ѧ����', 64, '����', 4);
INSERT INTO yangzh_Course01 VALUES ('C06', '��ѧ����ʵ��', 32, '����', 2);

EXEC AddTeacher 'T01', '�Űٸ�', '��', 35, '��ʦ', '13888888888', '123456';
EXEC AddTeacher 'T02', '������', 'Ů', 42, '������', '18868686868', '123456';
EXEC AddTeacher 'T03', '���Ķ�', '��', 52, '����', '15656565656', '123456';
EXEC AddTeacher 'T04', '������', 'Ů', 45, '������', '13728282828', '123456';
EXEC AddTeacher 'T05', '���', '��', 43, '����', '13523452345', '123456';

INSERT INTO yangzh_SetUpCourse01 VALUES (1603, '�������', 'C01', 'T01', '2016-2017', '2', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1604, '�������', 'C01', 'T01', '2016-2017', '2', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1603, '�������', 'C05', 'T05', '2016-2017', '2', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1603, '�������', 'C06', 'T05', '2016-2017', '2', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1602, '����ѧ', 'C01', 'T01', '2016-2017', '2', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1603, '�������', 'C02', 'T04', '2016-2017', '��', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1701, '�������ѧ�뼼��', 'C03', 'T02', '2017-2018', '1', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1603, '�������', 'C04', 'T02', '2017-2018', '1', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1602, '����ѧ', 'C04', 'T02', '2017-2018', '1', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1604, '�������', 'C04', 'T03', '2017-2018', '1', 0);

EXEC AddAdmin 'admin', '123456';