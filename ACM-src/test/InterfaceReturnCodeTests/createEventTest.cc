/*
	Test of the CREATE_EVENT service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;

EVENT_ID_TYPE APERIODIC_EVENT_ID;
EVENT_NAME_TYPE EVENT_NAME = "EVENT1" ;

BUFFER_ID_TYPE TEST_BUFFER_ID;
PROCESS_ID_TYPE PROCESS_APERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME="PROCESS_APERIODIC";

RETURN_CODE_TYPE RETURN_CODE;

void PROCESS_APERIODIC (void)
{
	RETURN_CODE_TYPE APERIODIC_RETURN;
	USE_INFINITE_LOOP_PROTECTION; //Always put this statement in infinite loops. It creates a thread cancellation point which is required in order to stop the process if necessary.		
	CREATE_EVENT (EVENT_NAME,&APERIODIC_EVENT_ID,&APERIODIC_RETURN );
	//Tests that invalid mode is returned when attempting to create an event in the normal partition state
	if(APERIODIC_RETURN == INVALID_MODE){
		LOGGER(APP,"Invalid Mode Test Succeeded!");  
		LOGGER(APP,"Return code is: %d" ,APERIODIC_RETURN); 
	}
	else{
		LOGGER(APP,"Invalid Mode Test Failed."); 
		LOGGER(APP,"Return code is: %d" ,APERIODIC_RETURN); 
	}
	TIMED_WAIT(convertSecsToNano(5), &APERIODIC_RETURN);
	return;
}

int main ()
{
//SET_PARTITION_NAME. Required for Log Identification
SET_PARTITION_NAME("TESTING_EVENT");

//! A dummy process fills in the idle gap in the scheduling window.
//It is necessary to implement suspend service properly. 
//Note: You can get rid of dummy process if you do not use suspend and resume services.
//APEX_HELPER_CREATE_AND_START_DUMMY_PROCESS(&RETURN_CODE); 

//Tests correct creation of events
CREATE_EVENT (EVENT_NAME,&APERIODIC_EVENT_ID,&RETURN_CODE );
if(RETURN_CODE == NO_ERROR){
	LOGGER(APP,"Event creation test Succeeded!");  
	LOGGER(APP,"Return code is: %d" ,RETURN_CODE); 
}
else{
	LOGGER(APP,"Event creation test Failed."); 
	LOGGER(APP,"Return code is: %d" ,RETURN_CODE);  
}

//Tests creation of an event that already exists
CREATE_EVENT (EVENT_NAME,&APERIODIC_EVENT_ID,&RETURN_CODE );
if(RETURN_CODE == NO_ACTION){
	LOGGER(APP,"Redundant event test Succeeded!");  
	LOGGER(APP,"Return code is: %d" ,RETURN_CODE);  
}
else{
	LOGGER(APP,"Redundant event test Failed."); 
	LOGGER(APP,"Return code is: %d" ,RETURN_CODE);  
}

//create the process APERIODIC process
PROCESS_ATTRIBUTE_TYPE PROCESS_APERIODIC_ATTR;
PROCESS_APERIODIC_ATTR.BASE_PRIORITY=90;
PROCESS_APERIODIC_ATTR.PERIOD=INFINITE_TIME_VALUE; //APeriodic
PROCESS_APERIODIC_ATTR.TIME_CAPACITY=INFINITE_TIME_VALUE;
PROCESS_APERIODIC_ATTR.DEADLINE=HARD;
PROCESS_APERIODIC_ATTR.ENTRY_POINT=(SYSTEM_ADDRESS_TYPE)PROCESS_APERIODIC;
PROCESS_APERIODIC_ATTR.STACK_SIZE=0; //We currently do not support restricting stack size in this emulator
strncpy(PROCESS_APERIODIC_ATTR.NAME,PROCESS_APERIODIC_NAME,MAX_NAME_LENGTH);
CREATE_PROCESS(&PROCESS_APERIODIC_ATTR,&PROCESS_APERIODIC_ID,	&RETURN_CODE);

//start the process APERIODIC
START(PROCESS_APERIODIC_ID,&RETURN_CODE);

//Setting partition mode. This will start the partition.
LOGGER(APP,"Setting Partition State", APP);
SET_PARTITION_MODE(NORMAL,&RETURN_CODE);

return 0;

}