/*
Test of the WAIT_SEMAPHORE_STATUS service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;

SEMAPHORE_NAME_TYPE semName= "mysem";
SEMAPHORE_ID_TYPE mysemid;
SEMAPHORE_NAME_TYPE semName2= "mysem2";
SEMAPHORE_ID_TYPE mysemid2;
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME="PROCESS_APERIODIC";
PROCESS_ID_TYPE PROCESS_APERIODIC_ID;

void PROCESS_APERIODIC (void){
	RETURN_CODE_TYPE RETURN_CODE;
	//Tests that no error is returned when correctly waiting on a semaphore
	WAIT_SEMAPHORE (
		/*!in */ mysemid,
		/*!in */ convertSecsToNano(0.5),
		/*!out*/ &RETURN_CODE );
	if(RETURN_CODE == NO_ERROR){
		LOGGER(APP,"Wait semaphore test Succeeded!");  
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE); 
	}
	else{
		LOGGER(APP,"Wait semaphore test Failed."); 
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);  
	}
	//Tests that the correct code is returned once the given timeout has expired
	TIMED_WAIT(convertSecsToNano(0.6), &RETURN_CODE);
	if(RETURN_CODE == TIMED_OUT){
		LOGGER(APP,"Timeout test Succeeded!");  
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE); 
	}
	else{
		LOGGER(APP,"Timeout test Failed."); 
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);  
	}
	//Tests that the correct code is returned when attempting to wait on a nonexistent semaphore
	SEMAPHORE_ID_TYPE nonexistentsemid = 10;
	WAIT_SEMAPHORE (
		/*!in */ nonexistentsemid,
		/*!in */ convertSecsToNano(0.5),
		/*!out*/ &RETURN_CODE );
	if(RETURN_CODE == INVALID_PARAM){
		LOGGER(APP,"Semaphore does not exist test Succeeded!");  
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE); 
	}
	else{
		LOGGER(APP,"Semaphore does not exist test Failed."); 
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);  
	}
	//Tests that the correct code is returned when the semaphore's value is <= 0 and the timeout is 0
	WAIT_SEMAPHORE (
		/*!in */ mysemid2,
		/*!in */ convertSecsToNano(0),
		/*!out*/ &RETURN_CODE );
	if(RETURN_CODE == NOT_AVAILABLE){
		LOGGER(APP, "Zero timeout test Succeeded!");  
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE); 
	}
	else{
		LOGGER(APP,"Zero timeout test Failed."); 
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

START(PROCESS_APERIODIC_ID, &RETURN_CODE);

CREATE_SEMAPHORE (
/*!in */ semName,
/*!in */ SEMAPHORE_VALUE_TYPE (0),
/*!in */ SEMAPHORE_VALUE_TYPE (1),
/*!in */ FIFO,
/*!out*/ &mysemid,
/*!out*/ &RETURN_CODE);

CREATE_SEMAPHORE (
/*!in */ semName2,
/*!in */ SEMAPHORE_VALUE_TYPE (0),
/*!in */ SEMAPHORE_VALUE_TYPE (1),
/*!in */ FIFO,
/*!out*/ &mysemid2,
/*!out*/ &RETURN_CODE);

//Setting partition mode. This will start the partition.
LOGGER(APP,"Setting Partition State", APP);
SET_PARTITION_MODE(NORMAL,&RETURN_CODE);

return 0;
 
}