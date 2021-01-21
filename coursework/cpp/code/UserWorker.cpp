//
// Created by Hua on 2017/5/22.
//

#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include "UserWorker.h"
#include "libQRCode\QrCode.h"

namespace hua{
	void UserWorker::Parse()
	{
		std::string command;
		try {
			command = GetCommandString();
		} catch(std::runtime_error& e) {
			std::cerr << e.what() << '\n';
			return;
		}
		//将命令字符串放入stringstream，等待进一步处理
		std::stringstream ss;
		ss << command;
		std::string basic_command, pos_command;
		ss >> basic_command;
		if(basic_command == "exit" && CheckEmpty(ss)) {
			Exit();
		}
		else if(basic_command == "clear" && CheckEmpty(ss)) {
			Clear();
		}
		else if(basic_command == "help" && CheckEmpty(ss)) {
			PrintHelp();
		}
		else {
			ss >> pos_command;
			if(basic_command == "list" && pos_command == "record" && CheckEmpty(ss)) {
				PrintListRecord();
			}
			else if(basic_command == "sort") {
				if(pos_command == "name" && CheckEmpty(ss)) {
					SortedByName();
				}
				else if(pos_command == "graduate_year" && CheckEmpty(ss)) {
					SortedByGraduateYear();
				}
				else {
					std::cerr << "ERROR: Syntax Error !\n";
				}
			}
			else if(basic_command  == "showqr") {
				int index;
				ss >> index;
				if(pos_command == "phone" && CheckEmpty(ss) && index > 0) {
					ShowTelephoneQRCode(static_cast<size_t>(index));
				}
				else if(pos_command == "email" && CheckEmpty(ss) && index > 0) {
					ShowEmailQRCode(static_cast<size_t>(index));
				}
				else {
					std::cerr << "ERROR: Syntax Error !\n";
				}
			}
			else if(basic_command  == "change" && pos_command == "password" && CheckEmpty(ss)) {
				std::string new_password, check_new_password;
				bool if_loop = CheckEmpty(ss);
				while (if_loop) {
					while (1) {
						std::cout << "New Password: ";
						new_password = hua::GetPassword();
						if (new_password == "") {
							std::cerr << "ERROR: Empty Password Is Forbidden !\n";
							continue;
						}
						if (!hua::HaveSpace(new_password)) {
							break;
						}
						std::cerr << "ERROR: Password Should Not Consist of Space !\n";
					}

					while (1) {
						std::cout << "New Password Again: ";
						check_new_password = hua::GetPassword();
						if (check_new_password == "") {
							std::cerr << "ERROR: Empty Password Is Forbidden !\n";
							continue;
						}
						if (!hua::HaveSpace(check_new_password)) {
							break;
						}
						std::cerr << "ERROR: Password Should Not Consist of Space !\n";
					}

					if(new_password == check_new_password) {
						break;
					}
					std::cerr << "ERROR: Password Mismatch! \n";
				}
				ChangePassword(new_password);
				password = new_password;
			}
			else if(basic_command  == "query" && pos_command == "record" && !CheckEmpty(ss)) {
				std::string equal_form_mark;
				ss >> equal_form_mark;
				if(equal_form_mark != "EQUAL_FORM_MARK") {
					std::cerr << "ERROR: Syntax Error !\n";
				}
				else {
					RecordType TempRecord;
					std::string Key;
					std::string Value;
					bool err = false;
					bool mkname = false,
						mksex = false,
						mkage = false,
						mkgraduate_year = false,
						mkdepartment = false,
						mkclass = false,
						mkaddress = false,
						mkphone = false,
						mkqq = false,
						mkemail = false;
					while((Key = "", Value = "", true) && ss >> Key >> Value) {
						if(Key == "name" && !mkname) {
							TempRecord.name = Value;
							mkname = true;
						}
						else if(Key == "sex" && !mksex) {
							TempRecord.sex = Value;
							mksex = true;
						}
						else if(Key == "age" && !mkage) {
							TempRecord.age = Value;
							mkage = true;
						}
						else if(Key == "graduate_year" && !mkgraduate_year) {
							TempRecord.graduate_year = Value;
							mkgraduate_year = true;
						}
						else if(Key == "department" && !mkdepartment) {
							TempRecord.department = Value;
							mkdepartment = true;
						}
						else if(Key == "class" && !mkclass) {
							TempRecord.class_number = Value;
							mkclass = true;
						}
						else if(Key == "address" && !mkaddress) {
							TempRecord.address = Value;
							mkaddress = true;
						}
						else if(Key == "phone" && !mkphone) {
							TempRecord.telephone_number = Value;
							mkphone = true;
						}
						else if(Key == "qq" && !mkqq) {
							TempRecord.qq = Value;
							mkqq = true;
						}
						else if(Key == "email" && !mkemail) {
							TempRecord.email = Value;
							mkemail = true;
						}
						else {
							err = true;
							break;
						}
					}
					if(err || (Value == "" && Key != "")) {
						std::cerr << "ERROR: Syntax Error !\n";
					}
					else {
						PrintQueryRecord(TempRecord);
					}
				}
			}
			else {
				std::cerr << "ERROR: Syntax Error !\n";
			}
		}
	}

