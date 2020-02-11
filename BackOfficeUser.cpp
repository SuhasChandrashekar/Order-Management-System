#include "stdafx.h"
#include<PkFramework.h>
#include<DAL.h>
#include"BackOfficeUser.h"
#include"OMSException.h"
#include<fstream>
#include<string>
#include<iomanip>
#include<iostream>
using namespace std;
using namespace MANH::DAL;

std::ofstream ShippedReportFile("ShippedReport.txt",std::ofstream::out);
std::ofstream UnShippedReportFile("UnShippedReport.txt",std::ofstream::out);

void BackOfficeUser::Menu(DALTransaction& trans)
{
	int choice;
	do
	{
		cout<<"\n\n==========Back Office User Menu=========="<<endl;
		cout<<"\n\n1.Generate Shipped order report\n2.Generate Un-Shipped order report\n3.Change Password\n4.Log Out\n5.Exit\n"<<endl;
		cout<<"\nEnter your choice"<<endl;
		cin>>choice;
		switch(choice)
		{
		case 1:ShippedOrders(trans);
				break;
		case 2:UnShippedOrdersReport(trans);
				break;
		case 3:ChangePassword(trans);
				break;
		case 4:break;
		case 5:exit(0);
		default:cout<<"Invalid input";
		}
	}while(choice!=4);
}

void BackOfficeUser::ShippedOrders(DALTransaction& trans)
{
	char startDate[20],endDate[20],startDay[20],startMonth[20],startYear[20],endDay[20],endMonth[20],endYear[20];
	int startDd,startMm,startYy,endDd,endMm,endYy;
	cout<<"Enter the date range"<<endl;
	cout<<"Enter the start date"<<endl;
	cin>>startDate;
	cout<<"Enter the end date"<<endl;
	cin>>endDate;
	sscanf(startDate,"%[^-]-%[^-]-%[^-]",startDay,startMonth,startYear);
	startDd=atoi(startDay);
	startMm=atoi(startMonth);
	startYy=atoi(startYear);
	sscanf(endDate,"%[^-]-%[^-]-%[^-]",endDay,endMonth,endYear);
	endDd=atoi(endDay);
	endMm=atoi(endMonth);
	endYy=atoi(endYear);
	ShippedReportFile<<"=========================SHIPPED ORDERS REPORT=========================\n\n"<<endl;
	ShippedReportFile<<"Shipments done from "<<startDate<<" to "<<endDate<<endl;
	ShippedOrdersReport(trans,startDd,startMm,startYy,endDd,endMm,endYy);
}

