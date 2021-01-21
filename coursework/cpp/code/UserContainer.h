//
// Created by Hua on 2017/5/11.
//

#ifndef ADDRESSBOOK_USERCONTAINER_H
#define ADDRESSBOOK_USERCONTAINER_H

#include <set>
#include <map>
#include <vector>
#include <string>
#include <stdexcept>

namespace hua {
	//前置声明
	class AbstractWorker;

	//权限枚举，定义用户权限
	enum Privilege{
		User, Administrator
	};

	//用户信息，权限和密码
    struct UserInfo {
        Privilege   privilege;
        std::string password;

        UserInfo(): privilege(User), password(std::string("")) {}
		UserInfo(std::string pass, Privilege pri = User):
			privilege(pri), password(pass) {}
        UserInfo(const UserInfo&) = default;
        ~UserInfo() = default;

		UserInfo& operator=(const UserInfo&) = default;
    };

	//用户容器类
    class UserContainer {
	public:
		//别名
		using UserName = std::string;
		using Iterator = std::map<UserName, UserInfo>::iterator;

		UserContainer();
		//禁止拷贝
		UserContainer(const UserContainer&) = delete;
		~UserContainer() = default;

		//禁止拷贝
		UserContainer& operator=(const UserContainer&) = delete;

		//添加用户
		void                  AddUser(const std::string& username, const std::string& password, const Privilege& privilege);
		//删除用户
		void                  DeleteUser(const std::string username);
		//根据用户名查询密码
		std::string           QueryPassword(const std::string& username) const;
		//根据用户名查询权限
		Privilege             QueryPrivilege(const std::string& username) const;
		//改密码
		void                  ChangePassword(const std::string& username, const std::string& new_password);
		//改权限
		void                  ChangePrivilege(const std::string& username, const Privilege& new_privilege);
		//列出所有用户
		std::set<std::string> ListUser();
		//将容器内容保存到文件
		void                  Save();
		//登录（用于校验）
		AbstractWorker*       Login(const std::string& username, const std::string& password);
		//检测容器是否为空
		bool                  Empty();
		//注册第一个管理员用户
		void                  RegisterFirstUser(const std::string& username, const std::string& password);
	private:
		//核心容器，Key为用户名，Value为UserInfo的map
		std::map<UserName, UserInfo> CoreContainer;
	};

	//异常处理类，用于用户名不存在的情况
	class KeyExistException: public std::runtime_error {
	public:
		explicit KeyExistException(const std::string& message): runtime_error(message) {}
	};

	//异常处理类，用于登录失败的情况
	class LoginFailException: public std::runtime_error {
	public:
		explicit LoginFailException(const std::string& message): runtime_error(message) {}
	};
}

#endif
