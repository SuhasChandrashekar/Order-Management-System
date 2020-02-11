#include "stdafx.h"
#include<PkFramework.h>
#include<DAL.h>
#include"FrontOfficeUser.h"
#include"Order.h"
#include"OMSException.h"
#include<fstream>
#include<string>
#include<iostream>
using namespace std;

Order *orderObj[25];
int orderCount;

std::ofstream ErrorFile("ErrorFile.txt",std::ofstream::out);

bool CheckItemInDb(DALTransaction& trans,char* itemId);
bool CheckOrderInDb(DALTransaction& trans,int index);
bool CheckWhetherShipped(DALTransaction& trans,int index);
bool CheckWhetherCancelled(DALTransaction& trans,int index);

void FrontOfficeUser::Menu(DALTransaction& trans)
{
	int choice;
	do
	{
		cout<<"\n\n==========Front Office User Menu=========="<<endl;
		cout<<"\n\n1.Supply batch file\n2.Change Password\n3.Log Out\n4.Exit\n"<<endl;
		cout<<"\nEnter your choice"<<endl;
		cin>>choice;
		switch(choice)
		{
		case 1:SupplyBatchFile(trans);
			break;
		case 2:ChangePassword(trans);
			break;
		case 3:break;
		case 4:exit(0);
		default:cout<<"Invalid input";
		}
	}while(choice!=3);
}

void FrontOfficeUser::SupplyBatchFile(DALTransaction& trans)
{
	char fileName[20],orderId[20],shipTo[20],orderStatus[20],createTime[20],modifyTime[20],temp[20],itemId[20],itemQuantity[20];
	orderCount=0;
	LineItem *itemObj;
	cout<<"\nEnter the file name"<<endl;
	cin>>fileName;
	ifstream myfile(fileName);
	try
	{
		//throw an exception if file cannot be opened
		if(!myfile.is_open())
		{
			throw OMSException("File cannot be opened");										
		}
		while(!myfile.eof())
		{
			myfile.getline(orderId,20,'~');
			myfile.getline(shipTo,20,'~');
			myfile.getline(orderStatus,20,'~');
			myfile.getline(createTime,20,'~');
			myfile.getline(modifyTime,20,'~');
			myfile.getline(temp,20,'~');
			//create an order obj for each order
			orderObj[orderCount]=new Order(orderId,shipTo,orderStatus,createTime,modifyTime);	
			//$ specifies the end of items in an order in my batch file
			while(strcmp(temp,"$")!=0)															
			{
				myfile.getline(itemId,20,'~');
				myfile.getline(itemQuantity,20,'~');
				myfile.getline(temp,20,'~');
				//create a lineItem Obj for each item and link it the order Obj
				LineItem *itemObj=new LineItem(itemId,itemQuantity);							
				orderObj[orderCount]->AddNewLineItem(itemObj);
				if(myfile.eof())
					break;
			}
			//orderObj[orderCount]->DisplayOrderDetails();    
			orderCount++;
		}
		cout<<"\n\nBatch file successfully submitted"<<endl;
		validate(trans);
		}
	catch(OMSException& exceptionObj)
	{
		cout<<exceptionObj.ErrorText()<<endl;
	}
}

