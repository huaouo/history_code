//
// Created by Hua on 2017/5/22.
//

#ifndef ADDRESSBOOK_ADMINISTRATORWORKER_H
#define ADDRESSBOOK_ADMINISTRATORWORKER_H

#include <string>
#include "UserWorker.h"

namespace hua{

	//����Ա�ֱ࣬�Ӽ̳�����ͨ�û��ࣨ������ͨ�û���ȫ�����ܣ�����Ӽ̳��Գ���������
	class AdministratorWorker: public UserWorker {
	public:
		//��ֹĬ�Ϲ���
		AdministratorWorker() = delete;
		//ÿ�����������һ���������󣬽�ֹ����
		AdministratorWorker(const AdministratorWorker&) = delete;
		AdministratorWorker(const std::string& usrname, const std::string& pwd,
			UserContainer& uc): UserWorker(usrname, pwd, uc) {}
		~AdministratorWorker() = default;

		//ͬ�������캯������ֹ����
		AdministratorWorker& operator=(const AdministratorWorker&) = default;

		//���������ַ���������������غ���
		void Parse() override;
		//�����¼�ɹ�����
		void PrintHello() override;
		//�������
		void PrintHelp() override;
		//��Ӽ�¼
		void AddRecord(const RecordType& new_record);
		//ɾ����¼
		void DeleteRecord(const std::vector<RecordContainer::Iterator>::size_type& index);
		//�г��û�
		void ListUser();
		//����û�
		void AddUser(const std::string& new_username, const std::string& new_password, 
			const Privilege& new_privilege);
		//ɾ���û�
		void DeleteUser(const std::string& username);
		//�������û�������
		void ChangePassword(std::string& username, std::string& new_password) const;
		//�������û�Ȩ��
		void ChangePrivilege(const std::string& username, const Privilege& new_privilege);
	private:
		//�û���ѯ��������
		std::set<std::string> user_query;
	};
}
#endif
