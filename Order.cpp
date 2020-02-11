#include "stdafx.h"
#include<PkFramework.h>
#include<DAL.h>
#include"Order.h"
#include<string.h>
#include<iostream>
using namespace std;

Order::Order()
{
	orderId=NULL;
	shipTo=NULL;
	orderStatus=NULL;
	createDateTime=NULL;
	modifyDateTime=NULL;
	iList=NULL;
}

Order::Order(char* orderId,char* shipTo,char* orderStatus,char* createDateTime,char* modifyDateTime)
{
	this->orderId=new char[strlen(orderId)+1];
	strcpy(this->orderId,orderId);
	this->shipTo=new char[strlen(shipTo)+1];
	strcpy(this->shipTo,shipTo);
	this->orderStatus=new char[strlen(orderStatus)+1];
	strcpy(this->orderStatus,orderStatus);
	this->createDateTime=new char[strlen(createDateTime)+1];
	strcpy(this->createDateTime,createDateTime);
	this->modifyDateTime=new char[strlen(modifyDateTime)+1];
	strcpy(this->modifyDateTime,modifyDateTime);
	iList=NULL;
}

void Order::AddNewLineItem(LineItem* newItem)
{
	if(iList==NULL)
	{
		iList=newItem;
	}
	else if(iList->GetNextItem()==NULL)
	{
		iList->SetNextItem(newItem);
	}
	else
	{
		LineItem* temp=iList;
		while(temp->GetNextItem()!=NULL)
		{
			temp=temp->GetNextItem();
		}
		temp->SetNextItem(newItem);
	}
}

char* Order::GetOrderStatus()
{
	return orderStatus;
}

LineItem* Order::GetItemList()
{
	return iList;
}

void Order::DisplayOrderDetails()
{
	cout<<"\n"<<orderId<<" "<<shipTo<<" "<<orderStatus<<" "<<createDateTime<<" "<<modifyDateTime<<endl;
	if(iList!=NULL)
	{
		for(LineItem* temp=iList;temp!=NULL;temp=temp->GetNextItem())
		{
			temp->DisplayItemDetails();
		}
	}
}

char* Order::GetOrderId()
{
	return orderId;
}
	
char* Order::GetShipTo()
{
	return shipTo;
}

char* Order::GetCreateDateTime()
{
	return createDateTime;
}

char* Order::GetModifyDateTime()
{
	return modifyDateTime;
}

Order::~Order()
{
	if(orderId)
	{
		delete orderId;
		orderId=NULL;
	}
	if(shipTo)
	{
		delete shipTo;
		shipTo=NULL;
	}
	if(orderStatus)
	{
		delete orderStatus;
		orderStatus=NULL;
	}
	if(createDateTime)
	{
		delete createDateTime;
		createDateTime=NULL;
	}
	if(modifyDateTime)
	{
		delete modifyDateTime;
		modifyDateTime=NULL;
	}
	LineItem *temp=iList;
	while(temp!=NULL)
	{
		iList=iList->GetNextItem();
		delete temp;
		temp=iList;
	}
	temp=NULL;
	iList=NULL;
}