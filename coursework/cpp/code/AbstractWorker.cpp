//
// Created by Hua on 2017/5/24.
//

#include <string>
#include <iostream>
#include <string>
#include <cstdlib>
#include "AbstractWorker.h"
#include "GlobalDefine.h"

namespace hua{
	void AbstractWorker::PrintMainPrompt() const
	{
		std::cout << PROMPT1;
	}

	void AbstractWorker::PrintSecondaryPrompt() const
	{
		std::cout << PROMPT2;
	}

	void AbstractWorker::Clear() const
	{
		system(CLEAR);
	}

	void AbstractWorker::Exit() const
	{
		//�����˳�
		std::exit(0);
	}

	std::string AbstractWorker::GetCommandString()
	{
		//��󷵻ص��ַ���
		std::string ret;
		//��ʱ�ַ���
		std::string temp;
		bool firstline = true;
		PrintMainPrompt();
		while(1) {
			//��ȡһ��
			std::getline(std::cin, temp);
			if(temp == "" && firstline == true) {
				PrintMainPrompt();
				continue;
			}
			firstline = false;
			//��ȡ��֮ǰ���ַ���
			std::size_t semicolon_pos = temp.find(';');
			if(semicolon_pos < temp.size()) {
				//֮�������зǿո��ַ���ֱ���׳��﷨����
				for(std::size_t i = semicolon_pos + 1; i < temp.size(); ++i) {
					if(temp[i] != ' ') {
						throw CommandLineSyntaxError(std::string("ERROR: Syntax Error !"));
					}
				}
				ret += (" " + temp.substr(0, semicolon_pos));
				std::size_t equal_pos;
				bool first_equal = true;
				//ȥ=���ڵ�һ���Ⱥ�ǰ����EQUAL_FORM_MARK
				while((equal_pos = ret.find('=')) < ret.size()) {
					ret.replace(equal_pos, 1, " ");
					if(first_equal) {
						while(--equal_pos > 0 && ret[equal_pos] == ' ');
						while(--equal_pos > 0 && ret[equal_pos] != ' ');
						ret.replace(equal_pos, 1, " EQUAL_FORM_MARK ");
						first_equal = false;
					}
				}
				return ret;
			}
			ret += (" " + temp);
			PrintSecondaryPrompt();
		}
	}
}

