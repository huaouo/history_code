//
// Created by Hua on 2017/5/11.
//

#include "GlobalDefine.h"
#include "UserContainer.h"
#include "AbstractWorker.h"

int main()
{
    //������������������ȷ��ʾ���
    std::system((std::string("mode con cols=") + std::to_string(hua::cols)).c_str());
	//�޸ı���
	system("title Alumni Directory Management System(ADMS)");
	hua::UserContainer users;
	//��¼������û�ָ�룬��ֵ�������û����࣬����ʵ�ֶ�̬��
	hua::AbstractWorker* worker = hua::RegisterAndLoginInterface(users);
	system("cls");
	//�����ӭ����
	worker -> PrintHello();
	while(1) {
		//��������ַ���������
		worker -> Parse();
	}
	return 0;
}
