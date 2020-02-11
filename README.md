# Order-Management-System
C++ application to parse a batch file of orders and to test the feasibility of fulfillment based on inventory available

Batch File Design 

	Format:	OrderId~shipTo~Username~Order Status~Create Time~Modify Time~|~itemId~itemQty~...(items)~$
		
	~|~ -to specify there is an item next
	~$ -to specify the end of items in an order

	**All entries made on a single line
	
	File:BatchFile.txt

	Order Naming Convention : Order Id starts with my initials - "SMC" Example: SMC001,SMC002

	Order Status Codes : 1=NEW, 2=UPDATE, 3=CANCEL


Database Order status convention

	1-Incomplete/Unshipped	1-Completed/Shipped	3-Cancelled


Updating Order

	-Updation is performed for items which are present in the original order.Items not present in the original order items are written into error file.


Error Log

	-All the invalid entries in the batch file are written to errorfile - ErrorFile.txt


Shipped Order Report

	-This report will display the shipments done in a given date range.
	-This is done by checking the orders that are marked as shipped and whose last modified date lies the given date range.
	File:ShippedReport.txt
	

Instructions to check Un-Shipped Order Report

	-This report will display the orders that are currently unshipped but will be shipped in the coming week.
	-This is checked by checking the orders that are marked as unshipped and whose last modified date lies in the last week.
	-The application automatically takes the system date as the current date and compares with the modified date.
	File:UnShippedReport.txt

Note:
All entries in the batch file are dated between 10-08-2016 and 20-08-2016 