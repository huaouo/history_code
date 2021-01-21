//
// Created by Hua on 2017/5/25.
//

#include <iostream>
#include <sstream>
#include <algorithm>
#include "AdministratorWorker.h"

namespace hua {
	void AdministratorWorker::Parse()
	{
		std::string command;
		try {
			//获取命令字符串
			command = GetCommandString();
		} catch(std::runtime_error& e) {
			std::cerr << e.what() << '\n';
			return;
		}
		//将命令字符串放入stringstream进行解析
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
			else if(basic_command == "list" && pos_command == "user" && CheckEmpty(ss)) {
				ListUser();
			}
			else if(basic_command == "delete") {
				if(pos_command == "record") {
					int index = 0;
					ss >> index;
					if(index <= 0 || !CheckEmpty(ss)) {
						std::cerr << "ERROR: Syntax Error !\n";
					}
					else {
						DeleteRecord(index);
					}
				}
				else if(pos_command == "user") {
					std::string username;
					ss >> username;
					if(username == "" || !CheckEmpty(ss)) {
						std::cerr << "ERROR: Syntax Error !\n";
					}
					else {
						DeleteUser(username);
					}
				}
				else {
					std::cerr << "ERROR: Syntax Error !\n";
				}
			}
			else if (basic_command == "add" && pos_command == "user" && !CheckEmpty(ss)) {
				std::string username, privilege;
				ss >> username >> privilege;
				if (username == "" || (privilege != "User" && privilege != "Administrator") || !CheckEmpty(ss)) {
					std::cerr << "ERROR: Syntax Error !\n";
				}
				else {
					std::string new_password, check_new_password;
					while (1) {
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

						if (new_password == check_new_password) {
							break;
						}
						std::cerr << "ERROR: Password Mismatch! \n";
					}
					AddUser(username, new_password, privilege == "User"? User: Administrator);
				}
			}
			else if (basic_command == "add" && pos_command == "record" && !CheckEmpty(ss)) {
				std::vector<RecordType> temp_container;
				RecordType temp;
				bool fail = false;
				while(!CheckEmpty(ss)) {
					for(auto& i: temp) {
						i = "__Empty_Mark";
					}
					for(auto& i: temp) {
						ss >> i;
						if(i == "*") {
							i = "";
						}
					}
					for(auto& i: temp) {
						if(i == "__Empty_Mark") {
							fail = true;
							goto EXIT_LOOP;
						}
					}
					temp_container.push_back(temp);
				}
			EXIT_LOOP:
				if(fail) {
					std::cerr << "ERROR: Syntax Error !\n";
				}
				else {
					for(auto& i: temp_container) {
						AddRecord(i);
					}
					std::clog << temp_container.size() << " Record Added Successfully !\n";
				}
			}
			else if (basic_command == "sort") {
				if (pos_command == "name" && CheckEmpty(ss)) {
					SortedByName();
				}
				else if (pos_command == "graduate_year" && CheckEmpty(ss)) {
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
			else if(basic_command  == "change" && pos_command == "password" && !CheckEmpty(ss)) {
				std::string new_username, new_password, check_new_password;
				ss >> new_username;
				if(!CheckEmpty(ss)) {
					std::cerr << "ERROR: Syntax Error !\n";
				}
				else {
					while (1) {
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

						if (new_password == check_new_password) {
							break;
						}
						std::cerr << "ERROR: Password Mismatch! \n";
					}
					ChangePassword(new_username, new_password);
					if(new_username == username) {
						password = new_password;
					}
				}
			}
			else if(basic_command  == "change" && pos_command == "privilege" && !CheckEmpty(ss)) {
				std::string tmpusername, privilege;
				ss >> tmpusername >> privilege;
				if((privilege != "User" && privilege != "Administrator") || !CheckEmpty(ss)) {
					std::cerr << "ERROR: Syntax Error !\n";
				}
				else if(tmpusername == username) {
					std::cerr << "ERROR: You Cannot Change The Privilege of Yourself !\n";
				}
				else {
					ChangePrivilege(tmpusername, (privilege == "User")? User: Administrator);
				}
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

	void AdministratorWorker::PrintHello()
	{
		std::cout << username << "(Administrator)@AddressBook\n" 
			<< "Type \"help\" for command line syntax help.\n";
	}

	void AdministratorWorker::PrintHelp()
	{
		std::cout << "Commands Available:\n"
			<< "clear;\t\t\t\t\t\t\t清屏\n"
			<< "help;\t\t\t\t\t\t\t输出帮助\n"
			<< "add record <[record1]...>;\t\t\t\t增加记录\n"
			<< "add user <username> <User|Administrator>;\t\t增加用户\n"
			<< "list record;\t\t\t\t\t\t列出记录\n"
			<< "list user;\t\t\t\t\t\t列出用户\n"
			<< "delete record <index>;\t\t\t\t\t删除记录\n"
			<< "delete user <username>;\t\t\t\t\t删除用户\n"
			<< "showqr phone <index>;\t\t\t\t\t在查询记录后指定以二维码形式输出某记录的电话\n"
			<< "showqr email <index>;\t\t\t\t\t在查询记录后指定以二维码形式输出某记录的电子邮箱\n"
			<< "sort <name|graduate_year>;\t\t\t\t按 姓名|毕业年份 对记录进行排序\n"
			<< "change password <username>;\t\t\t\t修改某帐户密码\n"
			<< "change privilege <username> <User|Administrator>;\t修改某帐户权限\n"
			<< "query record <[key]=[value]>;\t\t\t\t查询记录\n";

	}

	void AdministratorWorker::AddRecord(const RecordType& new_record)
	{
		records.AddRecord(new_record);
	}

	void AdministratorWorker::DeleteRecord(const std::vector<RecordContainer::Iterator>::size_type& index)
	{
		//若输入索引出错，直接抛出错误
		if(index > record_query.size()) {
			std::cerr << "Query Index Doesn\'t Exist !\n";
		}
		else {
			records.DeleteRecord(record_query[index - 1]);
			std::clog << "Record Deleted Successfully !\n";
		}
	}

	void AdministratorWorker::ListUser()
	{
		std::cout << "Here Are The Users:\n";
		user_query = users.ListUser();
		//字段长度
		std::size_t width1 = std::string("UserName").size(),
			width2 = std::string("Password").size();
		for(auto& i: user_query) {
			width1 = std::max(width1, i.size());
			width2 = std::max(width2, std::string(users.QueryPrivilege(i)? "Administrator": "User").size());
		}
		std::cout << std::string(width1 - std::string("UserName").size(), ' ') << "UserName|" 
			<< std::string(width2 - std::string("Password").size(), ' ') << "Password\n";
		for(auto& i: user_query) {
			std::cout << std::string(width1 - i.size(), ' ') << i << "|" 
				<< std::string(width2 - std::string(users.QueryPrivilege(i)? "Administrator": "User").size(), ' ')
				<< (users.QueryPrivilege(i)? "Administrator": "User") << '\n';
		}
	}

	void AdministratorWorker::AddUser(const std::string& new_username, const std::string& new_password, 
		const Privilege& new_privilege)
	{
		try {
			users.AddUser(new_username, new_password, new_privilege);
			std::clog << "User Add Successfully !\n";
		}
		catch(std::runtime_error& e) {
			std::cerr << e.what() << '\n';
		}
	}

	void AdministratorWorker::DeleteUser(const std::string& del_username)
	{
		//不允许删除自己
		if(del_username == username) {
			std::cerr << "You Cannot Delete Yourself !\n";
			return;
		}
		std::set<std::string>::iterator it;
		try {
			users.DeleteUser(del_username);
			if((it = user_query.find(del_username)) != user_query.end()) {
				user_query.erase(it);
			}
			std::clog << "User Deleted Successfully !\n";
		}
		catch(std::runtime_error& e) {
			std::cerr << e.what() << '\n';
		}
	}

	void AdministratorWorker::ChangePassword(std::string& username, std::string& new_password) const
	{
		try {
			users.ChangePassword(username, new_password);
			std::clog << "Password of " << username << " Changed Successfully !\n";
		}
		catch(std::runtime_error& e) {
			std::cerr << e.what() << '\n';
		}
	}

	void AdministratorWorker::ChangePrivilege(const std::string& username, const Privilege& new_privilege)
	{
		//不允许修改为相同权限
		if(new_privilege == users.QueryPrivilege(username)) {
			std::cerr << "ERROR: The New Privilege Should Not Be The Same As The Old !\n";
		}
		try {
			users.ChangePrivilege(username, new_privilege);
			std::clog << "Privilege of " << username << " Changed Successfully !\n";
		}
		catch(std::runtime_error& e) {
			std::cerr << e.what() << '\n';
		}
	}
}
