/*used to show the use of event port with buffers*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <iomanip>
#include <iostream>
using namespace std;

using namespace ACM_USER;

char BLACKBOARD_NAME[30]="testboard";

void createBB(BLACKBOARD_ID_TYPE* my_board, RETURN_CODE_TYPE* RETURN_CODE)
{
	CREATE_BLACKBOARD(BLACKBOARD_NAME,200, my_board,RETURN_CODE);
	assert(*RETURN_CODE == NO_ERROR);
}

ACM_EVENT_PORT* createReadEventPort(RETURN_CODE_TYPE* RETURN_CODE)
{
	ACM_EVENT_PORT* ep = new ACM_EVENT_PORT(READ,
		BLACKBOARD_NAME,
		200,
		convertSecsToNano(4),//VALIDITY_PERIOD
		1,
		RETURN_CODE,
		true,
		false);
	assert(*RETURN_CODE == NO_ERROR);
	return ep;
}

ACM_EVENT_PORT* createWriteEventPort(RETURN_CODE_TYPE* RETURN_CODE)
{
	ACM_EVENT_PORT* ep = new ACM_EVENT_PORT(WRITE,
		BLACKBOARD_NAME,
		200,
		convertSecsToNano(4),//VALIDITY_PERIOD
		1,
		RETURN_CODE,
		true,
		false);
	assert(*RETURN_CODE == NO_ERROR);
	return ep;
}

void runTest(ACM_EVENT_PORT* read,ACM_EVENT_PORT* write)
{
	std::cout<<"starting runTest\n";
	BLACKBOARD_ID_TYPE my_board;
	RETURN_CODE_TYPE RETURN_CODE;
	
	try{
		
		char message[200]="This is the blackboard-eventport message";
		MESSAGE_SIZE_TYPE mlength = strlen(message) +1;
		char readData[200];
		bool isValid = true;
		MESSAGE_SIZE_TYPE length;
		LOGGER(APP,"READING FROM EMPTY BBOARD");
		/*
		( MESSAGE_ADDR_TYPE MESSAGE_ADDR, 
								   MESSAGE_SIZE_TYPE* LENGTH,
								   bool& IS_VALID,
								   SYSTEM_TIME_TYPE TIME_OUT,
								   RETURN_CODE_TYPE *RETURN_CODE);
								   */
		read->READ_FROM_EVENT_PORT((APEX_BYTE*)readData,&length, &isValid, convertSecsToNano(5), &RETURN_CODE);
		LOGGER(APP,"Return code from READING FROM EMPTY EVENTPORT - BBOARD is %d",RETURN_CODE);
		
		RETURN_CODE = NO_ERROR;
		cout <<" length is "<<mlength<<endl;
		write->WRITE_TO_EVENT_PORT((APEX_BYTE*)message,mlength, convertSecsToNano(5), &RETURN_CODE);

		assert(RETURN_CODE==NO_ERROR);
		read->READ_FROM_EVENT_PORT((APEX_BYTE*)readData,&length,&isValid, convertSecsToNano(5), &RETURN_CODE);

		
		printf("*************************************************************\n");
		printf("\n");
		printf("\n");
		printf("\n");
		printf("%s",readData);
		printf("\n");
		printf("\n");
		printf("\n");
		printf("*************************************************************\n");
		assert(RETURN_CODE==NO_ERROR);

		read->READ_FROM_EVENT_PORT((APEX_BYTE*)readData,&length, &isValid, convertSecsToNano(5), &RETURN_CODE);
		LOGGER(APP,"Return code from Re-READING EVENTPORT - BBOARD is %d",RETURN_CODE);
		RETURN_CODE = NO_ERROR;


		int i=10;
		int j;
		RETURN_CODE = NO_ERROR;
		write->WRITE_TO_EVENT_PORT((APEX_BYTE*)(&i),sizeof(int), convertSecsToNano(5), &RETURN_CODE);
		read->READ_FROM_EVENT_PORT((APEX_BYTE*)(&j),&length, &isValid, convertSecsToNano(5), &RETURN_CODE);

		printf("*************************************************************\n");
		printf("\n");
		printf("\n");
		printf("\n");
		printf("Integer value %d was read. Size of the message was %ld",j,length);
		printf("\n");
		printf("\n");
		printf("\n");
		printf("*************************************************************\n");
		assert(RETURN_CODE==NO_ERROR);

		RETURN_CODE = NO_ERROR;

		LOGGER(APP,"Checking if return data is stale or not");
		write->WRITE_TO_EVENT_PORT((APEX_BYTE*)(&i),sizeof(int), convertSecsToNano(5), &RETURN_CODE);
		HELPER_CLOCK_NANOSLEEP(convertSecsToNano(5));
		read->READ_FROM_EVENT_PORT((APEX_BYTE*)(&j),&length, &isValid, convertSecsToNano(5), &RETURN_CODE);
		if (isValid)
			LOGGER(APP,"*********Return data is NOT stale!!!***********");
		else
			LOGGER(APP,"********Return data is stale!!!************");

		


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

void runTest1()
{
	std::cout<<"starting runTest1\n";
	BLACKBOARD_ID_TYPE my_board;
	RETURN_CODE_TYPE RETURN_CODE;
	
	try
	{
		createBB(&my_board, &RETURN_CODE);
		ACM_EVENT_PORT* read =   createReadEventPort(&RETURN_CODE);
		ACM_EVENT_PORT* write =  createWriteEventPort(&RETURN_CODE);
		runTest(read,write);
		delete read;
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

void runTest2()
{
	std::cout<<"starting\n";
	RETURN_CODE_TYPE RETURN_CODE;
	
	try
	{
		ACM_EVENT_PORT* read =   createReadEventPort(&RETURN_CODE);
		ACM_EVENT_PORT* write =  createWriteEventPort(&RETURN_CODE);
		runTest(read,write);
		delete read;
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
	bool run_test1 = true;

	if (argc == 1)
		if (strcmp(argv[0],"2")==0)
			run_test1= false;

	if (run_test1)
		runTest1();
	else
		runTest2();

	return 0;
}




		