void FrontOfficeUser::validate(DALTransaction& trans)
{
	int counter;
	bool valid,orderExists,shipped,cancelled;
	char itemId[20],itemQuantity[20];
	//validate for each order
	for(counter=0;counter<orderCount;counter++)
	{
		valid=true;
		if(orderObj[counter]->GetItemList()!=NULL)
		{
			for(LineItem* temp=orderObj[counter]->GetItemList();temp!=NULL && valid==1;temp=temp->GetNextItem())
			{
				strcpy(itemId,temp->GetItemId());
				strcpy(itemQuantity,temp->GetItemQuantity());
				//check for valid quantity and write in errorfile if invalid
				if(atoi(itemQuantity)<0)
				{
					valid=false;
					ErrorFile<<"Quantity of Item "<<temp->GetItemId()<<" in Order "<<orderObj[counter]->GetOrderId()<<" is invalid!!! Order discarded"<<endl;
				}
				//check for valid item and write in errorfile if invalid
				else if(!CheckItemInDb(trans,itemId))
				{
					valid=false;
					ErrorFile<<"Item "<<temp->GetItemId()<<" in Order "<<orderObj[counter]->GetOrderId()<<" is invalid!!! Order discarded"<<endl;
				}
			}
		}
		if(valid)
		{
			//for insert
			if(strcmp(orderObj[counter]->GetOrderStatus(),"1")==0)
			{
				orderExists=CheckOrderInDb(trans,counter);
				cancelled=CheckWhetherCancelled(trans,counter);
				shipped=CheckWhetherShipped(trans,counter);
				if(!orderExists)
					InsertOrder(trans,counter);
				else
					ErrorFile<<"Order "<<orderObj[counter]->GetOrderId()<<" Already Present...Cannot Insert"<<endl;
			}
			//for update
			else if(strcmp(orderObj[counter]->GetOrderStatus(),"2")==0)
			{	
				orderExists=CheckOrderInDb(trans,counter);
				cancelled=CheckWhetherCancelled(trans,counter);
				shipped=CheckWhetherShipped(trans,counter);
				if(orderExists)
					{
						if(!cancelled)
						{
							if(!shipped)
								UpdateOrder(trans,counter);
							else
								ErrorFile<<"Update Failed...Order "<<orderObj[counter]->GetOrderId()<<" already shipped"<<endl;
						}
						else
							ErrorFile<<"Update Failed...Order "<<orderObj[counter]->GetOrderId()<<" already cancelled"<<endl;
					}
				else
					ErrorFile<<"Update Failed...Order "<<orderObj[counter]->GetOrderId()<<" Not Present"<<endl;
			}
			//for cancel
			else if(strcmp(orderObj[counter]->GetOrderStatus(),"3")==0)
			{
				orderExists=CheckOrderInDb(trans,counter);
				cancelled=CheckWhetherCancelled(trans,counter);
				shipped=CheckWhetherShipped(trans,counter);
				if(orderExists)
				{
					if(!cancelled)
					{
						if(!shipped)
							CancelOrder(trans,counter);
						else
							ErrorFile<<"Cancellation Failed...Order "<<orderObj[counter]->GetOrderId()<<" already shipped"<<endl;
					}
					else
						ErrorFile<<"Cancellation Failed...Order "<<orderObj[counter]->GetOrderId()<<" already cancelled"<<endl;
				}
				else
					ErrorFile<<"Cancellation Failed...Order "<<orderObj[counter]->GetOrderId()<<" Not Present"<<endl;
			}
		}
	}
}

