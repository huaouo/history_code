//
// Created by Hua on 2017/5/11.
//

#include "GlobalDefine.h"
#include "UserContainer.h"
#include "AbstractWorker.h"

int main()
{
    //将窗口列数调大，以正确显示表格
    std::system((std::string("mode con cols=") + std::to_string(hua::cols)).c_str());
	//修改标题
	system("title Alumni Directory Management System(ADMS)");
	hua::UserContainer users;
	//登录并获得用户指针，赋值给抽象用户基类，用于实现多态性
	hua::AbstractWorker* worker = hua::RegisterAndLoginInterface(users);
	system("cls");
	//输出欢迎界面
	worker -> PrintHello();
	while(1) {
		//获得命令字符串并解析
		worker -> Parse();
	}
	return 0;
}
