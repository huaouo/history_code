//
// Created by Hua on 2017/5/22.
//

#ifndef ADDRESSBOOK_ADMINISTRATORWORKER_H
#define ADDRESSBOOK_ADMINISTRATORWORKER_H

#include <string>
#include "UserWorker.h"

namespace hua{

	//管理员类，直接继承自普通用户类（具有普通用户的全部功能），间接继承自抽象工作基类
	class AdministratorWorker: public UserWorker {
	public:
		//阻止默认构造
		AdministratorWorker() = delete;
		//每个程序仅存在一个工作对象，禁止拷贝
		AdministratorWorker(const AdministratorWorker&) = delete;
		AdministratorWorker(const std::string& usrname, const std::string& pwd,
			UserContainer& uc): UserWorker(usrname, pwd, uc) {}
		~AdministratorWorker() = default;

		//同拷贝构造函数，禁止拷贝
		AdministratorWorker& operator=(const AdministratorWorker&) = default;

		//解析命令字符串，调用类中相关函数
		void Parse() override;
		//输出登录成功界面
		void PrintHello() override;
		//输出帮助
		void PrintHelp() override;
		//添加记录
		void AddRecord(const RecordType& new_record);
		//删除记录
		void DeleteRecord(const std::vector<RecordContainer::Iterator>::size_type& index);
		//列出用户
		void ListUser();
		//添加用户
		void AddUser(const std::string& new_username, const std::string& new_password, 
			const Privilege& new_privilege);
		//删除用户
		void DeleteUser(const std::string& username);
		//改任意用户的密码
		void ChangePassword(std::string& username, std::string& new_password) const;
		//改任意用户权限
		void ChangePrivilege(const std::string& username, const Privilege& new_privilege);
	private:
		//用户查询缓存容器
		std::set<std::string> user_query;
	};
}
#endif
