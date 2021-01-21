//
// Created by Hua on 2017/5/11.
//

#ifndef ADDRESSBOOK_GLOBALDEFINE_H
#define ADDRESSBOOK_GLOBALDEFINE_H
#include <string>

//ȫ�ֺ����ͱ���
namespace hua {
	//ǰ��������
	class AbstractWorker;
	class UserContainer;

	//�������������ַ���
	extern const char* CLEAR;
	//�û��ļ���
	extern const char* UserFile;
	//��¼�ļ���
	extern const char* RecordFile;
	//����ʾ�� >>>
	extern const char* PROMPT1;
	//����ʾ�� ...
	extern const char* PROMPT2;
	//������ͣʱ��
	extern const int SLEEP_TIME;
	//��������
	extern const short cols;

	//����ַ����������Ƿ�Ϊ��
	bool CheckEmpty(std::stringstream& ss);
	//��ȡ���룬��ʾ��*
	std::string GetPassword();
	//�ж��ַ����Ƿ��пո�
	bool HaveSpace(const std::string& s);
	//��¼��ע��
	AbstractWorker* RegisterAndLoginInterface(UserContainer& uc);
}

#endif
