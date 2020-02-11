#include "stdafx.h"
#include<PkFramework.h>
#include<DAL.h>
#include"Admin.h"

void Admin::AddUser(DALTransaction& trans)
{
	DALInsert insertObj(trans);
	char userId[6],password[15],role[15];
	_tcout<<"Enter the user id"<<endl;
	cin>>userId;
	_tcout<<"Enter the password"<<endl;
	cin>>password;
	_tcout<<"Enter the role"<<endl;
	cin>>role;

	try
	{
		RWTString rwtUserID(userId),rwtPassword(password),rwtRole(role);
	
		DALTable UserMasterTableObj(DALTables::UserMaster);
		insertObj.set_table(UserMasterTableObj);

		DALInputHostVar IHVUserId(rwtUserID),IHVPassword(rwtPassword),IHVRole(rwtRole);
		DALColumn userIDColumn=UserMasterTableObj[DbUserMaster::UserId];
		DALColumn passwordColumn=UserMasterTableObj[DbUserMaster::UserPswd];
		DALColumn roleColumn=UserMasterTableObj[DbUserMaster::UserRole];

		DALColumnList columns;
		columns.append(&userIDColumn);
		columns.append(&passwordColumn);
		columns.append(&roleColumn);

		DALVarList values;

		values.append(&IHVUserId);
		values.append(&IHVPassword);
		values.append(&IHVRole);

		insertObj.set_columns(columns);
		insertObj.set_values(values);

		insertObj.execute();

		trans.save();
		trans.commit();
	}
	catch(DALException exceptionObj)
	{
		_tcout<<exceptionObj.message();
	}
}

void Admin::DeleteUser(DALTransaction& trans)
{
	DALDelete deleteObj(trans);
	char userID[20];
	_tcout<<"Enter the user ID "<<endl;
	cin>>userID;
	RWTString rwtUserID(userID);
	try
	{
	DALTable UserMasterTableObj(DALTables::UserMaster);
	deleteObj.set_table(UserMasterTableObj);

	DALInputHostVar IHVUserID(rwtUserID);

	deleteObj.where(UserMasterTableObj[DbUserMaster::UserId]==IHVUserID);

	deleteObj.execute();
	trans.save();
	trans.commit();
	}
	catch(DALException exceptionObj)
	{
		_tcout<<exceptionObj.message();
	}
}

void Admin::Menu(DALTransaction& trans)
{
	int choice;
	do
	{
		cout<<"\n\n==========Admin User Menu=========="<<endl;
		cout<<"\n\n1.Add user\n2.DeleteUser\n3.Change Role\n4.Change Password\n5.Log Out\n6.Exit\n\nEnter your choice"<<endl;
		cin>>choice;
		switch(choice)
		{
		case 1:AddUser(trans);
			break;
		case 2:DeleteUser(trans);
			break;
		case 3:ModifyUserRole(trans);
			break;
		case 4:ChangePassword(trans);
			break;
		case 5:break;
		case 6:exit(0);
		default:cout<<"Invalid Input"<<endl;
		}
	}while(choice!=5);
}

void Admin::ModifyUserRole(DALTransaction& trans)
{
	DALUpdate updateObj(trans);
	char userID[50];
	char role[50];

	_tcout<<"Ener the user ID:"<<endl;
	cin>>userID;

	_tcout<<"Enter the role"<<endl;
	cin>>role;

	try
	{
		RWTString rwtUserID(userID);
		RWTString rwtRole(role);

		DALInputHostVar IHVUserID(rwtUserID);
		DALInputHostVar IHVRole(rwtRole);

		DALTable UserMasterTableObj(DALTables::UserMaster);
		updateObj.set_table(UserMasterTableObj);

		updateObj.where(UserMasterTableObj[DbUserMaster::UserId]==IHVUserID);

		DALAssignment updateAssignmentObj(UserMasterTableObj[DbUserMaster::UserRole],IHVRole);

		updateObj.addAssignment(updateAssignmentObj);
		updateObj.execute();

		trans.save();
		trans.commit();
	}
	
	catch(DALException exceptionObj)
	{
		_tcout<<exceptionObj.message();
	}
}