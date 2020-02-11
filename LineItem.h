#ifndef _LINEITEM_H
#define _LINEITEM_H

class LineItem
{
private:
	char* itemId;
	char* itemQuantity;
	LineItem* nextItem;

public:
	LineItem();
	LineItem(char* itemId,char* itemQuantity);
	void SetNextItem(LineItem* nextItem);
	LineItem* GetNextItem();
	char* GetItemId();
	char* GetItemQuantity();
	void DisplayItemDetails();
	~LineItem();
};

#endif