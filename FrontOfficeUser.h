#include"User.h"
#include"Order.h"

class FrontOfficeUser:public User
{
public:
	void Menu(DALTransaction& trans);
	void SupplyBatchFile(DALTransaction& trans);
	void validate(DALTransaction& trans);
	void InsertOrder(DALTransaction& trans,int index);
	void UpdateOrder(DALTransaction& trans,int index);
	void CancelOrder(DALTransaction& trans,int index);
};
