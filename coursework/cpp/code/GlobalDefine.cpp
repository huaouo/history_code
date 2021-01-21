//
// Created by Hua on 2017/5/31.
//

#include <iostream>
#include <sstream>
#include <conio.h>
#include <Windows.h>
#include "GlobalDefine.h"
#include "UserContainer.h"

namespace hua {
	//全局变量，含义见 GlobalDefine.h
	const char* CLEAR = "cls";
	const char* UserFile = "User.dat";
	const char* RecordFile = "Record.dat";
	const char* PROMPT1 = ">>>";
	const char* PROMPT2 = "...";
	const int SLEEP_TIME = 2000;
	extern const short cols = 150;

	bool CheckEmpty(std::stringstream& ss)
	{
		std::string temp, templine;
		ss >> temp;
		if(ss) {
			//先取出来，再放进去
			std::getline(ss, templine);
			ss.clear();
			ss << temp << ' ' << templine;
			return false;
		}
		else {
			return true;
		}
	}

	std::string GetPassword()
	{
		std::string password;
		int cnt = 0;
		char tmp;
		while((tmp = _getch()) != '\r') {
			if(tmp == '\b') {
				if(cnt > 0) {
					//删除屏幕上一个字符
					--cnt;
					_putch('\b');
					_putch(' ');
					_putch('\b');
					password = password.substr(0, password.size() - 1);
				}
			}
			else {
				//输出 *
				_putch('*');
				password += tmp;
				++cnt;
			}
		}
		_putch('\n');
		return password;
	}

	bool HaveSpace(const std::string& s)
	{
		if(s.find(' ') < s.size()) {
			return true;
		}
		return false;
	}

	AbstractWorker* RegisterAndLoginInterface(UserContainer& users)
	{
		if(users.Empty()) {
			std::cerr << "No Users Aviable, You May Register The First Administrator !\n";
			std::string username, password, check_password;
			while(1) {
				std::cout << "UserName: ";
				std::getline(std::cin, username);
				if(username == "") {
					std::cerr << "ERROR: Empty UserName Is Forbidden !\n";
					continue;
				}
				if(!hua::HaveSpace(username)) {
					break;
				}
				std::cerr << "ERROR: UserName Should Not Consist of Space !\n";
			}

			while(1) {
				while(1) {
					std::cout << "Password: ";
					password = hua::GetPassword();
					if(password == "") {
						std::cerr << "ERROR: Empty Password Is Forbidden !\n";
						continue;
					}
					if(!hua::HaveSpace(password)) {
						break;
					}
					std::cerr << "ERROR: Password Should Not Consist of Space !\n";
				}

				while(1) {
					std::cout << "Password Again: ";
					check_password = hua::GetPassword();
					if(check_password == "") {
						std::cerr << "ERROR: Empty Password Is Forbidden !\n";
						continue;
					}
					if(!hua::HaveSpace(check_password)) {
						break;
					}
					std::cerr << "ERROR: Password Should Not Consist of Space !\n";
				}
				if(password == check_password) {
					users.RegisterFirstUser(username, password);
					std::cout << "First Administrator Registered Successfully !\n";
					Sleep(SLEEP_TIME);
					system("cls");
					break;
				}
				std::cerr << "ERROR: Password Mismatch !\n";
			}
		}

		std::string username, password;
		//计数，尝试次数不能超过三次
		for(int attempt = 0; attempt < 3; ++attempt) {
			std::cout << "Please Login !\n";
			while(1) {
				std::cout << "UserName: ";
				std::getline(std::cin, username);
				if(username == "") {
					std::cerr << "ERROR: Empty UserName Is Forbidden !\n";
					continue;
				}
				if(!hua::HaveSpace(username)) {
					break;
				}
				std::cerr << "ERROR: UserName Should Not Consist of Space !\n";
			}
			while(1) {
				std::cout << "Password: ";
				password = hua::GetPassword();
				if(password == "") {
					std::cerr << "ERROR: Empty Password Is Forbidden !\n";
					continue;
				}
				if(!hua::HaveSpace(password)) {
					break;
				}
				std::cerr << "ERROR: Password Should Not Consist of Space !\n";
			}
			hua::AbstractWorker* ret = nullptr;
			try {
				ret = users.Login(username, password);
			} catch(std::runtime_error& e) {
				std::cerr << e.what() << '\n';
				continue;
			}
			return ret;
		}
		std::cerr << "You've Failed 3 Times, Bye ! \n";
		//暂停片刻
		Sleep(SLEEP_TIME);
		//直接退出
		exit(0);
	}
}
