#include "stdafx.h"
#include<PkFramework.h>
#include<DAL.h>
#include"LineItem.h"
#include<string.h>
#include<iostream>
using namespace std;

LineItem::LineItem()
{
	itemId=NULL;
	itemQuantity=NULL;
	nextItem=NULL;
}

LineItem::LineItem(char* itemId,char* itemQuantity)
{
	this->itemId=new char[strlen(itemId)+1];
	strcpy(this->itemId,itemId);
	this->itemQuantity=new char[strlen(itemQuantity)+1];
	strcpy(this->itemQuantity,itemQuantity);
	nextItem=NULL;
}

LineItem* LineItem::GetNextItem()
{
	return nextItem;
}

void LineItem::SetNextItem(LineItem* nextItem)
{
	this->nextItem=nextItem;
}

char* LineItem::GetItemId()
{
	return itemId;
}

char* LineItem::GetItemQuantity()
{
	return itemQuantity;
}

void LineItem::DisplayItemDetails()
{
	cout<<itemId<<" "<<itemQuantity<<endl;
}

LineItem::~LineItem()
{
	if(itemId)
	{
		delete itemId;
		itemId=NULL;
	}
	if(itemQuantity)
	{
		delete itemQuantity;
		itemQuantity=NULL;
	}
	if(nextItem)
	{
		delete nextItem;
		nextItem=NULL;
	}
}