#include "stdafx.h"
#include<stdlib.h>
#include"User.h"

User::User()
{
	userId=NULL;
	userPassword=NULL;
	userRole=NULL;
}

User::User(char* userId,char* userPassword,char* userRole)
{
	this->userId=new char[strlen(userId)+1];
	strcpy(this->userId,userId);
	this->userPassword=new char[strlen(userPassword)+1];
	strcpy(this->userPassword,userPassword);
	this->userRole=new char[strlen(userRole)+1];
	strcpy(this->userRole,userRole);
}

void User::SetUserId(char* userId)
{
	this->userId=new char[strlen(userId)+1];
	strcpy(this->userId,userId);
}

void User::SetUserPassword(char* userPassword)
{
	this->userPassword=new char[strlen(userPassword)+1];
	strcpy(this->userPassword,userPassword);
}

char* User::GetUserId()
{
	return userId;
}

char* User::GetUserPassword()
{
	return userPassword;
}

void User::ChangePassword(DALTransaction& trans)
{
	DALUpdate updateObj(trans);
	char userId[20],userPassword1[20],userPassword2[20],currentPassword[20];
	cout<<"Enter current password"<<endl;
	cin>>currentPassword;
	if(strcmp(currentPassword,GetUserPassword())==0)
	{
		cout<<"Enter new Password"<<endl;
		cin>>userPassword1;
		cout<<"Re-Enter the password"<<endl;
		cin>>userPassword2;
		if(strcmp(userPassword1,userPassword2)==0)
		{
			strcpy(userId,GetUserId());
			try
			{
				RWTString rwtuserId(userId),rwtuserPassword(userPassword1);
				DALInputHostVar IHVuserId(rwtuserId),IHVuserPassword(rwtuserPassword);

				DALTable UserMasterTableObj(DALTables::UserMaster);
				updateObj.set_table(UserMasterTableObj);

				updateObj.where(UserMasterTableObj[DbUserMaster::UserId]==IHVuserId);
				DALAssignment updateAssignmentObj(UserMasterTableObj[DbUserMaster::UserPswd],IHVuserPassword);

				updateObj.addAssignment(updateAssignmentObj);
				updateObj.execute();

				trans.save();
				trans.commit();

				cout<<"Password successfully changed"<<endl;
			}
			catch(DALException exceptionObj)
			{
				_tcout<<exceptionObj.message();
			}
		}
		else
			cout<<"Password does not match!!!"<<endl;
	}
	else
		cout<<"Wrong Password"<<endl;
}

User::~User()
{
	if(userId)
	{
		delete userId;
		userId=NULL;
	}
	if(userPassword)
	{
		delete userPassword;
		userPassword=NULL;
	}
	if(userRole)
	{
		delete userRole;
		userRole=NULL;
	}
}
