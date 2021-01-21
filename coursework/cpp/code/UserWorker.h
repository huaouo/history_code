//
// Created by Hua on 2017/5/22.
//

#ifndef ADDRESSBOOK_USERWORKER_H
#define ADDRESSBOOK_USERWORKER_H

#include "AbstractWorker.h"

namespace hua{
	//用户类，继承自抽象用户基类
	class UserWorker: public AbstractWorker {
	public:
		UserWorker() = default;
		//禁止拷贝
		UserWorker(const UserWorker&) = delete;
		UserWorker(const std::string& usrname, const std::string& pwd, UserContainer& uc): 
			AbstractWorker(usrname, pwd, uc) {}
		~UserWorker() = default;

		//禁止拷贝
		UserWorker& operator=(const UserWorker&) = default;

		//解析命令字符串，调用类中相关函数
		void Parse() override;
		//输出登录成功界面
		void PrintHello() override;
		//输出帮助
		void PrintHelp() override;
		//列出所有记录
		void PrintListRecord() override;
		//查询记录
		void PrintQueryRecord(const RecordType& query_key) override;
		//输出电话号码二维码
		void ShowTelephoneQRCode(const std::vector<RecordContainer::Iterator>::size_type& index) override;
		//输出电子邮箱二维码
		void ShowEmailQRCode(const std::vector<RecordContainer::Iterator>::size_type& index) override;
		//按名字排序
		void SortedByName() override;
		//按毕业时间排序
		void SortedByGraduateYear() override;
		//改密码
		void ChangePassword(std::string& new_password) const;
	};
}

#endif
