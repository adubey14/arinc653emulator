#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <common.h>
using namespace ACM_USER;

using namespace ACM_EXAMPLE;
PROCESS_ID_TYPE PROCESS_PERIODIC_ID;

PROCESS_NAME_TYPE PROCESS_PERIODIC_NAME="PROCESS_PERIODIC";
SAMPLING_PORT_NAME_TYPE SAMPLING_PORT_NAME="SAMPLE_PORT_DEST"; //This port will be visible if you list message queues on your system
//Note you need to create a different port with different name for partition 2.
SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID;


void PROCESS_PERIODIC (void)
{
	static Payload  data; //Just used to prevent multiple instantiations. You can make this non static.
	RETURN_CODE_TYPE RETURN_CODE;
	MESSAGE_SIZE_TYPE LENGTH;
	VALIDITY_TYPE VALIDITY;
	LOGGER(USER,"Reading sampling port");
	READ_SAMPLING_MESSAGE (
			/*in */ SAMPLING_PORT_ID,
			/*out*/ (MESSAGE_ADDR_TYPE)&data,
			/*out*/ &LENGTH,
			/*out*/ &VALIDITY,
			/*out*/ &RETURN_CODE );
			
	if(LENGTH==sizeof(	Payload))
	{
			if(VALIDITY== INVALID) LOGGER(USER,"Read Invalid message %f at %lld",data.data,data.time);
			if(VALIDITY== VALID) LOGGER(USER,"Read Valid message %f at %lld",data.data,data.time);			
	}
	else
	{
	
		LOGGER(USER,"Port is empty");
	
		   
	}
}

int main ()
{
//SET_PARTITION_NAME. Required for Log Identification
SET_PARTITION_NAME("Partition 2");

RETURN_CODE_TYPE RETURN_CODE;


CREATE_SAMPLING_PORT(
			/*in */ SAMPLING_PORT_NAME,
			/*in */ sizeof(Payload),
			/*in */ DESTINATION,
			/*in */ convertSecsToNano(4),
			/*out*/ &SAMPLING_PORT_ID,
			/*out*/ &RETURN_CODE );
ACM_ASSERT (RETURN_CODE==NO_ERROR);



//! A dummy process fills in the idle gap in the scheduling window.
//It is necessary to implement suspend service properly. 
//Note: You can get rid of dummy process if you do not use suspend and resume services.
//APEX_HELPER_CREATE_AND_START_DUMMY_PROCESS(&RETURN_CODE); 
//ACM_ASSERT(RETURN_CODE==NO_ERROR);

//create the process PERIODIC process
PROCESS_ATTRIBUTE_TYPE PROCESS_PERIODIC_ATTR;
PROCESS_PERIODIC_ATTR.BASE_PRIORITY=90;
PROCESS_PERIODIC_ATTR.PERIOD=convertSecsToNano(4); //PERIODIC
PROCESS_PERIODIC_ATTR.TIME_CAPACITY=convertSecsToNano(4);
PROCESS_PERIODIC_ATTR.DEADLINE=HARD;
PROCESS_PERIODIC_ATTR.ENTRY_POINT=(SYSTEM_ADDRESS_TYPE)PROCESS_PERIODIC;
PROCESS_PERIODIC_ATTR.STACK_SIZE=0; //We currently do not support restricting stack size in this emulator
strncpy(PROCESS_PERIODIC_ATTR.NAME,PROCESS_PERIODIC_NAME,MAX_NAME_LENGTH);
CREATE_PROCESS(&PROCESS_PERIODIC_ATTR,&PROCESS_PERIODIC_ID,	&RETURN_CODE);
ACM_ASSERT(RETURN_CODE==NO_ERROR);

//start the process PERIODIC
START(PROCESS_PERIODIC_ID,&RETURN_CODE);
ACM_ASSERT(RETURN_CODE==NO_ERROR);

//Setting partition mode. This will start the partition.
LOGGER(USER,"Setting Partition State");
SET_PARTITION_MODE(NORMAL,&RETURN_CODE);

return 0;

}