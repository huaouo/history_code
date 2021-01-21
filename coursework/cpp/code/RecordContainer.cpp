//
// Created by Hua on 2017/5/19.
//

#include <vector>
#include <algorithm>
#include <fstream>
#include "RecordContainer.h"

namespace hua {

	std::string* RecordType::begin()
	{
		return &name;
	}

	std::string* RecordType::end()
	{
		return (&email) + 1;
	}

	RecordContainer::RecordContainer()
	{
		std::ifstream InFileStream(RecordFile, std::ios::in);
		RecordType TempRecord;
		while(1) {
			for(auto& s: TempRecord) {
				//文件读取失败直接停止
				if(!(InFileStream >> s)) {
					goto EXIT_LOOP;
				}
				std::size_t found = s.size();
				while((found = s.find("+"))  < s.size()) {
					s = s.replace(found, 1, " ");
				}
				if(s == "NULL") {
					s = "";
				}
			}
			CoreContainer.push_back(TempRecord);
		}
		EXIT_LOOP:
		InFileStream.close();
	}

	void RecordContainer::AddRecord(const RecordType& new_record)
	{
		CoreContainer.push_back(new_record);
		Save();
	}

	void RecordContainer::DeleteRecord(const Iterator& it)
	{
		CoreContainer.erase(it);
		Save();
	}

	std::vector<RecordContainer::Iterator> RecordContainer::QueryRecord(const RecordType& key)
	{
		std::vector<Iterator> ret;
		//判断是否有值相等
		for(auto it = CoreContainer.begin();  it != CoreContainer.end(); ++it) {
			if((key.sex == it -> sex                              || key.sex == "")
				&&(key.qq == it -> qq                             || key.qq == "")
				&&(key.name == it -> name                         || key.name == "")
				&&(key.age == it -> age                           || key.age == "")
				&&(key.address == it -> address                   || key.address == "")
				&&(key.email == it -> email                       || key.email == "")
				&&(key.graduate_year == it -> graduate_year       || key.graduate_year == "")
				&&(key.department == it -> department             || key.department == "")
				&&(key.class_number == it -> class_number         || key.class_number == "")
				&&(key.telephone_number == it -> telephone_number || key.telephone_number == "")) {

				//放入临时容器
				ret.push_back(it);
			}
		}
		if(ret.size() == 0) {
			throw ValueExistException(std::string("ERROR: Record Doesn\'t Exist or No Record !"));
		}
		return ret;
	}

	void RecordContainer::SortedByName()
	{
		if(!CoreContainer.size()){
			throw ValueExistException(std::string("Record Container is Empty !"));
		}
		//lambda表达式实现排序规则
		std::sort(CoreContainer.begin(), CoreContainer.end(), [](const RecordType& le, const RecordType& ri) {
			return le.name < ri.name;
		});
		Save();
	}

	void RecordContainer::SortedByGraduateYear()
	{
		if(!CoreContainer.size()){
			throw ValueExistException(std::string("Record Container is Empty !"));
		}
		//lambda表达式实现排序规则
		std::sort(CoreContainer.begin(), CoreContainer.end(), [](const RecordType& le, const RecordType& ri) {
			return le.graduate_year < ri.graduate_year;
		});
		Save();
	}

	void RecordContainer::Save()
	{
		std::ofstream OutFileStream(RecordFile, std::ios::out);
		for(auto& i: CoreContainer) {
			for(auto& s: i) {
				std::size_t found = s.size();
				while((found = s.find(" "))  < s.size()) {
					s = s.replace(found, 1, "+");
				}
				OutFileStream << ((s == "")? "NULL": s) << '\n';
			}
			OutFileStream << '\n';
		}
		OutFileStream.close();
	}
}
