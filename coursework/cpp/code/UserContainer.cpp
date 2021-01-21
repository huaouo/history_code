//
// Created by Hua on 2017/5/18.
//

#include <set>
#include <string>
#include <utility>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "UserContainer.h"
#include "GlobalDefine.h"
#include "UserWorker.h"
#include "AdministratorWorker.h"

namespace hua {
	UserContainer::UserContainer()
	{
		std::string username, TempString;
		UserInfo info;
		std::ifstream InFileStream(UserFile, std::ios::in);
		while(1) {
			//从文件读入相关信息
			if(!(InFileStream >> username >> info.password >> TempString)) {
				break;
			}
			info.privilege = ((TempString == "User")? User: Administrator);
			CoreContainer.insert(make_pair(username, info));
		}
		InFileStream.close();
	}

	void UserContainer::AddUser(const std::string& username, const std::string& password,
		const Privilege& privilege)
	{
		if(CoreContainer[username].password != "") {
			throw KeyExistException(std::string("ERROR: User Exists !"));
		}

		//增加用户
		CoreContainer[username] = UserInfo(password, privilege);
		Save();
	}

	void UserContainer::DeleteUser(const std::string username)
	{
		auto it = CoreContainer.find(username);
		//没找到要删除的用户则抛出异常
		if(it == CoreContainer.end()) {
			throw KeyExistException(std::string("ERROR: User Doesn\'t Exist !"));
		}

		//删除用户
		CoreContainer.erase(username);
		Save();
	}

	std::string UserContainer::QueryPassword(const std::string& username) const
	{
		auto it = CoreContainer.find(username);
		//没找到要查询的用户则抛出异常
		if(it == CoreContainer.end()) {
			throw KeyExistException(std::string("ERROR: User Doesn\'t Exist !"));
		}

		return (it -> second).password;
	}

	Privilege UserContainer::QueryPrivilege(const std::string& username) const
	{
		auto it = CoreContainer.find(username);
		//没找到则抛出异常
		if(it == CoreContainer.end()) {
			throw KeyExistException(std::string("ERROR: User Doesn\'t Exist !"));
		}

		return (it -> second).privilege;
	}

	void UserContainer::ChangePassword(const std::string & username, const std::string & new_password)
	{
		auto it = CoreContainer.find(username);
		if(it == CoreContainer.end()) {
			throw KeyExistException(std::string("ERROR: User Doesn\'t Exist !"));
		}
		if((it->second).password == new_password) {
			throw KeyExistException(std::string("ERROR: New Password Shouldn\'t Be The Same As The Old !"));
		}
		(it->second).password = new_password;
		Save();
	}

	void UserContainer::ChangePrivilege(const std::string& username, const Privilege& new_privilege)
	{
		auto it = CoreContainer.find(username);
		if(it == CoreContainer.end()) {
			throw KeyExistException(std::string("ERROR: User Doesn\'t Exists !"));
		}
		if((it->second).privilege== new_privilege) {
			throw KeyExistException(std::string("ERROR: New Privilege Shouldn\'t Be The Same As The Old !"));
		}
		(it->second).privilege = new_privilege;
		Save();
	}

	std::set<std::string> UserContainer::ListUser()
	{
		std::set<std::string> ret;
		for(auto& i: CoreContainer) {
			ret.insert(i.first);
		}
		return ret;
	}

	void UserContainer::Save()
	{
		std::ofstream OutFileStream(UserFile, std::ios::out);
		std::string username, password, privilege;
		for(auto& i: CoreContainer) {
			OutFileStream << i.first << ' ' << i.second.password << ' '
				<< (i.second.privilege? "Administrator": "User") << '\n';
		}
		OutFileStream.close();
	}

	AbstractWorker* UserContainer::Login(const std::string& username, const std::string& password)
	{
		AbstractWorker* ptr = nullptr;
		if(CoreContainer[username].password == password) {
			Privilege p = CoreContainer[username].privilege;
			//按不同类别登录并返回用户指针
			if(p == User) {
				ptr = new UserWorker(username, password, *this);
			}
			else {
				ptr = new AdministratorWorker(username, password, *this);
			}
			return ptr;
		}
		else {
			throw LoginFailException(std::string("ERROR: UserName or Password Wrong !"));
		}
	}

	bool UserContainer::Empty()
	{
		return CoreContainer.empty();
	}

	void UserContainer::RegisterFirstUser(const std::string& username, const std::string& password)
	{
		//先建立文件
		system((std::string("type NUL > ") + std::string(UserFile)).c_str());
		std::ofstream OutFileStream(UserFile, std::ios::out);
		OutFileStream << username << ' ' << password << " Administrator" << '\n';
		OutFileStream.close();
		new(this) UserContainer();
	}
}
