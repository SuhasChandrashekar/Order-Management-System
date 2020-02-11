#ifndef _ORDER_H
#define _ORDER_H
#include"LineItem.h"

class Order
{
private:
	char* orderId;
	char* shipTo;
	char* orderStatus;
	char* createDateTime;
	char* modifyDateTime;
	LineItem* iList;

public:
	Order();
	Order(char* orderId,char* shipTo,char* orderStatus,char* createDateTime,char* modDateTime);
	void AddNewLineItem(LineItem* newItem);
	char* GetOrderStatus();
	LineItem* GetItemList();
	void DisplayOrderDetails();
	char* GetOrderId();
	char* GetShipTo();
	char* GetCreateDateTime();
	char* GetModifyDateTime();
	~Order();
};

#endif