void FrontOfficeUser::InsertOrder(DALTransaction& trans,int index)
{
	cout<<"Inserting..."<<endl;
	char orderId[20],shipTo[20],orderStatus[20],createDateTime[20],modifyDateTime[20],orderId1[20],itemId[20],itemQuantity[20],createDay[5],createMonth[5],createYear[5],modifyDay[5],modifyMonth[5],modifyYear[5];
	int createDd,createMm,createYy,modDd,modMm,modYy;
	strcpy(orderId,orderObj[index]->GetOrderId());
	strcpy(shipTo,orderObj[index]->GetShipTo());
	strcpy(orderStatus,"1");
	strcpy(createDateTime,orderObj[index]->GetCreateDateTime());
	strcpy(modifyDateTime,orderObj[index]->GetModifyDateTime());
	
	sscanf(createDateTime,"%[^-]-%[^-]-%[^-]",createDay,createMonth,createYear);
	createDd=atoi(createDay);
	createMm=atoi(createMonth);
	createYy=atoi(createYear);
	sscanf(modifyDateTime,"%[^-]-%[^-]-%[^-]",modifyDay,modifyMonth,modifyYear);
	modDd=atoi(modifyDay);
	modMm=atoi(modifyMonth);
	modYy=atoi(modifyYear);
	
	try
	{
		DALInsert insertObj(trans);

		DALTable OrderHdrTableObj(DALTables::OrderHdr);
		insertObj.set_table(OrderHdrTableObj);
	
		DALDate createDate(createDd,createMm,createYy);
		DALDate modifiedDate(modDd,modMm,modYy);
				
		RWTString rwtorderId(orderId),rwtshipTo(shipTo),rwtorderStatus(orderStatus);
		DALInputHostVar IHVorderId(rwtorderId),IHVshipTo(rwtshipTo),IHVorderStatus(rwtorderStatus),IHVcreateDateTime(createDate),IHVmodifyDateTime(modifiedDate);
	
		DALColumn orderIdColumn=OrderHdrTableObj[DbOrderHdr::OrderId];
		DALColumn shipToColumn=OrderHdrTableObj[DbOrderHdr::ShipTo];
		DALColumn orderStatusColumn=OrderHdrTableObj[DbOrderHdr::OrderStatus];
		DALColumn createDateTimeColumn=OrderHdrTableObj[DbOrderHdr::CreateDateTime];
		DALColumn modifyDateTimeColumn=OrderHdrTableObj[DbOrderHdr::ModDateTime];

		DALColumnList columns;
		columns.append(&orderIdColumn);
		columns.append(&shipToColumn);
		columns.append(&orderStatusColumn);
		columns.append(&createDateTimeColumn);
		columns.append(&modifyDateTimeColumn);

		DALVarList values;

		values.append(&IHVorderId);
		values.append(&IHVshipTo);
		values.append(&IHVorderStatus);
		values.append(&IHVcreateDateTime);
		values.append(&IHVmodifyDateTime);

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
	try
	{
		strcpy(orderId1,orderObj[index]->GetOrderId());
		if(orderObj[index]->GetItemList()!=NULL)
		{
			for(LineItem* temp=orderObj[index]->GetItemList();temp!=NULL;temp=temp->GetNextItem())
			{
				DALInsert insertObj(trans);
				DALTable OrderDtlTableObj(DALTables::OrderDtl);
				insertObj.set_table(OrderDtlTableObj);
				strcpy(itemId,temp->GetItemId());
				strcpy(itemQuantity,temp->GetItemQuantity());
				RWTString rwtitemId(itemId),rwtitemQuantity(itemQuantity),rwtorderId1(orderId1);
				DALInputHostVar IHVitemId(rwtitemId),IHVitemQuantity(rwtitemQuantity),IHVorderId1(rwtorderId1);

				DALColumn orderId1Column=OrderDtlTableObj[DbOrderDtl::OrderId];
				DALColumn itemIdColumn=OrderDtlTableObj[DbOrderDtl::SkuId];
				DALColumn itemQuantityColumn=OrderDtlTableObj[DbOrderDtl::OrdQty];

				DALColumnList columns;
				columns.append(&orderId1Column);
				columns.append(&itemIdColumn);
				columns.append(&itemQuantityColumn);

				DALVarList values;
				values.append(&IHVorderId1);
				values.append(&IHVitemId);
				values.append(&IHVitemQuantity);

				insertObj.set_columns(columns);
				insertObj.set_values(values);

				insertObj.execute();

				trans.save();
				trans.commit();
			}
		}
	}
	catch(DALException exceptionObj)
	{
		_tcout<<exceptionObj.message();
	}
}

void FrontOfficeUser::UpdateOrder(DALTransaction& trans,int index)
{
	cout<<"Updating.."<<endl;
	char orderId[20],itemId[20],itemQuantity[20],modifyDate[20],modifyDay[50],modifyMonth[50],modifyYear[5];
	int modDd,modMm,modYy;
	strcpy(orderId,orderObj[index]->GetOrderId());
	strcpy(modifyDate,orderObj[index]->GetModifyDateTime());
	sscanf(modifyDate,"%[^-]-%[^-]-%[^-]",modifyDay,modifyMonth,modifyYear);
	modDd=atoi(modifyDay);
	modMm=atoi(modifyMonth);
	modYy=atoi(modifyYear);
	try
	{
		DALUpdate updateObj1(trans);
						
		DALTable OrderHdrTableObj(DALTables::OrderHdr);
		updateObj1.set_table(OrderHdrTableObj);
		
		DALDate modifiedDate(modDd,modMm,modYy);

		RWTString rwtorderId(orderId);
		DALInputHostVar IHVorderId(rwtorderId),IHVmodifyDate(modifiedDate);

		updateObj1.where(OrderHdrTableObj[DbOrderHdr::OrderId]==IHVorderId);
		DALAssignment updateAssignmentObj(OrderHdrTableObj[DbOrderHdr::ModDateTime],IHVmodifyDate);

		updateObj1.addAssignment(updateAssignmentObj);
		updateObj1.execute();


		trans.save();
		trans.commit();
	}
	catch(DALException exceptionObj)
	{
		_tcout<<exceptionObj.message();
	}
	try
	{
		if(orderObj[index]->GetItemList()!=NULL)
		{
			for(LineItem* temp=orderObj[index]->GetItemList();temp!=NULL;temp=temp->GetNextItem())
			{
				DALUpdate updateObj(trans);
				DALTable OrderDtlTableObj(DALTables::OrderDtl);
				updateObj.set_table(OrderDtlTableObj);
				strcpy(itemId,temp->GetItemId());
				strcpy(itemQuantity,temp->GetItemQuantity());
				RWTString rwtitemId(itemId),rwtitemQuantity(itemQuantity),rwtorderId(orderId);
				DALInputHostVar IHVitemId(rwtitemId),IHVitemQuantity(rwtitemQuantity),IHVorderId(rwtorderId);
				
				updateObj.where(OrderDtlTableObj[DbOrderDtl::OrderId]==IHVorderId && OrderDtlTableObj[DbOrderDtl::SkuId]==IHVitemId);

				DALAssignment updateAssignmentObj(OrderDtlTableObj[DbOrderDtl::OrdQty],IHVitemQuantity);

				updateObj.addAssignment(updateAssignmentObj);
				updateObj.execute();
				
				//check for invalid item in update record
				int updated=updateObj.get_rowCount();
				if(!updated)
				{
					ErrorFile<<"Item "<<temp->GetItemId()<<" in Order "<<orderObj[index]->GetOrderId()<<" is invalid!!! Item discarded"<<endl;
				}

				trans.save();
				trans.commit();
			}
		}
	}
	catch(DALException exceptionObj)
	{
		_tcout<<exceptionObj.message();
	}
}

void FrontOfficeUser::CancelOrder(DALTransaction& trans,int index)
{
	DALUpdate updateObj(trans);
	cout<<"Cancelling.."<<endl;
	char orderId[20],orderStatus[20];
	strcpy(orderId,orderObj[index]->GetOrderId());
	strcpy(orderStatus,"3");
	try
	{
		RWTString rwtorderId(orderId),rwtorderStatus(orderStatus);
		DALInputHostVar IHVorderId(rwtorderId),IHVorderStatus(rwtorderStatus);
			
		DALTable OrderHdrTableObj(DALTables::OrderHdr);
		updateObj.set_table(OrderHdrTableObj);
		
		updateObj.where(OrderHdrTableObj[DbOrderHdr::OrderId]==IHVorderId);
		DALAssignment updateAssignmentObj(OrderHdrTableObj[DbOrderHdr::OrderStatus],IHVorderStatus);

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

bool CheckItemInDb(DALTransaction& trans,char* itemId)
{
	DALSelect selectObj(trans);
	DALTable ItemMasterObj(DALTables::ItemMaster);

	RWTString rwtItemId;
	DALHostVar HVItemId(rwtItemId);
	
	selectObj.addSelect(ItemMasterObj[DbItemMaster::SkuId],HVItemId);

	char itemId1[20];
	selectObj.execute();
	
	while(selectObj.next())
	{
		wcstombs(itemId1,rwtItemId,20);
		if(strcmp(itemId,itemId1)==0)
			return true;
	}
	return false;
}

bool CheckOrderInDb(DALTransaction& trans,int index)
{
	char orderId[20];

	DALSelect selectObj(trans);
	DALTable OrderHdrObj(DALTables::OrderHdr);
	
	RWTString rwtorderId;
	DALHostVar HVorderId(rwtorderId);

	strcpy(orderId,orderObj[index]->GetOrderId());
	selectObj.addSelect(OrderHdrObj[DbOrderHdr::OrderId],HVorderId);

	char orderId1[20];
	selectObj.execute();

	while(selectObj.next())
	{
		wcstombs(orderId1,rwtorderId,20);
		if(strcmp(orderId,orderId1)==0)
			return true;
	}
	return false;
}

bool CheckWhetherShipped(DALTransaction& trans,int index)
{
	DALSelect selectObj(trans);
	DALTable OrderHdrObj(DALTables::OrderHdr);
	
	char orderId[20],orderId1[20];
	int orderStatus1,orderStatus=2;
	
	RWTString rwtorderId;
	DALHostVar HVorderId(rwtorderId),HVorderStatus(orderStatus1);

	strcpy(orderId,orderObj[index]->GetOrderId());

	selectObj.addSelect(OrderHdrObj[DbOrderHdr::OrderId],HVorderId);
	selectObj.addSelect(OrderHdrObj[DbOrderHdr::OrderStatus],HVorderStatus);

	selectObj.execute();

	while(selectObj.next())
	{
		wcstombs(orderId1,rwtorderId,20);
		if(orderStatus1==orderStatus && strcmp(orderId1,orderId)==0)
			return true;
	}
	return false;
}

bool CheckWhetherCancelled(DALTransaction& trans,int index)
{
	DALSelect selectObj(trans);
	DALTable OrderHdrObj(DALTables::OrderHdr);
	
	char orderId[20],orderId1[20];
	int orderStatus1,orderStatus=3;
	
	RWTString rwtorderId;
	DALHostVar HVorderId(rwtorderId),HVorderStatus(orderStatus1);

	strcpy(orderId,orderObj[index]->GetOrderId());

	selectObj.addSelect(OrderHdrObj[DbOrderHdr::OrderId],HVorderId);
	selectObj.addSelect(OrderHdrObj[DbOrderHdr::OrderStatus],HVorderStatus);

	selectObj.execute();

	while(selectObj.next())
	{
		wcstombs(orderId1,rwtorderId,20);
		if(orderStatus1==orderStatus && strcmp(orderId1,orderId)==0)
			return true;
	}
	return false;
}