void BackOfficeUser::ShippedOrdersReport(DALTransaction& trans,int startDd, int startMm, int startYy, int endDd,int endMm, int endYy)
{
	char orderId[20],shipTo[20],userId[20],itemId[20],orderStatus[20];
	int itemQuantity=0,count=0;
	strcpy(orderStatus,"2");

	try
	{
		DALSelect selectObj1(trans);
		DALSelect selectObj2(trans);
		DALTable orderHdrTableObj(DALTables::OrderHdr);
		DALTable orderDtlTableObj(DALTables::OrderDtl);
	
		DALDate startDate(startDd,startMm,startYy);
		DALDate endDate(endDd,endMm,endYy);
		
		//check for invalid dates
		if(!startDate.isValid() && !endDate.isValid())
		{
			throw OMSException("Invalid start and end dates!!!");
		}
	
		if(!startDate.isValid())
		{
			throw OMSException("Invalid start date!!!");
		}

		if(!endDate.isValid())
		{
			throw OMSException("Invalid end date!!!");
		}

		if(startDate>endDate)
		{
			throw OMSException("Invalid date range!!!");
		}
	
		DALDate modDate;
		DALDate createDate;

		DALConstant order("%SMC%");

		RWTString rwtOrderStatus1(orderStatus),rwtOrderStatus2(orderStatus);
		DALInputHostVar IHVStartDate(startDate),IHVEndDate(endDate),IHVStatus(rwtOrderStatus1);
		DALColumn ModifiedDateColumn(orderHdrTableObj[DbOrderHdr::ModDateTime]);
		DALCondition completedOrdersBetweenStartAndEndDate(datepart(ModifiedDateColumn).between(IHVStartDate, IHVEndDate));

		RWTString rwtOrderId,rwtShipTo,rwtUserId,rwtItemId;

		DALHostVar HVOrderId(rwtOrderId);
		DALHostVar HVShipTo(rwtShipTo);
		DALHostVar HVOrderStatus(rwtOrderStatus2);
		DALHostVar HVUserId(rwtUserId);
		DALHostVar HVCreateDateTime(createDate);
		DALHostVar HVModDateTime(modDate);

		DALHostVar HVItemId(rwtItemId);
		DALHostVar HVItemQty(itemQuantity);

		selectObj1.addSelect(orderHdrTableObj[DbOrderHdr::OrderId],HVOrderId);
		selectObj1.addSelect(orderHdrTableObj[DbOrderHdr::ShipTo],HVShipTo);
		selectObj1.addSelect(orderHdrTableObj[DbOrderHdr::OrderStatus],HVOrderStatus);
		selectObj1.addSelect(orderHdrTableObj[DbOrderHdr::UserId],HVUserId);
		selectObj1.addSelect(orderHdrTableObj[DbOrderHdr::CreateDateTime],HVCreateDateTime);
		selectObj1.addSelect(orderHdrTableObj[DbOrderHdr::ModDateTime],HVModDateTime);

		selectObj1.where(completedOrdersBetweenStartAndEndDate && orderHdrTableObj[DbOrderHdr::OrderStatus] == IHVStatus && (orderHdrTableObj[DbOrderHdr::OrderId].like(order) ));
		selectObj1.addOrderBy(orderHdrTableObj[DbOrderHdr::ModDateTime],DAL_SORTASCENDING);
		
		selectObj1.execute();
		while(selectObj1.next())
		{
			count++;
			
			wcstombs(orderId,rwtOrderId,20);
			wcstombs(shipTo,rwtShipTo,20);
			wcstombs(userId,rwtUserId,20);

			ShippedReportFile<<std::left<<setw(15)<<"\n\n\tOrder Id"<<setfill(' ')<<": "<<orderId<<endl;
			ShippedReportFile<<std::left<<setw(15)<<"\tShipped To"<<setfill(' ')<<": "<<shipTo<<endl;
			ShippedReportFile<<std::left<<setw(15)<<"\tShipped date"<<setfill(' ')<<": "<<modDate.dayOfMonth()<<"-"<<modDate.monthName()<<"-"<<modDate.year()<<endl<<endl;
			
			ShippedReportFile<<"\tItem Id\t"<<"ItemQty"<<endl;
			ShippedReportFile<<"\t-------\t"<<"-------"<<endl;
			
			DALInputHostVar IHVOrderId(rwtOrderId);
			selectObj2.addSelect(orderDtlTableObj[DbOrderDtl::SkuId],HVItemId);	//
			selectObj2.addSelect(orderDtlTableObj[DbOrderDtl::OrdQty],HVItemQty);	//
			selectObj2.where(orderDtlTableObj[DbOrderDtl::OrderId]==IHVOrderId);	//
			selectObj2.execute();

			while(selectObj2.next())
			{
				wcstombs(itemId,rwtItemId,30);
				ShippedReportFile<<"\t"<<itemId;
				ShippedReportFile<<"\t"<<itemQuantity<<endl;
			}
		}
		cout<<"Shipped order report sucessfully generated..."<<endl;
		ShippedReportFile<<"------------------------------------------------------"<<endl;
		ShippedReportFile<<"Count of orders shipped during specified date:"<<count<<endl;
	}
	catch(OMSException& exceptionObj)
	{
		cout<<exceptionObj.ErrorText()<<endl;
	}
	catch(DALException exObj)
	{
		_tcout<< exObj.message();
	}
}

