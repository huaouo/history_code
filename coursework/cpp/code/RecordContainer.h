//
// Created by Hua on 2017/5/11.
//

#ifndef ADDRESSBOOK_RECORDCONTAINER_H
#define ADDRESSBOOK_RECORDCONTAINER_H

#include <string>
#include <vector>
#include "GlobalDefine.h"

namespace hua {
	//��¼���͵Ľṹ��
    struct RecordType {
		std::string name;
        std::string sex;
		std::string age;
		std::string graduate_year;
		std::string department;
		std::string class_number;
		std::string address;
		std::string telephone_number;
        std::string qq;
        std::string email;

		RecordType() = default;
        RecordType(const RecordType&) = default;
        ~RecordType() = default;

		//�ṹ�����stringָ���ĩstringָ��ĺ�һ��ָ��λ��
		//�˺�������֧��C++ 11���﷨range for
		std::string* begin();
		std::string* end();
    };

	//��¼������
	class RecordContainer {
	public:
		//����
		using SizeType = std::vector<RecordType>::size_type;
		using Iterator = std::vector<RecordType>::iterator;

		RecordContainer();
		//��ֹ����
		RecordContainer(const RecordContainer&) = delete;
		~RecordContainer() = default;

		//��ֹ����
		RecordContainer& operator=(const RecordContainer&) = delete;

		//��Ӽ�¼
		void                  AddRecord(const RecordType& new_record);
		//ɾ����¼
		void                  DeleteRecord(const Iterator& it);
		//��ѯ��¼
		std::vector<Iterator> QueryRecord(const RecordType& key);
		//����������
		void                  SortedByName();
		//����ҵʱ������
		void                  SortedByGraduateYear();
		//���������ݱ��浽�ļ�
		void                  Save();

	private:
		//������������RecordType����vector
		std::vector<RecordType> CoreContainer;
	};

	//�쳣�����࣬���������ڲ�����ĳֵ�����
	class ValueExistException: public std::runtime_error {
	public:
		explicit ValueExistException(const std::string& message): std::runtime_error(message) {}
	};
}

#endif
