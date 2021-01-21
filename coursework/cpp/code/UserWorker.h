//
// Created by Hua on 2017/5/22.
//

#ifndef ADDRESSBOOK_USERWORKER_H
#define ADDRESSBOOK_USERWORKER_H

#include "AbstractWorker.h"

namespace hua{
	//�û��࣬�̳��Գ����û�����
	class UserWorker: public AbstractWorker {
	public:
		UserWorker() = default;
		//��ֹ����
		UserWorker(const UserWorker&) = delete;
		UserWorker(const std::string& usrname, const std::string& pwd, UserContainer& uc): 
			AbstractWorker(usrname, pwd, uc) {}
		~UserWorker() = default;

		//��ֹ����
		UserWorker& operator=(const UserWorker&) = default;

		//���������ַ���������������غ���
		void Parse() override;
		//�����¼�ɹ�����
		void PrintHello() override;
		//�������
		void PrintHelp() override;
		//�г����м�¼
		void PrintListRecord() override;
		//��ѯ��¼
		void PrintQueryRecord(const RecordType& query_key) override;
		//����绰�����ά��
		void ShowTelephoneQRCode(const std::vector<RecordContainer::Iterator>::size_type& index) override;
		//������������ά��
		void ShowEmailQRCode(const std::vector<RecordContainer::Iterator>::size_type& index) override;
		//����������
		void SortedByName() override;
		//����ҵʱ������
		void SortedByGraduateYear() override;
		//������
		void ChangePassword(std::string& new_password) const;
	};
}

#endif
