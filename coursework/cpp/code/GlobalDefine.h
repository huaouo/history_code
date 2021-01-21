//
// Created by Hua on 2017/5/11.
//

#ifndef ADDRESSBOOK_GLOBALDEFINE_H
#define ADDRESSBOOK_GLOBALDEFINE_H
#include <string>

//全局函数和变量
namespace hua {
	//前置类声明
	class AbstractWorker;
	class UserContainer;

	//清屏操作命令字符串
	extern const char* CLEAR;
	//用户文件名
	extern const char* UserFile;
	//记录文件名
	extern const char* RecordFile;
	//主提示符 >>>
	extern const char* PROMPT1;
	//次提示符 ...
	extern const char* PROMPT2;
	//单次暂停时间
	extern const int SLEEP_TIME;
	//窗口列数
	extern const short cols;

	//检测字符串流对象是否为空
	bool CheckEmpty(std::stringstream& ss);
	//获取密码，显示成*
	std::string GetPassword();
	//判断字符串是否有空格
	bool HaveSpace(const std::string& s);
	//登录和注册
	AbstractWorker* RegisterAndLoginInterface(UserContainer& uc);
}

#endif
