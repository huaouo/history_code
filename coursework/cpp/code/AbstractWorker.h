//
// Created by Hua on 2017/5/22.
//

#ifndef ADDRESSBOOK_ABSTRACTUSER_H
#define ADDRESSBOOK_ABSTRACTUSER_H

#include <string>
#include "UserContainer.h"
#include "RecordContainer.h"

namespace hua{

	//抽象工作基类，定义管理员类和普通用户类的必有部分
	class AbstractWorker {
	public:
		AbstractWorker() = default;
		//每个程序仅存在一个工作对象，禁止拷贝
		AbstractWorker(const AbstractWorker&) = delete;
		AbstractWorker(const std::string& usrname, const std::string& pwd, UserContainer& uc):
			username(usrname), password(pwd), users(uc){}
		//虚析构函数，防止基类部分不被析构
		virtual ~AbstractWorker() = default;

		//同拷贝构造函数，禁止拷贝
		AbstractWorker& operator=(const AbstractWorker&) = delete;

		//输出主提示符 >>>
		void        PrintMainPrompt() const;
		//输出次提示符 ...
		void        PrintSecondaryPrompt() const;
		//清屏
		void        Clear() const;
		//退出程序
		void        Exit() const;
		//将多行输入处理、整合成单个命令字符串
		std::string GetCommandString();

		//解析命令字符串，调用类中相关函数
		virtual void Parse() = 0;
		//输出登录成功界面
		virtual void PrintHello() = 0;
		//输出帮助
		virtual void PrintHelp() = 0;
		//列出所有记录
		virtual void PrintListRecord() = 0;
		//查询记录
		virtual void PrintQueryRecord(const RecordType& query_key) = 0;
		//输出电话号码二维码
		virtual void ShowTelephoneQRCode(const std::vector<RecordContainer::Iterator>::size_type& index) = 0;
		//输出电子邮箱二维码
		virtual void ShowEmailQRCode(const std::vector<RecordContainer::Iterator>::size_type& index) = 0;
		//按名字排序
		virtual void SortedByName() = 0;
		//按毕业时间排序
		virtual void SortedByGraduateYear() = 0;
	protected:
		//用户名
		std::string username;
		//密码
		std::string password;
		//用户信息容器
		UserContainer& users;
		//记录容器
		RecordContainer records;
		//记录查询缓存容器
		std::vector<RecordContainer::Iterator> record_query;
	};

	//异常处理类，用于命令字符串格式错误
	class CommandLineSyntaxError: public std::runtime_error {
	public:
		explicit CommandLineSyntaxError(const std::string& message): runtime_error(message) {}
	};
}
#endif
