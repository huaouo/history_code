//
// Created by Hua on 2017/5/22.
//

#ifndef ADDRESSBOOK_ABSTRACTUSER_H
#define ADDRESSBOOK_ABSTRACTUSER_H

#include <string>
#include "UserContainer.h"
#include "RecordContainer.h"

namespace hua{

	//���������࣬�������Ա�����ͨ�û���ı��в���
	class AbstractWorker {
	public:
		AbstractWorker() = default;
		//ÿ�����������һ���������󣬽�ֹ����
		AbstractWorker(const AbstractWorker&) = delete;
		AbstractWorker(const std::string& usrname, const std::string& pwd, UserContainer& uc):
			username(usrname), password(pwd), users(uc){}
		//��������������ֹ���ಿ�ֲ�������
		virtual ~AbstractWorker() = default;

		//ͬ�������캯������ֹ����
		AbstractWorker& operator=(const AbstractWorker&) = delete;

		//�������ʾ�� >>>
		void        PrintMainPrompt() const;
		//�������ʾ�� ...
		void        PrintSecondaryPrompt() const;
		//����
		void        Clear() const;
		//�˳�����
		void        Exit() const;
		//���������봦�����ϳɵ��������ַ���
		std::string GetCommandString();

		//���������ַ���������������غ���
		virtual void Parse() = 0;
		//�����¼�ɹ�����
		virtual void PrintHello() = 0;
		//�������
		virtual void PrintHelp() = 0;
		//�г����м�¼
		virtual void PrintListRecord() = 0;
		//��ѯ��¼
		virtual void PrintQueryRecord(const RecordType& query_key) = 0;
		//����绰�����ά��
		virtual void ShowTelephoneQRCode(const std::vector<RecordContainer::Iterator>::size_type& index) = 0;
		//������������ά��
		virtual void ShowEmailQRCode(const std::vector<RecordContainer::Iterator>::size_type& index) = 0;
		//����������
		virtual void SortedByName() = 0;
		//����ҵʱ������
		virtual void SortedByGraduateYear() = 0;
	protected:
		//�û���
		std::string username;
		//����
		std::string password;
		//�û���Ϣ����
		UserContainer& users;
		//��¼����
		RecordContainer records;
		//��¼��ѯ��������
		std::vector<RecordContainer::Iterator> record_query;
	};

	//�쳣�����࣬���������ַ�����ʽ����
	class CommandLineSyntaxError: public std::runtime_error {
	public:
		explicit CommandLineSyntaxError(const std::string& message): runtime_error(message) {}
	};
}
#endif
