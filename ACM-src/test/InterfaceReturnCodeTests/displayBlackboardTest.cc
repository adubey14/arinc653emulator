/*
Test of the DISPLAY_BLACKBOARD service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <common.h>
using namespace ACM_USER;
using namespace ACM_EXAMPLE;

BLACKBOARD_NAME_TYPE BB_NAME= "MY_BB";
BLACKBOARD_ID_TYPE BB_ID;
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME="PROCESS_APERIODIC";
PROCESS_ID_TYPE PROCESS_APERIODIC_ID;

void PROCESS_APERIODIC (void){
	Payload myLoad;
	myLoad.data = 1;
	myLoad.time=ACM_USER::currentTimeInNanoSeconds();
	RETURN_CODE_TYPE RETURN_CODE;
	//Tests that no error is returned when correctly displaying a blackboard
	DISPLAY_BLACKBOARD(BB_ID, (APEX_BYTE*)(&myLoad), sizeof(myLoad), &RETURN_CODE);
	if(RETURN_CODE == NO_ERROR){
		LOGGER(APP,"Display blackboard test Succeeded!");  
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE); 
	}
	else{
		LOGGER(APP,"Display blackboard test Failed."); 
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);  
	}

	//Tests that the correct code is returned when attempting to display a nonexistent blackboard
	BLACKBOARD_ID_TYPE NONEXISTENT_BB_ID = 10;
	DISPLAY_BLACKBOARD (NONEXISTENT_BB_ID, (APEX_BYTE*)(&myLoad), sizeof(myLoad), &RETURN_CODE);
	if(RETURN_CODE == INVALID_PARAM){
		LOGGER(APP,"Blackboard does not exist test Succeeded!");  
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE); 
	}
	else{
		LOGGER(APP,"Blackboard does not exist test Failed."); 
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);  
	}
	return;
}

int main ()
{
RETURN_CODE_TYPE RETURN_CODE;

//SET_PARTITION_NAME. Required for Log Identification
SET_PARTITION_NAME("TESTING_PROCESS_SCHEDULING");

//! A dummy process fills in the idle gap in the scheduling window.
//It is necessary to implement suspend service properly. 
//Note: You can get rid of dummy process if you do not use suspend and resume services.
//APEX_HELPER_CREATE_AND_START_DUMMY_PROCESS(&RETURN_CODE); 


//create the process APERIODIC process with no invalid parameters
PROCESS_ATTRIBUTE_TYPE PROCESS_APERIODIC_ATTR;
PROCESS_APERIODIC_ATTR.BASE_PRIORITY=91; 
PROCESS_APERIODIC_ATTR.PERIOD=INFINITE_TIME_VALUE;
PROCESS_APERIODIC_ATTR.TIME_CAPACITY=INFINITE_TIME_VALUE;
PROCESS_APERIODIC_ATTR.DEADLINE=HARD;
PROCESS_APERIODIC_ATTR.ENTRY_POINT=(SYSTEM_ADDRESS_TYPE)PROCESS_APERIODIC;
PROCESS_APERIODIC_ATTR.STACK_SIZE=0; //We currently do not support restricting stack size in this emulator
strncpy(PROCESS_APERIODIC_ATTR.NAME,PROCESS_APERIODIC_NAME,MAX_NAME_LENGTH);
CREATE_PROCESS(&PROCESS_APERIODIC_ATTR,&PROCESS_APERIODIC_ID,&RETURN_CODE);

CREATE_BLACKBOARD (
	/*!in */ BB_NAME,
	/*!in */ sizeof(Payload),
	/*!out*/ &BB_ID,
	/*!out*/ &RETURN_CODE);

START(PROCESS_APERIODIC_ID, &RETURN_CODE);

//Setting partition mode. This will start the partition.
LOGGER(APP,"Setting Partition State", APP);
SET_PARTITION_MODE(NORMAL,&RETURN_CODE);

return 0;
 
}