void BackOfficeUser::UnShippedOrdersReport(DALTransaction& trans)
{
	DALSelect selectObj1(trans);
	DALSelect selectObj2(trans);
	DALTable orderHdrTableObj(DALTables::OrderHdr);
	DALTable orderDtlTableObj(DALTables::OrderDtl);

	char orderId[20],itemId[20],shipTo[100],itemDesc[100],userId[20],orderStatus[20];
	strcpy(orderStatus,"1");

	DALConstant order("%SMC%");

	DALDate modDate,shipDate;
	RWTString rwtOrderId,rwtShipTo,rwtUserId,rwtItemDesc,rwtOrderStatus(orderStatus);
	
	DALInputHostVar IHVStartDate(DALDate::now()-7), IHVEndDate(DALDate::now());
	DALColumn ModifiedDateColumn(orderHdrTableObj[DbOrderHdr::ModDateTime]);
	
	DALCondition completedOrdersBetweenStartAndEndDate(datepart(ModifiedDateColumn).between(IHVStartDate, IHVEndDate));
	
	DALInputHostVar IHVStatus(rwtOrderStatus);	
	
	DALHostVar HVOrderId(rwtOrderId);
	DALHostVar HVShipTo(rwtShipTo);
	DALHostVar HVOrderStatus(rwtOrderStatus);
	DALHostVar HVUserId(rwtUserId);
	DALHostVar HVModDateTime(modDate) ,HVSkuDesc(rwtItemDesc);
	
	int itemQty=0,count=0;
	
	RWTString rwtItemId;
	DALHostVar HVItemId(rwtItemId), HVItemQty(itemQty);
	UnShippedReportFile<<"=========================UN-SHIPPED ORDERS REPORT=========================\n\n"<<endl;
	try
	{
		selectObj1.addSelect(orderHdrTableObj[DbOrderHdr::OrderId],HVOrderId);
		selectObj1.addSelect(orderHdrTableObj[DbOrderHdr::ShipTo],HVShipTo);
		selectObj1.addSelect(orderHdrTableObj[DbOrderHdr::UserId],HVUserId);
		selectObj1.addSelect(orderHdrTableObj[DbOrderHdr::ModDateTime],HVModDateTime);
		
		selectObj1.where(completedOrdersBetweenStartAndEndDate && (orderHdrTableObj[DbOrderHdr::OrderId].like(order)) && orderHdrTableObj[DbOrderHdr::OrderStatus]==IHVStatus); 
		selectObj1.addOrderBy(orderHdrTableObj[DbOrderHdr::ModDateTime],DAL_SORTASCENDING);
		selectObj1.execute();
		while(selectObj1.next())
		{
			count++;

			wcstombs(orderId,rwtOrderId,30);
			wcstombs(shipTo,rwtShipTo,100);
			wcstombs(userId,rwtUserId,30);

			shipDate = modDate+7;
			UnShippedReportFile<<std::left<<setw(35)<<"\n\n\t   Scheduled Shipping Date"<<setfill(' ')<<": "<<shipDate.dayOfMonth()<<"-"<<shipDate.monthName()<<"-"<<shipDate.year()<<endl;

			DALInputHostVar IHVOrderId(rwtOrderId);
			selectObj2.addSelect(orderDtlTableObj[DbOrderDtl::SkuId],HVItemId);
			selectObj2.addSelect(orderDtlTableObj[DbOrderDtl::OrdQty],HVItemQty);
			selectObj2.where(orderDtlTableObj[DbOrderDtl::OrderId]==IHVOrderId);
			selectObj2.execute();
			UnShippedReportFile<<std::left<<setw(35)<<"\t   Order Id"<<setfill(' ')<<": "<<orderId<<endl;
			UnShippedReportFile<<"\t\t   Item Id\t\t"<<"ItemQty"<<endl;
			while(selectObj2.next())
			{
				wcstombs(itemId,rwtItemId,30);
				UnShippedReportFile<<"\t\t   "<<itemId<<"\t";
				UnShippedReportFile<<"\t\t"<<itemQty<<endl;
			}
		}
		cout<<"Un-Shipped order report sucessfully generated..."<<endl;
		UnShippedReportFile<<"------------------------------------------------------"<<endl;
		UnShippedReportFile<<"Count of orders to be completed by this week:"<<count<<endl;
	}
	catch(DALException exObj)
	{
		_tcout<< exObj.message();
	}
}