	void UserWorker::PrintHello()
	{
		std::cout << username << "(User)@AddressBook\n"
			<< "Type \"help\" for command line syntax help.\n";
	}

	void UserWorker::PrintHelp()
	{
		std::cout << "Commands Available:\n"
			<< "clear;\t\t\t\t清屏\n"
			<< "help;\t\t\t\t输出帮助\n"
			<< "list record;\t\t\t列出记录\n"
			<< "showqr phone <index>;\t\t在查询记录后指定以二维码形式输出某记录的电话\n"
			<< "showqr email <index>;\t\t在查询记录后指定以二维码形式输出某记录的电子邮箱\n"
			<< "sort <name|graduate_year>;\t按 姓名|毕业年份 对记录进行排序\n"
			<< "change password;\t\t修改本帐户密码\n"
			<< "query record <[key]=[value]>;\t查询记录\n";
	}

	void UserWorker::PrintListRecord()
	{
		RecordType empty;
		PrintQueryRecord(empty);
	}

	void UserWorker::PrintQueryRecord(const RecordType& query_key)
	{
		//字段名
		const std::vector<std::string> cols = {"Index", "Name", "Sex", "Age",
			"Graduate_Year", "Department", "Class", "Address", "Phone", "QQ", "Email"};
		//字段长度
		std::vector<std::size_t> width(cols.size());
		//初始化字段长度
		for(std::size_t i = 0; i < cols.size(); ++i){
			width[i] = cols[i].size();
		}
		try {
			record_query = records.QueryRecord(query_key);
		}
		catch(std::runtime_error& e) {
			std::cerr << e.what() << '\n';
			return;
		}
		//更新字段长度
		for(auto& i: record_query) {
			for(std::size_t in = 1; in < cols.size(); ++in) {
				width[in] = std::max((*((*i).begin() + in - 1)).size(), width[in]);
			}
		}
		std::string gapline = "+";
		for(auto& i: width) {
			gapline += std::string(i, '-') + "+";
		}
		std::cout << gapline << "\n|";
		for(size_t i = 0; i < cols.size(); ++i) {
			std::cout << std::string(width[i] - cols[i].size(), ' ') << cols[i] << '|';
		}
		std::cout << '\n' << gapline << '\n';
		size_t index = 0;
		//输出每个记录的信息
		for(auto& i: record_query) {
			++index;
			std::cout << '|' << std::string(width[0] - std::to_string(index).size(), ' ') << std::to_string(index) << '|';
			for(std::size_t in = 0; in < cols.size() - 1; ++in) {
				const std::string& s = *((*i).begin() + in);
				std::cout << std::string(width[in + 1] - s.size(), ' ') << s << '|';
			}
			std::cout << '\n' << gapline << '\n';
		}
	}

	void UserWorker::ShowTelephoneQRCode(const std::vector<RecordContainer::Iterator>::size_type& index)
	{
		if(index > record_query.size()) {
			std::cerr << "ERROR: Query Index Doesn\'t Exist !\n";
		}
		else {
			auto qr = qrcodegen::QrCode::encodeText((record_query[index - 1] -> telephone_number).c_str(),
			qrcodegen::QrCode::Ecc::LOW);
			//循环输出二维码
			for(std::size_t y = 0; y < static_cast<std::size_t>(qr.size); ++y) {
				for(std::size_t x = 0; x < static_cast<std::size_t>(qr.size); ++x) {
					std::cout << (qr.getModule(static_cast<int>(x), static_cast<int>(y))? "  ": "█");
				}
				std::cout << std::endl;
			}
		}
	}

	void UserWorker::ShowEmailQRCode(const std::vector<RecordContainer::Iterator>::size_type& index)
	{
		if(index > record_query.size()) {
			std::cerr << "ERROR: Query Index Doesn\'t Exist !\n";
		}
		else {
			auto qr = qrcodegen::QrCode::encodeText((record_query[index - 1] -> email).c_str(),
			qrcodegen::QrCode::Ecc::LOW);
			//循环输出二维码
			for(std::size_t y = 0; y < static_cast<std::size_t>(qr.size); ++y) {
				for(std::size_t x = 0; x < static_cast<std::size_t>(qr.size); ++x) {
					std::cout << (qr.getModule(static_cast<int>(x), static_cast<int>(y))? "  ": "█");
				}
				std::cout << std::endl;
			}
		}
	}

	void UserWorker::SortedByName()
	{
		try {
			records.SortedByName();
			std::clog << "Sorted by Name Successfully !\n";
		}
		catch(std::runtime_error& e) {
			std::cerr << e.what() << '\n';
		}
	}

	void UserWorker::SortedByGraduateYear()
	{
		try {
			records.SortedByGraduateYear();
			std::clog << "Sorted by Graduate_Year Successfully !\n";
		}
		catch(std::runtime_error& e) {
			std::cerr << e.what() << '\n';
		}
	}

	void UserWorker::ChangePassword(std::string& new_password) const
	{
		try{
			users.ChangePassword(username, new_password);
			std::clog << "Password Changed Successfully !\n";
		}
		catch(std::runtime_error& e) {
			std::cerr << e.what() << '\n';
		}
	}
}
