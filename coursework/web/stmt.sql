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
  yzh_Sex01          NCHAR        NOT NULL CHECK (yzh_Sex01 IN ('男', '女')),
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
  yzh_Requirement01 NCHAR(2) CHECK (yzh_Requirement01 in ('考试', '考查')),
  yzh_Credit01      FLOAT        NOT NULL,
);

CREATE TABLE yangzh_Teacher01 (
  yzh_TeacherID01    NVARCHAR(20) PRIMARY KEY,
  yzh_TeacherName01  NVARCHAR(20) NOT NULL,
  yzh_Sex01          NCHAR        NOT NULL CHECK (yzh_Sex01 IN ('男', '女')),
  yzh_Age01          INT          NOT NULL,
  yzh_Title01        NVARCHAR(3) CHECK (yzh_Title01 IN ('助教', '讲师', '副教授', '教授')),
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
INSERT INTO yangzh_Region01 VALUES ('上海市', '上海市');
INSERT INTO yangzh_Region01 VALUES ('丽江市', '云南省');
INSERT INTO yangzh_Region01 VALUES ('临沧市', '云南省');
INSERT INTO yangzh_Region01 VALUES ('保山市', '云南省');
INSERT INTO yangzh_Region01 VALUES ('大理白族自治州', '云南省');
INSERT INTO yangzh_Region01 VALUES ('德宏傣族景颇族自治州', '云南省');
INSERT INTO yangzh_Region01 VALUES ('怒江傈僳族自治州', '云南省');
INSERT INTO yangzh_Region01 VALUES ('文山壮族苗族自治州', '云南省');
INSERT INTO yangzh_Region01 VALUES ('昆明市', '云南省');
INSERT INTO yangzh_Region01 VALUES ('昭通市', '云南省');
INSERT INTO yangzh_Region01 VALUES ('普洱市', '云南省');
INSERT INTO yangzh_Region01 VALUES ('曲靖市', '云南省');
INSERT INTO yangzh_Region01 VALUES ('楚雄彝族自治州', '云南省');
INSERT INTO yangzh_Region01 VALUES ('玉溪市', '云南省');
INSERT INTO yangzh_Region01 VALUES ('红河哈尼族彝族自治州', '云南省');
INSERT INTO yangzh_Region01 VALUES ('西双版纳傣族自治州', '云南省');
INSERT INTO yangzh_Region01 VALUES ('迪庆藏族自治州', '云南省');
INSERT INTO yangzh_Region01 VALUES ('乌兰察布市', '内蒙古自治区');
INSERT INTO yangzh_Region01 VALUES ('乌海市', '内蒙古自治区');
INSERT INTO yangzh_Region01 VALUES ('兴安盟', '内蒙古自治区');
INSERT INTO yangzh_Region01 VALUES ('包头市', '内蒙古自治区');
INSERT INTO yangzh_Region01 VALUES ('呼伦贝尔市', '内蒙古自治区');
INSERT INTO yangzh_Region01 VALUES ('呼和浩特市', '内蒙古自治区');
INSERT INTO yangzh_Region01 VALUES ('巴彦淖尔市', '内蒙古自治区');
INSERT INTO yangzh_Region01 VALUES ('赤峰市', '内蒙古自治区');
INSERT INTO yangzh_Region01 VALUES ('通辽市', '内蒙古自治区');
INSERT INTO yangzh_Region01 VALUES ('鄂尔多斯市', '内蒙古自治区');
INSERT INTO yangzh_Region01 VALUES ('锡林郭勒盟', '内蒙古自治区');
INSERT INTO yangzh_Region01 VALUES ('阿拉善盟', '内蒙古自治区');
INSERT INTO yangzh_Region01 VALUES ('北京市', '北京市');
INSERT INTO yangzh_Region01 VALUES ('吉林市', '吉林省');
INSERT INTO yangzh_Region01 VALUES ('四平市', '吉林省');
INSERT INTO yangzh_Region01 VALUES ('延边朝鲜族自治州', '吉林省');
INSERT INTO yangzh_Region01 VALUES ('松原市', '吉林省');
INSERT INTO yangzh_Region01 VALUES ('白城市', '吉林省');
INSERT INTO yangzh_Region01 VALUES ('白山市', '吉林省');
INSERT INTO yangzh_Region01 VALUES ('辽源市', '吉林省');
INSERT INTO yangzh_Region01 VALUES ('通化市', '吉林省');
INSERT INTO yangzh_Region01 VALUES ('长春市', '吉林省');
INSERT INTO yangzh_Region01 VALUES ('乐山市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('内江市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('凉山彝族自治州', '四川省');
INSERT INTO yangzh_Region01 VALUES ('南充市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('宜宾市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('巴中市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('广元市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('广安市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('德阳市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('成都市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('攀枝花市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('泸州市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('甘孜藏族自治州', '四川省');
INSERT INTO yangzh_Region01 VALUES ('眉山市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('绵阳市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('自贡市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('资阳市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('达州市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('遂宁市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('阿坝藏族羌族自治州', '四川省');
INSERT INTO yangzh_Region01 VALUES ('雅安市', '四川省');
INSERT INTO yangzh_Region01 VALUES ('天津市', '天津市');
INSERT INTO yangzh_Region01 VALUES ('中卫市', '宁夏回族自治区');
INSERT INTO yangzh_Region01 VALUES ('吴忠市', '宁夏回族自治区');
INSERT INTO yangzh_Region01 VALUES ('固原市', '宁夏回族自治区');
INSERT INTO yangzh_Region01 VALUES ('石嘴山市', '宁夏回族自治区');
INSERT INTO yangzh_Region01 VALUES ('银川市', '宁夏回族自治区');
INSERT INTO yangzh_Region01 VALUES ('亳州市', '安徽省');
INSERT INTO yangzh_Region01 VALUES ('六安市', '安徽省');
INSERT INTO yangzh_Region01 VALUES ('合肥市', '安徽省');
INSERT INTO yangzh_Region01 VALUES ('安庆市', '安徽省');
INSERT INTO yangzh_Region01 VALUES ('宣城市', '安徽省');
INSERT INTO yangzh_Region01 VALUES ('宿州市', '安徽省');
INSERT INTO yangzh_Region01 VALUES ('池州市', '安徽省');
INSERT INTO yangzh_Region01 VALUES ('淮北市', '安徽省');
INSERT INTO yangzh_Region01 VALUES ('淮南市', '安徽省');
INSERT INTO yangzh_Region01 VALUES ('滁州市', '安徽省');
INSERT INTO yangzh_Region01 VALUES ('芜湖市', '安徽省');
INSERT INTO yangzh_Region01 VALUES ('蚌埠市', '安徽省');
INSERT INTO yangzh_Region01 VALUES ('铜陵市', '安徽省');
INSERT INTO yangzh_Region01 VALUES ('阜阳市', '安徽省');
INSERT INTO yangzh_Region01 VALUES ('马鞍山市', '安徽省');
INSERT INTO yangzh_Region01 VALUES ('黄山市', '安徽省');
INSERT INTO yangzh_Region01 VALUES ('东营市', '山东省');
INSERT INTO yangzh_Region01 VALUES ('临沂市', '山东省');
INSERT INTO yangzh_Region01 VALUES ('威海市', '山东省');
INSERT INTO yangzh_Region01 VALUES ('德州市', '山东省');
INSERT INTO yangzh_Region01 VALUES ('日照市', '山东省');
INSERT INTO yangzh_Region01 VALUES ('枣庄市', '山东省');
INSERT INTO yangzh_Region01 VALUES ('泰安市', '山东省');
INSERT INTO yangzh_Region01 VALUES ('济南市', '山东省');
INSERT INTO yangzh_Region01 VALUES ('济宁市', '山东省');
INSERT INTO yangzh_Region01 VALUES ('淄博市', '山东省');
INSERT INTO yangzh_Region01 VALUES ('滨州市', '山东省');
INSERT INTO yangzh_Region01 VALUES ('潍坊市', '山东省');
INSERT INTO yangzh_Region01 VALUES ('烟台市', '山东省');
INSERT INTO yangzh_Region01 VALUES ('聊城市', '山东省');
INSERT INTO yangzh_Region01 VALUES ('莱芜市', '山东省');
INSERT INTO yangzh_Region01 VALUES ('菏泽市', '山东省');
INSERT INTO yangzh_Region01 VALUES ('青岛市', '山东省');
INSERT INTO yangzh_Region01 VALUES ('临汾市', '山西省');
INSERT INTO yangzh_Region01 VALUES ('吕梁市', '山西省');
INSERT INTO yangzh_Region01 VALUES ('大同市', '山西省');
INSERT INTO yangzh_Region01 VALUES ('太原市', '山西省');
INSERT INTO yangzh_Region01 VALUES ('忻州市', '山西省');
INSERT INTO yangzh_Region01 VALUES ('晋中市', '山西省');
INSERT INTO yangzh_Region01 VALUES ('晋城市', '山西省');
INSERT INTO yangzh_Region01 VALUES ('朔州市', '山西省');
INSERT INTO yangzh_Region01 VALUES ('运城市', '山西省');
INSERT INTO yangzh_Region01 VALUES ('长治市', '山西省');
INSERT INTO yangzh_Region01 VALUES ('阳泉市', '山西省');
INSERT INTO yangzh_Region01 VALUES ('东莞市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('中山市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('云浮市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('佛山市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('广州市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('惠州市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('揭阳市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('梅州市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('汕头市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('汕尾市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('江门市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('河源市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('深圳市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('清远市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('湛江市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('潮州市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('珠海市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('肇庆市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('茂名市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('阳江市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('韶关市', '广东省');
INSERT INTO yangzh_Region01 VALUES ('北海市', '广西壮族自治区');
INSERT INTO yangzh_Region01 VALUES ('南宁市', '广西壮族自治区');
INSERT INTO yangzh_Region01 VALUES ('崇左市', '广西壮族自治区');
INSERT INTO yangzh_Region01 VALUES ('来宾市', '广西壮族自治区');
INSERT INTO yangzh_Region01 VALUES ('柳州市', '广西壮族自治区');
INSERT INTO yangzh_Region01 VALUES ('桂林市', '广西壮族自治区');
INSERT INTO yangzh_Region01 VALUES ('梧州市', '广西壮族自治区');
INSERT INTO yangzh_Region01 VALUES ('河池市', '广西壮族自治区');
INSERT INTO yangzh_Region01 VALUES ('玉林市', '广西壮族自治区');
INSERT INTO yangzh_Region01 VALUES ('百色市', '广西壮族自治区');
INSERT INTO yangzh_Region01 VALUES ('贵港市', '广西壮族自治区');
INSERT INTO yangzh_Region01 VALUES ('贺州市', '广西壮族自治区');
INSERT INTO yangzh_Region01 VALUES ('钦州市', '广西壮族自治区');
INSERT INTO yangzh_Region01 VALUES ('防城港市', '广西壮族自治区');
INSERT INTO yangzh_Region01 VALUES ('乌鲁木齐市', '新疆维吾尔自治区');
INSERT INTO yangzh_Region01 VALUES ('伊犁哈萨克自治州', '新疆维吾尔自治区');
INSERT INTO yangzh_Region01 VALUES ('克孜勒苏柯尔克孜自治州', '新疆维吾尔自治区');
INSERT INTO yangzh_Region01 VALUES ('克拉玛依市', '新疆维吾尔自治区');
INSERT INTO yangzh_Region01 VALUES ('博尔塔拉蒙古自治州', '新疆维吾尔自治区');
INSERT INTO yangzh_Region01 VALUES ('吐鲁番市', '新疆维吾尔自治区');
INSERT INTO yangzh_Region01 VALUES ('和田地区', '新疆维吾尔自治区');
INSERT INTO yangzh_Region01 VALUES ('哈密市', '新疆维吾尔自治区');
INSERT INTO yangzh_Region01 VALUES ('喀什地区', '新疆维吾尔自治区');
INSERT INTO yangzh_Region01 VALUES ('塔城地区', '新疆维吾尔自治区');
INSERT INTO yangzh_Region01 VALUES ('巴音郭楞蒙古自治州', '新疆维吾尔自治区');
INSERT INTO yangzh_Region01 VALUES ('昌吉回族自治州', '新疆维吾尔自治区');
INSERT INTO yangzh_Region01 VALUES ('自治区直辖县级行政单位', '新疆维吾尔自治区');
INSERT INTO yangzh_Region01 VALUES ('阿克苏地区', '新疆维吾尔自治区');
INSERT INTO yangzh_Region01 VALUES ('阿勒泰地区', '新疆维吾尔自治区');
INSERT INTO yangzh_Region01 VALUES ('南京市', '江苏省');
INSERT INTO yangzh_Region01 VALUES ('南通市', '江苏省');
INSERT INTO yangzh_Region01 VALUES ('宿迁市', '江苏省');
INSERT INTO yangzh_Region01 VALUES ('常州市', '江苏省');
INSERT INTO yangzh_Region01 VALUES ('徐州市', '江苏省');
INSERT INTO yangzh_Region01 VALUES ('扬州市', '江苏省');
INSERT INTO yangzh_Region01 VALUES ('无锡市', '江苏省');
INSERT INTO yangzh_Region01 VALUES ('泰州市', '江苏省');
INSERT INTO yangzh_Region01 VALUES ('淮安市', '江苏省');
INSERT INTO yangzh_Region01 VALUES ('盐城市', '江苏省');
INSERT INTO yangzh_Region01 VALUES ('苏州市', '江苏省');
INSERT INTO yangzh_Region01 VALUES ('连云港市', '江苏省');
INSERT INTO yangzh_Region01 VALUES ('镇江市', '江苏省');
INSERT INTO yangzh_Region01 VALUES ('上饶市', '江西省');
INSERT INTO yangzh_Region01 VALUES ('九江市', '江西省');
INSERT INTO yangzh_Region01 VALUES ('南昌市', '江西省');
INSERT INTO yangzh_Region01 VALUES ('吉安市', '江西省');
INSERT INTO yangzh_Region01 VALUES ('宜春市', '江西省');
INSERT INTO yangzh_Region01 VALUES ('抚州市', '江西省');
INSERT INTO yangzh_Region01 VALUES ('新余市', '江西省');
INSERT INTO yangzh_Region01 VALUES ('景德镇市', '江西省');
INSERT INTO yangzh_Region01 VALUES ('萍乡市', '江西省');
INSERT INTO yangzh_Region01 VALUES ('赣州市', '江西省');
INSERT INTO yangzh_Region01 VALUES ('鹰潭市', '江西省');
INSERT INTO yangzh_Region01 VALUES ('保定市', '河北省');
INSERT INTO yangzh_Region01 VALUES ('唐山市', '河北省');
INSERT INTO yangzh_Region01 VALUES ('廊坊市', '河北省');
INSERT INTO yangzh_Region01 VALUES ('张家口市', '河北省');
INSERT INTO yangzh_Region01 VALUES ('承德市', '河北省');
INSERT INTO yangzh_Region01 VALUES ('沧州市', '河北省');
INSERT INTO yangzh_Region01 VALUES ('石家庄市', '河北省');
INSERT INTO yangzh_Region01 VALUES ('秦皇岛市', '河北省');
INSERT INTO yangzh_Region01 VALUES ('衡水市', '河北省');
INSERT INTO yangzh_Region01 VALUES ('邢台市', '河北省');
INSERT INTO yangzh_Region01 VALUES ('邯郸市', '河北省');
INSERT INTO yangzh_Region01 VALUES ('三门峡市', '河南省');
INSERT INTO yangzh_Region01 VALUES ('信阳市', '河南省');
INSERT INTO yangzh_Region01 VALUES ('南阳市', '河南省');
INSERT INTO yangzh_Region01 VALUES ('周口市', '河南省');
INSERT INTO yangzh_Region01 VALUES ('商丘市', '河南省');
INSERT INTO yangzh_Region01 VALUES ('安阳市', '河南省');
INSERT INTO yangzh_Region01 VALUES ('平顶山市', '河南省');
INSERT INTO yangzh_Region01 VALUES ('开封市', '河南省');
INSERT INTO yangzh_Region01 VALUES ('新乡市', '河南省');
INSERT INTO yangzh_Region01 VALUES ('洛阳市', '河南省');
INSERT INTO yangzh_Region01 VALUES ('漯河市', '河南省');
INSERT INTO yangzh_Region01 VALUES ('濮阳市', '河南省');
INSERT INTO yangzh_Region01 VALUES ('焦作市', '河南省');
INSERT INTO yangzh_Region01 VALUES ('许昌市', '河南省');
INSERT INTO yangzh_Region01 VALUES ('郑州市', '河南省');
INSERT INTO yangzh_Region01 VALUES ('驻马店市', '河南省');
INSERT INTO yangzh_Region01 VALUES ('鹤壁市', '河南省');
INSERT INTO yangzh_Region01 VALUES ('丽水市', '浙江省');
INSERT INTO yangzh_Region01 VALUES ('台州市', '浙江省');
INSERT INTO yangzh_Region01 VALUES ('嘉兴市', '浙江省');
INSERT INTO yangzh_Region01 VALUES ('宁波市', '浙江省');
INSERT INTO yangzh_Region01 VALUES ('杭州市', '浙江省');
INSERT INTO yangzh_Region01 VALUES ('温州市', '浙江省');
INSERT INTO yangzh_Region01 VALUES ('湖州市', '浙江省');
INSERT INTO yangzh_Region01 VALUES ('绍兴市', '浙江省');
INSERT INTO yangzh_Region01 VALUES ('舟山市', '浙江省');
INSERT INTO yangzh_Region01 VALUES ('衢州市', '浙江省');
INSERT INTO yangzh_Region01 VALUES ('金华市', '浙江省');
INSERT INTO yangzh_Region01 VALUES ('三亚市', '海南省');
INSERT INTO yangzh_Region01 VALUES ('三沙市', '海南省');
INSERT INTO yangzh_Region01 VALUES ('儋州市', '海南省');
INSERT INTO yangzh_Region01 VALUES ('海口市', '海南省');
INSERT INTO yangzh_Region01 VALUES ('十堰市', '湖北省');
INSERT INTO yangzh_Region01 VALUES ('咸宁市', '湖北省');
INSERT INTO yangzh_Region01 VALUES ('孝感市', '湖北省');
INSERT INTO yangzh_Region01 VALUES ('宜昌市', '湖北省');
INSERT INTO yangzh_Region01 VALUES ('恩施土家族苗族自治州', '湖北省');
INSERT INTO yangzh_Region01 VALUES ('武汉市', '湖北省');
INSERT INTO yangzh_Region01 VALUES ('荆州市', '湖北省');
INSERT INTO yangzh_Region01 VALUES ('荆门市', '湖北省');
INSERT INTO yangzh_Region01 VALUES ('襄阳市', '湖北省');
INSERT INTO yangzh_Region01 VALUES ('鄂州市', '湖北省');
INSERT INTO yangzh_Region01 VALUES ('随州市', '湖北省');
INSERT INTO yangzh_Region01 VALUES ('黄冈市', '湖北省');
INSERT INTO yangzh_Region01 VALUES ('黄石市', '湖北省');
INSERT INTO yangzh_Region01 VALUES ('岳阳市', '湖南省');
INSERT INTO yangzh_Region01 VALUES ('常德市', '湖南省');
INSERT INTO yangzh_Region01 VALUES ('张家界市', '湖南省');
INSERT INTO yangzh_Region01 VALUES ('怀化市', '湖南省');
INSERT INTO yangzh_Region01 VALUES ('株洲市', '湖南省');
INSERT INTO yangzh_Region01 VALUES ('永州市', '湖南省');
INSERT INTO yangzh_Region01 VALUES ('湘潭市', '湖南省');
INSERT INTO yangzh_Region01 VALUES ('湘西土家族苗族自治州', '湖南省');
INSERT INTO yangzh_Region01 VALUES ('益阳市', '湖南省');
INSERT INTO yangzh_Region01 VALUES ('娄底市', '湖南省');
INSERT INTO yangzh_Region01 VALUES ('衡阳市', '湖南省');
INSERT INTO yangzh_Region01 VALUES ('邵阳市', '湖南省');
INSERT INTO yangzh_Region01 VALUES ('郴州市', '湖南省');
INSERT INTO yangzh_Region01 VALUES ('长沙市', '湖南省');
INSERT INTO yangzh_Region01 VALUES ('澳门特别行政区', '澳门特别行政区');
INSERT INTO yangzh_Region01 VALUES ('临夏回族自治州', '甘肃省');
INSERT INTO yangzh_Region01 VALUES ('兰州市', '甘肃省');
INSERT INTO yangzh_Region01 VALUES ('嘉峪关市', '甘肃省');
INSERT INTO yangzh_Region01 VALUES ('天水市', '甘肃省');
INSERT INTO yangzh_Region01 VALUES ('定西市', '甘肃省');
INSERT INTO yangzh_Region01 VALUES ('平凉市', '甘肃省');
INSERT INTO yangzh_Region01 VALUES ('庆阳市', '甘肃省');
INSERT INTO yangzh_Region01 VALUES ('张掖市', '甘肃省');
INSERT INTO yangzh_Region01 VALUES ('武威市', '甘肃省');
INSERT INTO yangzh_Region01 VALUES ('甘南藏族自治州', '甘肃省');
INSERT INTO yangzh_Region01 VALUES ('白银市', '甘肃省');
INSERT INTO yangzh_Region01 VALUES ('酒泉市', '甘肃省');
INSERT INTO yangzh_Region01 VALUES ('金昌市', '甘肃省');
INSERT INTO yangzh_Region01 VALUES ('陇南市', '甘肃省');
INSERT INTO yangzh_Region01 VALUES ('三明市', '福建省');
INSERT INTO yangzh_Region01 VALUES ('南平市', '福建省');
INSERT INTO yangzh_Region01 VALUES ('厦门市', '福建省');
INSERT INTO yangzh_Region01 VALUES ('宁德市', '福建省');
INSERT INTO yangzh_Region01 VALUES ('泉州市', '福建省');
INSERT INTO yangzh_Region01 VALUES ('漳州市', '福建省');
INSERT INTO yangzh_Region01 VALUES ('福州市', '福建省');
INSERT INTO yangzh_Region01 VALUES ('莆田市', '福建省');
INSERT INTO yangzh_Region01 VALUES ('龙岩市', '福建省');
INSERT INTO yangzh_Region01 VALUES ('山南市', '西藏自治区');
INSERT INTO yangzh_Region01 VALUES ('拉萨市', '西藏自治区');
INSERT INTO yangzh_Region01 VALUES ('日喀则市', '西藏自治区');
INSERT INTO yangzh_Region01 VALUES ('昌都市', '西藏自治区');
INSERT INTO yangzh_Region01 VALUES ('林芝市', '西藏自治区');
INSERT INTO yangzh_Region01 VALUES ('那曲地区', '西藏自治区');
INSERT INTO yangzh_Region01 VALUES ('阿里地区', '西藏自治区');
INSERT INTO yangzh_Region01 VALUES ('六盘水市', '贵州省');
INSERT INTO yangzh_Region01 VALUES ('安顺市', '贵州省');
INSERT INTO yangzh_Region01 VALUES ('毕节市', '贵州省');
INSERT INTO yangzh_Region01 VALUES ('贵阳市', '贵州省');
INSERT INTO yangzh_Region01 VALUES ('遵义市', '贵州省');
INSERT INTO yangzh_Region01 VALUES ('铜仁市', '贵州省');
INSERT INTO yangzh_Region01 VALUES ('黔东南苗族侗族自治州', '贵州省');
INSERT INTO yangzh_Region01 VALUES ('黔南布依族苗族自治州', '贵州省');
INSERT INTO yangzh_Region01 VALUES ('黔西南布依族苗族自治州', '贵州省');
INSERT INTO yangzh_Region01 VALUES ('丹东市', '辽宁省');
INSERT INTO yangzh_Region01 VALUES ('大连市', '辽宁省');
INSERT INTO yangzh_Region01 VALUES ('抚顺市', '辽宁省');
INSERT INTO yangzh_Region01 VALUES ('朝阳市', '辽宁省');
INSERT INTO yangzh_Region01 VALUES ('本溪市', '辽宁省');
INSERT INTO yangzh_Region01 VALUES ('沈阳市', '辽宁省');
INSERT INTO yangzh_Region01 VALUES ('盘锦市', '辽宁省');
INSERT INTO yangzh_Region01 VALUES ('营口市', '辽宁省');
INSERT INTO yangzh_Region01 VALUES ('葫芦岛市', '辽宁省');
INSERT INTO yangzh_Region01 VALUES ('辽阳市', '辽宁省');
INSERT INTO yangzh_Region01 VALUES ('铁岭市', '辽宁省');
INSERT INTO yangzh_Region01 VALUES ('锦州市', '辽宁省');
INSERT INTO yangzh_Region01 VALUES ('阜新市', '辽宁省');
INSERT INTO yangzh_Region01 VALUES ('鞍山市', '辽宁省');
INSERT INTO yangzh_Region01 VALUES ('重庆市', '重庆市');
INSERT INTO yangzh_Region01 VALUES ('咸阳市', '陕西省');
INSERT INTO yangzh_Region01 VALUES ('商洛市', '陕西省');
INSERT INTO yangzh_Region01 VALUES ('安康市', '陕西省');
INSERT INTO yangzh_Region01 VALUES ('宝鸡市', '陕西省');
INSERT INTO yangzh_Region01 VALUES ('延安市', '陕西省');
INSERT INTO yangzh_Region01 VALUES ('榆林市', '陕西省');
INSERT INTO yangzh_Region01 VALUES ('汉中市', '陕西省');
INSERT INTO yangzh_Region01 VALUES ('渭南市', '陕西省');
INSERT INTO yangzh_Region01 VALUES ('西安市', '陕西省');
INSERT INTO yangzh_Region01 VALUES ('铜川市', '陕西省');
INSERT INTO yangzh_Region01 VALUES ('果洛藏族自治州', '青海省');
INSERT INTO yangzh_Region01 VALUES ('海东市', '青海省');
INSERT INTO yangzh_Region01 VALUES ('海北藏族自治州', '青海省');
INSERT INTO yangzh_Region01 VALUES ('海南藏族自治州', '青海省');
INSERT INTO yangzh_Region01 VALUES ('海西蒙古族藏族自治州', '青海省');
INSERT INTO yangzh_Region01 VALUES ('玉树藏族自治州', '青海省');
INSERT INTO yangzh_Region01 VALUES ('西宁市', '青海省');
INSERT INTO yangzh_Region01 VALUES ('黄南藏族自治州', '青海省');
INSERT INTO yangzh_Region01 VALUES ('香港特别行政区', '香港特别行政区');
INSERT INTO yangzh_Region01 VALUES ('七台河市', '黑龙江省');
INSERT INTO yangzh_Region01 VALUES ('伊春市', '黑龙江省');
INSERT INTO yangzh_Region01 VALUES ('佳木斯市', '黑龙江省');
INSERT INTO yangzh_Region01 VALUES ('双鸭山市', '黑龙江省');
INSERT INTO yangzh_Region01 VALUES ('哈尔滨市', '黑龙江省');
INSERT INTO yangzh_Region01 VALUES ('大兴安岭地区', '黑龙江省');
INSERT INTO yangzh_Region01 VALUES ('大庆市', '黑龙江省');
INSERT INTO yangzh_Region01 VALUES ('牡丹江市', '黑龙江省');
INSERT INTO yangzh_Region01 VALUES ('绥化市', '黑龙江省');
INSERT INTO yangzh_Region01 VALUES ('鸡西市', '黑龙江省');
INSERT INTO yangzh_Region01 VALUES ('鹤岗市', '黑龙江省');
INSERT INTO yangzh_Region01 VALUES ('黑河市', '黑龙江省');
INSERT INTO yangzh_Region01 VALUES ('齐齐哈尔市', '黑龙江省');

INSERT INTO yangzh_Major01 VALUES ('软件工程');
INSERT INTO yangzh_Major01 VALUES ('金融学');
INSERT INTO yangzh_Major01 VALUES ('计算机科学与技术');

INSERT INTO yangzh_Class01 VALUES (1603, '软件工程');
INSERT INTO yangzh_Class01 VALUES (1604, '软件工程');
INSERT INTO yangzh_Class01 VALUES (1602, '金融学');
INSERT INTO yangzh_Class01 VALUES (1701, '计算机科学与技术');

EXEC AddStudent '201626810301', '潘鹏', 0, '男', 18, 0, 1603, '软件工程', '汕头市', '123456';
EXEC AddStudent '201626810302', '潘俊威', 0, '男', 20, 0, 1603, '软件工程', '四平市', '123456';
EXEC AddStudent '201626810303', '张睿波', 0, '男', 20, 0, 1603, '软件工程', '广州市', '123456';
EXEC AddStudent '201626810304', '张晓珊', 0, '女', 18, 0, 1603, '软件工程', '延边朝鲜族自治州', '123456';
EXEC AddStudent '201626810305', '张利珍', 0, '女', 22, 0, 1603, '软件工程', '甘南藏族自治州', '123456';
EXEC AddStudent '201626810306', '张柳珍', 0, '女', 21, 0, 1603, '软件工程', '三门峡市', '123456';
EXEC AddStudent '201626810307', '张宇云', 0, '女', 20, 0, 1603, '软件工程', '荆门市', '123456';
EXEC AddStudent '201626810308', '张秀君', 0, '女', 20, 0, 1603, '软件工程', '定西市', '123456';
EXEC AddStudent '201626810309', '张宇良', 0, '男', 21, 0, 1603, '软件工程', '惠州市', '123456';
EXEC AddStudent '201626810310', '林启航', 0, '男', 18, 0, 1603, '软件工程', '本溪市', '123456';
EXEC AddStudent '201626810311', '林万兴', 0, '男', 18, 0, 1603, '软件工程', '廊坊市', '123456';
EXEC AddStudent '201626810312', '林倩怡', 0, '女', 22, 0, 1603, '软件工程', '鄂州市', '123456';
EXEC AddStudent '201626810313', '林富源', 0, '男', 19, 0, 1603, '软件工程', '景德镇市', '123456';
EXEC AddStudent '201626810314', '林丽敏', 0, '女', 20, 0, 1603, '软件工程', '咸宁市', '123456';
EXEC AddStudent '201626810315', '林诗珊', 0, '女', 21, 0, 1603, '软件工程', '衢州市', '123456';
EXEC AddStudent '201626810316', '邓日光', 0, '男', 22, 0, 1603, '软件工程', '黔西南布依族苗族自治州', '123456';
EXEC AddStudent '201626810317', '邓丽娜', 0, '女', 18, 0, 1603, '软件工程', '丽江市', '123456';
EXEC AddStudent '201626810318', '邓桂玲', 0, '女', 18, 0, 1603, '软件工程', '佳木斯市', '123456';
EXEC AddStudent '201626810319', '邓振华', 0, '男', 21, 0, 1603, '软件工程', '黄南藏族自治州', '123456';
EXEC AddStudent '201626810320', '邓广陆', 0, '男', 22, 0, 1603, '软件工程', '黄石市', '123456';
EXEC AddStudent '201626810321', '邓孟娜', 0, '女', 21, 0, 1603, '软件工程', '鹤壁市', '123456';
EXEC AddStudent '201626810322', '李伟城', 0, '男', 18, 0, 1603, '软件工程', '克拉玛依市', '123456';
EXEC AddStudent '201626810323', '李蔼玲', 0, '女', 19, 0, 1603, '软件工程', '郴州市', '123456';
EXEC AddStudent '201626810324', '李慈祥', 0, '男', 21, 0, 1603, '软件工程', '葫芦岛市', '123456';
EXEC AddStudent '201626810325', '李丽珊', 0, '女', 21, 0, 1603, '软件工程', '许昌市', '123456';
EXEC AddStudent '201626810326', '甘敬衡', 0, '男', 21, 0, 1603, '软件工程', '杭州市', '123456';
EXEC AddStudent '201626810327', '甘柳强', 0, '女', 21, 0, 1603, '软件工程', '安庆市', '123456';
EXEC AddStudent '201626810328', '甘映萍', 0, '女', 20, 0, 1603, '软件工程', '荆州市', '123456';
EXEC AddStudent '201626810329', '甘燕芬', 0, '女', 19, 0, 1603, '软件工程', '安顺市', '123456';
EXEC AddStudent '201626810330', '甘燕婷', 0, '女', 20, 0, 1604, '软件工程', '安庆市', '123456';
EXEC AddStudent '201626810401', '甘慈芳', 0, '女', 22, 0, 1604, '软件工程', '芜湖市', '123456';
EXEC AddStudent '201626810402', '甘晓琪', 0, '女', 18, 0, 1604, '软件工程', '白山市', '123456';
EXEC AddStudent '201626810403', '陈铎林', 0, '男', 18, 0, 1604, '软件工程', '上海市', '123456';
EXEC AddStudent '201626810404', '陈志权', 0, '男', 19, 0, 1604, '软件工程', '陇南市', '123456';
EXEC AddStudent '201626810405', '陈琳', 0, '女', 21, 0, 1604, '软件工程', '资阳市', '123456';
EXEC AddStudent '201626810406', '赖家文', 0, '男', 19, 0, 1604, '软件工程', '丽江市', '123456';
EXEC AddStudent '201626810407', '赖威佑', 0, '男', 20, 0, 1604, '软件工程', '抚顺市', '123456';
EXEC AddStudent '201626810408', '黄凯', 0, '男', 20, 0, 1604, '软件工程', '嘉峪关市', '123456';
EXEC AddStudent '201626810409', '罗文丽', 0, '女', 18, 0, 1604, '软件工程', '攀枝花市', '123456';
EXEC AddStudent '201626810410', '练子琪', 0, '女', 19, 0, 1604, '软件工程', '铜陵市', '123456';
EXEC AddStudent '201626810411', '俞德军', 0, '男', 18, 0, 1604, '软件工程', '焦作市', '123456';
EXEC AddStudent '201626810412', '杜燕珍', 0, '女', 19, 0, 1604, '软件工程', '北京市', '123456';
EXEC AddStudent '201626810413', '钟伟达', 0, '男', 18, 0, 1604, '软件工程', '扬州市', '123456';
EXEC AddStudent '201626810414', '曾志豪', 0, '男', 19, 0, 1604, '软件工程', '贺州市', '123456';
EXEC AddStudent '201626810415', '香紫琪', 0, '女', 20, 0, 1604, '软件工程', '福州市', '123456';
EXEC AddStudent '201626810416', '李妙华', 0, '女', 18, 0, 1604, '软件工程', '柳州市', '123456';
EXEC AddStudent '201626810417', '潘惠城', 0, '男', 19, 0, 1604, '软件工程', '福州市', '123456';
EXEC AddStudent '201626810418', '张雪彬', 0, '男', 19, 0, 1604, '软件工程', '宿州市', '123456';
EXEC AddStudent '201626810419', '陈雄林', 0, '男', 20, 0, 1604, '软件工程', '渭南市', '123456';
EXEC AddStudent '201626810420', '邓玉喜', 0, '女', 22, 0, 1604, '软件工程', '赣州市', '123456';
EXEC AddStudent '201626810421', '钟石英', 0, '女', 18, 0, 1604, '软件工程', '和田地区', '123456';
EXEC AddStudent '201626810422', '陈敏聪', 0, '女', 19, 0, 1604, '软件工程', '台州市', '123456';
EXEC AddStudent '201626810423', '潘依华', 0, '女', 22, 0, 1604, '软件工程', '肇庆市', '123456';
EXEC AddStudent '201626810424', '郑慧玉', 0, '女', 20, 0, 1604, '软件工程', '苏州市', '123456';
EXEC AddStudent '201626810425', '张志军', 0, '男', 21, 0, 1604, '软件工程', '荆州市', '123456';
EXEC AddStudent '201626810426', '甘念珠', 0, '女', 22, 0, 1604, '软件工程', '牡丹江市', '123456';
EXEC AddStudent '201626810427', '张敏聪', 0, '女', 18, 0, 1604, '软件工程', '崇左市', '123456';
EXEC AddStudent '201626810428', '潘小容', 0, '女', 20, 0, 1604, '软件工程', '巴音郭楞蒙古自治州', '123456';
EXEC AddStudent '201626810429', '甘伟容', 0, '男', 18, 0, 1604, '软件工程', '苏州市', '123456';
EXEC AddStudent '201626810430', '杜茂平', 0, '男', 22, 0, 1604, '软件工程', '丹东市', '123456';
EXEC AddStudent '201602070201', '钟力', 0, '男', 19, 0, 1602, '金融学', '黄冈市', '123456';
EXEC AddStudent '201602070202', '蓝祝生', 0, '男', 19, 0, 1602, '金融学', '潍坊市', '123456';
EXEC AddStudent '201602070203', '张柳英', 0, '女', 20, 0, 1602, '金融学', '塔城地区', '123456';
EXEC AddStudent '201602070204', '潘志威', 0, '男', 22, 0, 1602, '金融学', '阜新市', '123456';
EXEC AddStudent '201602070205', '罗岳流', 0, '男', 20, 0, 1602, '金融学', '江门市', '123456';
EXEC AddStudent '201602070206', '刘文浪', 0, '男', 18, 0, 1602, '金融学', '雅安市', '123456';
EXEC AddStudent '201602070207', '李雪辉', 0, '男', 22, 0, 1602, '金融学', '常德市', '123456';
EXEC AddStudent '201602070208', '蓝颂军', 0, '男', 18, 0, 1602, '金融学', '佛山市', '123456';
EXEC AddStudent '201602070209', '邓智达', 0, '男', 21, 0, 1602, '金融学', '大同市', '123456';
EXEC AddStudent '201602070210', '张宇清', 0, '女', 22, 0, 1602, '金融学', '吉林市', '123456';
EXEC AddStudent '201602070211', '张月霞', 0, '女', 22, 0, 1602, '金融学', '白银市', '123456';
EXEC AddStudent '201602070212', '林志能', 0, '男', 22, 0, 1602, '金融学', '中卫市', '123456';
EXEC AddStudent '201602070213', '林龙锋', 0, '男', 20, 0, 1602, '金融学', '新余市', '123456';
EXEC AddStudent '201602070214', '李玉方', 0, '女', 22, 0, 1602, '金融学', '温州市', '123456';
EXEC AddStudent '201602070215', '邓奕晖', 0, '男', 21, 0, 1602, '金融学', '保山市', '123456';
EXEC AddStudent '201602070216', '李勇军', 0, '男', 20, 0, 1602, '金融学', '濮阳市', '123456';
EXEC AddStudent '201602070217', '李彩玲', 0, '女', 20, 0, 1602, '金融学', '黔西南布依族苗族自治州', '123456';
EXEC AddStudent '201602070218', '甘利娟', 0, '女', 19, 0, 1602, '金融学', '伊春市', '123456';
EXEC AddStudent '201602070219', '钟银娣', 0, '女', 19, 0, 1602, '金融学', '泉州市', '123456';
EXEC AddStudent '201602070220', '赖紫平', 0, '女', 20, 0, 1602, '金融学', '伊春市', '123456';
EXEC AddStudent '201706060101', '李家威', 0, '男', 18, 0, 1701, '计算机科学与技术', '赣州市', '123456';
EXEC AddStudent '201706060102', '李欢媚', 0, '女', 18, 0, 1701, '计算机科学与技术', '大理白族自治州', '123456';
EXEC AddStudent '201706060103', '张文冬', 0, '男', 18, 0, 1701, '计算机科学与技术', '宜宾市', '123456';
EXEC AddStudent '201706060104', '陈啸靖', 0, '男', 22, 0, 1701, '计算机科学与技术', '大庆市', '123456';
EXEC AddStudent '201706060105', '赖远东', 0, '男', 19, 0, 1701, '计算机科学与技术', '东莞市', '123456';
EXEC AddStudent '201706060106', '李海聪', 0, '男', 22, 0, 1701, '计算机科学与技术', '海北藏族自治州', '123456';
EXEC AddStudent '201706060107', '李志勇', 0, '男', 22, 0, 1701, '计算机科学与技术', '辽阳市', '123456';
EXEC AddStudent '201706060108', '张丽花', 0, '女', 20, 0, 1701, '计算机科学与技术', '临夏回族自治州', '123456';
EXEC AddStudent '201706060109', '曾秋娴', 0, '女', 21, 0, 1701, '计算机科学与技术', '固原市', '123456';
EXEC AddStudent '201706060110', '邓力宏', 0, '男', 22, 0, 1701, '计算机科学与技术', '厦门市', '123456';
EXEC AddStudent '201706060111', '李艳芬', 0, '女', 20, 0, 1701, '计算机科学与技术', '梧州市', '123456';
EXEC AddStudent '201706060112', '陈胜文', 0, '男', 19, 0, 1701, '计算机科学与技术', '崇左市', '123456';
EXEC AddStudent '201706060113', '梁文惠', 0, '女', 21, 0, 1701, '计算机科学与技术', '益阳市', '123456';
EXEC AddStudent '201706060114', '陈旺通', 0, '男', 21, 0, 1701, '计算机科学与技术', '滁州市', '123456';
EXEC AddStudent '201706060115', '张国民', 0, '男', 21, 0, 1701, '计算机科学与技术', '菏泽市', '123456';
EXEC AddStudent '201706060116', '赖松飞', 0, '男', 21, 0, 1701, '计算机科学与技术', '凉山彝族自治州', '123456';
EXEC AddStudent '201706060117', '李滔', 0, '男', 21, 0, 1701, '计算机科学与技术', '保定市', '123456';
EXEC AddStudent '201706060118', '陈宁宇', 0, '男', 19, 0, 1701, '计算机科学与技术', '梧州市', '123456';
EXEC AddStudent '201706060119', '陈伟文', 0, '男', 20, 0, 1701, '计算机科学与技术', '驻马店市', '123456';
EXEC AddStudent '201706060120', '杜敏华', 0, '男', 18, 0, 1701, '计算机科学与技术', '邵阳市', '123456';

INSERT INTO yangzh_Course01 VALUES ('C01', '军事理论', 64, '考查', 4);
INSERT INTO yangzh_Course01 VALUES ('C02', '思想政治社会实践', 32, '考查', 2);
INSERT INTO yangzh_Course01 VALUES ('C03', '高等数学', 64, '考试', 4);
INSERT INTO yangzh_Course01 VALUES ('C04', '线性代数', 32, '考试', 2);
INSERT INTO yangzh_Course01 VALUES ('C05', '大学物理', 64, '考试', 4);
INSERT INTO yangzh_Course01 VALUES ('C06', '大学物理实验', 32, '考查', 2);

EXEC AddTeacher 'T01', '杜百根', '男', 35, '讲师', '13888888888', '123456';
EXEC AddTeacher 'T02', '赖燕婷', '女', 42, '副教授', '18868686868', '123456';
EXEC AddTeacher 'T03', '张文冬', '男', 52, '教授', '15656565656', '123456';
EXEC AddTeacher 'T04', '杜燕珍', '女', 45, '副教授', '13728282828', '123456';
EXEC AddTeacher 'T05', '李刚', '男', 43, '教授', '13523452345', '123456';

INSERT INTO yangzh_SetUpCourse01 VALUES (1603, '软件工程', 'C01', 'T01', '2016-2017', '2', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1604, '软件工程', 'C01', 'T01', '2016-2017', '2', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1603, '软件工程', 'C05', 'T05', '2016-2017', '2', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1603, '软件工程', 'C06', 'T05', '2016-2017', '2', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1602, '金融学', 'C01', 'T01', '2016-2017', '2', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1603, '软件工程', 'C02', 'T04', '2016-2017', '短', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1701, '计算机科学与技术', 'C03', 'T02', '2017-2018', '1', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1603, '软件工程', 'C04', 'T02', '2017-2018', '1', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1602, '金融学', 'C04', 'T02', '2017-2018', '1', 0);
INSERT INTO yangzh_SetUpCourse01 VALUES (1604, '软件工程', 'C04', 'T03', '2017-2018', '1', 0);

EXEC AddAdmin 'admin', '123456';