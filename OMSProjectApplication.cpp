// OMSProjectApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<PkFramework.h>
#include<DAL.h>
#include"Admin.h"
#include"FrontOfficeUser.h"
#include"BackOfficeUser.h"
#include"OMSException.h"
#include<iostream>
#include<string>
#define _CRTDBG_MAP_ALLOC
#include<stdlib.h>
#include<crtdbg.h>
using namespace std;

User* CreateObj(char* userID,char* userPassword,char* userRole);
void AuthenticateUser(DALTransaction& trans);

Admin adminObj;
FrontOfficeUser frontObj;
BackOfficeUser backObj;

void _tmain(int argc, _TCHAR* argv[])
{
	{
		MANH::DAL::init();
		try
		{
			DALTransaction trans=DALTransBuilder::newTransaction(TEXT("TESTDB"),TEXT("TESTUSER"),TEXT("TESTPASSWORD"));
			_tcout<<"Connection established"<<endl;
			AuthenticateUser(trans);
		}
		catch(DALException exceptionObj)
		{
			_tcout<<exceptionObj.message();
		}
		system("pause");
	}
	_CrtDumpMemoryLeaks();
}

void AuthenticateUser(DALTransaction& trans)
{
	DALSelect selectObj(trans);
	DALTable UserMasterTableObj(DALTables::UserMaster);
	char choice='y';
	int match=0;
	char userID[50],userPassword[50];
	char userID1[50],userPassword1[50],userRole1[25];
	do
	{
		cout<<"\n\nEnter User ID"<<endl;
		cin>>userID;
		cout<<"Enter the password"<<endl;
		cin>>userPassword;
	
		try
		{
		
			RWTString rwtUserID,rwtPassword,rwtRole;
			DALHostVar HVUserID(rwtUserID);
			DALHostVar HVPassword(rwtPassword);
			DALHostVar HVRole(rwtRole);
			selectObj.addSelect(UserMasterTableObj[DbUserMaster::UserId],HVUserID);
			selectObj.addSelect(UserMasterTableObj[DbUserMaster::UserPswd],HVPassword);
			selectObj.addSelect(UserMasterTableObj[DbUserMaster::UserRole],HVRole);
			selectObj.execute();
			while(selectObj.next())
			{
				wcstombs(userID1,rwtUserID,50);
				wcstombs(userPassword1,rwtPassword,50);
				wcstombs(userRole1,rwtRole,25);
				if(strcmp(userID1,userID)==0 && strcmp(userPassword1,userPassword)==0)
					{
						cout<<"Authenticated..."<<endl;
						match=1;
						break;
					}
			}
		}
	
		catch(DALException exceptionObj)
		{
		_tcout<<exceptionObj.message();
		}

		try
		{
			//throw an exception for invalid log-in
			if(!match)
				throw OMSException("Invalid log In!!!");											
			User *Obj=CreateObj(userID,userPassword,userRole1);
			if(Obj!=NULL)
				Obj->Menu(trans);
			else
				throw OMSException("Invalid role!!!");
		}
		
		catch(OMSException& exceptionObj)
		{
			cout<<exceptionObj.ErrorText()<<endl;
		}

		cout<<"\nDo you want to Re-Login?(y/n)"<<endl;
		cin>>choice;
		system("cls");
	}while(choice=='y'||choice=='Y');
}

//Return specific Object based on user role
User* CreateObj(char* userID,char* userPassword,char* userRole)
{
	if(strcmp(userRole,"Admin")==0)
	{
		adminObj.SetUserId(userID);
		adminObj.SetUserPassword(userPassword);
		return &adminObj;
	}
	if(strcmp(userRole,"FrontOffice")==0)
	{
		frontObj.SetUserId(userID);
		frontObj.SetUserPassword(userPassword);
		return &frontObj;
	}
	if(strcmp(userRole,"BackOffice")==0)
	{
		backObj.SetUserId(userID);
		backObj.SetUserPassword(userPassword);
		return &backObj;
	}
	else
		return NULL;
}
	
