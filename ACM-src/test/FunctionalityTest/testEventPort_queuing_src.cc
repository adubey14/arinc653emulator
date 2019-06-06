/*
This code shows the usage of ACM Event port which creates 
a seamless abstraction around sampling ports and queueing ports.
It also allows to add an inter module source via sockets.

*/

#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <iomanip>
#include <iostream>
using namespace std;
using namespace ACM_USER;
QUEUING_PORT_NAME_TYPE PORT_NAME_S ="sampling1";
QUEUING_PORT_ID_TYPE PORT_ID_SOURCE;

QUEUING_PORT_NAME_TYPE  PORT_NAME_D ="sampling2";
QUEUING_PORT_ID_TYPE PORT_ID_DEST;

#define portNumber 3314

ACM_EVENT_PORT* createReadEventPort(RETURN_CODE_TYPE* RETURN_CODE)
{
	ACM_EVENT_PORT* ep = new ACM_EVENT_PORT(READ,
		PORT_NAME_S,
		200,
		convertSecsToNano(5),//VALIDITY_PERIOD
		5,
		RETURN_CODE,
		false,
		true);
	assert(*RETURN_CODE == NO_ERROR);
	
	ep->ADD_INTER_MODULE_SOURCE_SOCKET(portNumber,RETURN_CODE);
	assert(*RETURN_CODE == NO_ERROR);
	
	return ep;
}

ACM_EVENT_PORT* createWriteEventPort(RETURN_CODE_TYPE* RETURN_CODE)
{
	ACM_EVENT_PORT* ep = new ACM_EVENT_PORT(WRITE,
		PORT_NAME_D,
		200,
		convertSecsToNano(5),//VALIDITY_PERIOD
		5,
		RETURN_CODE,
		false,
		true);
	assert(*RETURN_CODE == NO_ERROR);
	
	
	//Add Destination URL
	NAME_TYPE hostname="localhost";
	ep->ADD_INTER_MODULE_DESTINATION_URL(hostname,portNumber,RETURN_CODE);
	assert(*RETURN_CODE == NO_ERROR);	
	return ep;
}

void runTest(ACM_EVENT_PORT* write)
{
	std::cout<<"starting runTest\n";
	char message[200]="This is the sampling-eventport message";
	RETURN_CODE_TYPE RETURN_CODE;
		
	try{
		MESSAGE_SIZE_TYPE mlength = strlen(message) +1;
		char readData[200];
		bool isValid = true;
		MESSAGE_SIZE_TYPE length;
		//LOGGER(APP,"READING FROM EMPTY BBOARD");
		/*
		( MESSAGE_ADDR_TYPE MESSAGE_ADDR, 
								   MESSAGE_SIZE_TYPE* LENGTH,
								   bool& IS_VALID,
								   SYSTEM_TIME_TYPE TIME_OUT,
								   RETURN_CODE_TYPE *RETURN_CODE);
								   */
		
		write->WRITE_TO_EVENT_PORT((APEX_BYTE*)message,mlength, convertSecsToNano(5), &RETURN_CODE);
		assert(RETURN_CODE==NO_ERROR);		
		LOGGER(APP,"Write Completed");
		
		

	} // End of Try
	catch(ACM_BASE_EXCEPTION_TYPE& ex){

		LOGGER( ERROR, "ACM_BASE_EXCEPTION: %s",ex.getMessage().c_str());

		return;
	}
	catch (...){
		LOGGER( ERROR,"reached here. Exception Received");
		return;

	}

}



void runTest2()
{
	std::cout<<"starting\n";
	RETURN_CODE_TYPE RETURN_CODE;
	
	try
	{
	//	ACM_EVENT_PORT* read;
		//ACM_EVENT_PORT* read =   createReadEventPort(&RETURN_CODE);
		ACM_EVENT_PORT* write =  createWriteEventPort(&RETURN_CODE);
		runTest(write);
		//delete read;
		delete write;
	}
	catch(ACM_BASE_EXCEPTION_TYPE& ex){

		LOGGER( ERROR, "ACM_BASE_EXCEPTION: %s",ex.getMessage().c_str());

		return;
	}
	catch (...){
		LOGGER( ERROR,"reached here. Exception Received");
		return;

	}

}


int main(const int argc, char* argv[])
{
	runTest2();

	return 0;
}




		
