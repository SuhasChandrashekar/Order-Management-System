#include "stdafx.h"
#include<PkFramework.h>
#include"OMSException.h"
#include"iostream"
using namespace std;

//default constructor
OMSException::OMSException()																			
{
	ErrorMessage=NULL;
}

//parameterized constructor
OMSException::OMSException(char* ErrorMessage)														
{
		this->ErrorMessage=new char[strlen(ErrorMessage)+1];
		strcpy(this->ErrorMessage,ErrorMessage);
}
	
//copy constructor
OMSException::OMSException(const OMSException& exceptionObj)												
{
	ErrorMessage=new char[strlen(exceptionObj.ErrorMessage)+1];
	strcpy(ErrorMessage,exceptionObj.ErrorMessage);
}
	
//Function to return error message
char* OMSException::ErrorText()																	
{
	if(ErrorMessage!=NULL)
		return ErrorMessage;
}

//destructor
OMSException::~OMSException()																			
{
	if(ErrorMessage!=NULL)
	{
		delete ErrorMessage;
		ErrorMessage=NULL;
	}
}