//
// Created by Hua on 2017/5/11.
//

#ifndef ADDRESSBOOK_RECORDCONTAINER_H
#define ADDRESSBOOK_RECORDCONTAINER_H

#include <string>
#include <vector>
#include "GlobalDefine.h"

namespace hua {
	//记录类型的结构体
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

		//结构体的首string指针和末string指针的后一个指针位置
		//此函数用于支持C++ 11新语法range for
		std::string* begin();
		std::string* end();
    };

	//记录容器类
	class RecordContainer {
	public:
		//别名
		using SizeType = std::vector<RecordType>::size_type;
		using Iterator = std::vector<RecordType>::iterator;

		RecordContainer();
		//禁止拷贝
		RecordContainer(const RecordContainer&) = delete;
		~RecordContainer() = default;

		//禁止拷贝
		RecordContainer& operator=(const RecordContainer&) = delete;

		//添加记录
		void                  AddRecord(const RecordType& new_record);
		//删除记录
		void                  DeleteRecord(const Iterator& it);
		//查询记录
		std::vector<Iterator> QueryRecord(const RecordType& key);
		//按名字排序
		void                  SortedByName();
		//按毕业时间排序
		void                  SortedByGraduateYear();
		//将容器内容保存到文件
		void                  Save();

	private:
		//核心容器，是RecordType类型vector
		std::vector<RecordType> CoreContainer;
	};

	//异常处理类，用于容器内不存在某值的情况
	class ValueExistException: public std::runtime_error {
	public:
		explicit ValueExistException(const std::string& message): std::runtime_error(message) {}
	};
}

#endif
