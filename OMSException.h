class OMSException
{
private:
	char* ErrorMessage;
public:
	OMSException();
	OMSException(char* Message);
	OMSException(const OMSException& exceptionObj);
	char* ErrorText();
	~OMSException();
};