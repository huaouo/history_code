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
	//ǰ������
	class AbstractWorker;

	//Ȩ��ö�٣������û�Ȩ��
	enum Privilege{
		User, Administrator
	};

	//�û���Ϣ��Ȩ�޺�����
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

	//�û�������
    class UserContainer {
	public:
		//����
		using UserName = std::string;
		using Iterator = std::map<UserName, UserInfo>::iterator;

		UserContainer();
		//��ֹ����
		UserContainer(const UserContainer&) = delete;
		~UserContainer() = default;

		//��ֹ����
		UserContainer& operator=(const UserContainer&) = delete;

		//����û�
		void                  AddUser(const std::string& username, const std::string& password, const Privilege& privilege);
		//ɾ���û�
		void                  DeleteUser(const std::string username);
		//�����û�����ѯ����
		std::string           QueryPassword(const std::string& username) const;
		//�����û�����ѯȨ��
		Privilege             QueryPrivilege(const std::string& username) const;
		//������
		void                  ChangePassword(const std::string& username, const std::string& new_password);
		//��Ȩ��
		void                  ChangePrivilege(const std::string& username, const Privilege& new_privilege);
		//�г������û�
		std::set<std::string> ListUser();
		//���������ݱ��浽�ļ�
		void                  Save();
		//��¼������У�飩
		AbstractWorker*       Login(const std::string& username, const std::string& password);
		//��������Ƿ�Ϊ��
		bool                  Empty();
		//ע���һ������Ա�û�
		void                  RegisterFirstUser(const std::string& username, const std::string& password);
	private:
		//����������KeyΪ�û�����ValueΪUserInfo��map
		std::map<UserName, UserInfo> CoreContainer;
	};

	//�쳣�����࣬�����û��������ڵ����
	class KeyExistException: public std::runtime_error {
	public:
		explicit KeyExistException(const std::string& message): runtime_error(message) {}
	};

	//�쳣�����࣬���ڵ�¼ʧ�ܵ����
	class LoginFailException: public std::runtime_error {
	public:
		explicit LoginFailException(const std::string& message): runtime_error(message) {}
	};
}

#endif
