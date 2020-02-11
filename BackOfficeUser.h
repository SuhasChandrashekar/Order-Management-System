#include"User.h"

class BackOfficeUser:public User
{
public:
	void Menu(DALTransaction& trans);
	void GenerateReportForDateRange(DALTransaction& trans);
	void ShippedOrders(DALTransaction& trans);
	void ShippedOrdersReport(DALTransaction& trans,int startDd, int startMm, int startYy, int endDd,int endMm, int endYy);
	void UnShippedOrdersReport(DALTransaction& trans);
};