#ifndef _USER_H
#define _USER_H
#include "stdafx.h"
#include<PkFramework.h>
#include<DAL.h>

class User
{
private:
	char* userId;
	char* userPassword;
	char* userRole;

public:
	User();
	User(char* userId,char* userPassword,char* role);
	void SetUserId(char* userId);
	void SetUserPassword(char* userPassword);
	char* GetUserId();
	char* GetUserPassword();
	virtual void Menu(DALTransaction& trans)=0;
	void ChangePassword(DALTransaction& trans);
	virtual ~User();
};

#endif