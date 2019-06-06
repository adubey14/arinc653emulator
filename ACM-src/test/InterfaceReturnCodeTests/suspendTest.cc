/*
Test of the SUSPEND service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;
PROCESS_ID_TYPE PROCESS_PERIODIC_ID;
PROCESS_ID_TYPE PROCESS_APERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_PERIODIC_NAME="PROCESS_PERIODIC";
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME="PROCESS_APERIODIC";

void PROCESS_PERIODIC (void){
	RETURN_CODE_TYPE PERIODIC_RETURN;
	LOGGER(APP,"PERIODIC STARTED");
	
	TIMED_WAIT(convertSecsToNano(0.5),&PERIODIC_RETURN);
	
	//Tests that no error is returned when correctly suspending a process
	SUSPEND(PROCESS_APERIODIC_ID, &PERIODIC_RETURN);
	if(PERIODIC_RETURN == NO_ERROR ) {
		LOGGER(APP,"Suspend aperiodic test passed!");
		LOGGER(APP,"Return code is: %d" ,PERIODIC_RETURN);	
	}
	else {
		LOGGER(APP,"Suspend aperiodic test failed.");
		LOGGER(APP,"Return code is: %d" ,PERIODIC_RETURN);	
	}

	//Tests that "NO_ACTION" is returned when the specified process is already suspended
	SUSPEND(PROCESS_APERIODIC_ID, &PERIODIC_RETURN);
	if(PERIODIC_RETURN == NO_ACTION ) {
		LOGGER(APP,"Already syspended test passed!");
		LOGGER(APP,"Return code is: %d" ,PERIODIC_RETURN);	
	}
	else {
		LOGGER(APP,"Already suspended test failed.");
		LOGGER(APP,"Return code is: %d" ,PERIODIC_RETURN);	
	}
	
	PROCESS_ID_TYPE PROCESS_NONEXISTENT_ID=10;
	//Tests if the right return code is given when a process tries to suspend a process that does not exist
	SUSPEND(PROCESS_NONEXISTENT_ID, &PERIODIC_RETURN);
	if(PERIODIC_RETURN == INVALID_PARAM) {
		LOGGER(APP,"No such process test passed!");
		LOGGER(APP,"Return code is: %d" ,PERIODIC_RETURN);	
	}
	else {
		LOGGER(APP,"No such process test failed.");
		LOGGER(APP,"Return code is: %d" ,PERIODIC_RETURN);	
	}
	
	//Tests if the right return code is given when a process tries to suspend another process that is in the dormant state
	STOP(PROCESS_APERIODIC_ID, &PERIODIC_RETURN);
	SUSPEND(PROCESS_APERIODIC_ID, &PERIODIC_RETURN);
	if(PERIODIC_RETURN == INVALID_MODE) {
		LOGGER(APP,"Dormant process test passed!");
		LOGGER(APP,"Return code is: %d" ,PERIODIC_RETURN);	
	}
	else {
		LOGGER(APP,"Dormant process test failed.");
		LOGGER(APP,"Return code is: %d" ,PERIODIC_RETURN);	
	}
	while(true){
		USE_INFINITE_LOOP_PROTECTION; //Always put this statement in infinite loops. It creates a thread cancellation point which is required in order to stop the process if necessary.		
		TIMED_WAIT(convertSecsToNano(2.0),&PERIODIC_RETURN);   
	}
	STOP_SELF;
}


void PROCESS_APERIODIC (void){
	LOGGER(APP,"APERIODIC STARTED");
	RETURN_CODE_TYPE APERIODIC_RETURN;

	SUSPEND(PROCESS_APERIODIC_ID, &APERIODIC_RETURN);
	//Tests if the right return code is given when a process tries to suspend itself
	if(APERIODIC_RETURN == INVALID_PARAM) {
		LOGGER(APP,"Don't suspend self test passed!");
		LOGGER(APP,"Return code is: " + APERIODIC_RETURN);	
	}
	else {
		LOGGER(APP,"Don't suspend self test failed.");
		LOGGER(APP,"Return code is: " + APERIODIC_RETURN);	
	}
	
	//start the process APERIODIC
	START(PROCESS_APERIODIC_ID,&APERIODIC_RETURN);

	SUSPEND(PROCESS_PERIODIC_ID, &APERIODIC_RETURN);
	if(APERIODIC_RETURN == INVALID_MODE) {
		LOGGER(APP,"Don't suspend periodic process test passed!");
		LOGGER(APP,"Return code is: %d" ,APERIODIC_RETURN);	
	}
	else {
		LOGGER(APP,"Don't suspend periodic process test failed.");
		LOGGER(APP,"Return code is: %d" ,APERIODIC_RETURN);	
	}

	while(true){
		USE_INFINITE_LOOP_PROTECTION; //Always put this statement in infinite loops. It creates a thread cancellation point which is required in order to stop the process if necessary.		
		TIMED_WAIT(convertSecsToNano(5.0),&APERIODIC_RETURN);   
	}
}

int main (){
	//SET_PARTITION_NAME. Required for Log Identification
	SET_PARTITION_NAME("TESTING_PROCESS_SCHEDULING");

	RETURN_CODE_TYPE RETURN_CODE;

	//! A dummy process fills in the idle gap in the scheduling window.
	//It is necessary to implement suspend service properly. 
	//Note: You can get rid of dummy process if you do not use suspend and resume services.
	//APEX_HELPER_CREATE_AND_START_DUMMY_PROCESS(&RETURN_CODE); 

	//create the process PERIODIC process
	PROCESS_ATTRIBUTE_TYPE PROCESS_PERIODIC_ATTR;
	PROCESS_PERIODIC_ATTR.BASE_PRIORITY=91; // This prioority should be more than the aperiodic priority. Since we are starting the aperiodic process, it is possible that it will take the resources away from the periodic process
	PROCESS_PERIODIC_ATTR.PERIOD=convertSecsToNano(10);
	PROCESS_PERIODIC_ATTR.TIME_CAPACITY=convertSecsToNano(10);
	PROCESS_PERIODIC_ATTR.DEADLINE=HARD;
	PROCESS_PERIODIC_ATTR.ENTRY_POINT=(SYSTEM_ADDRESS_TYPE)PROCESS_PERIODIC;
	PROCESS_PERIODIC_ATTR.STACK_SIZE=0; //We currently do not support restricting stack size in this emulator
	strncpy(PROCESS_PERIODIC_ATTR.NAME,PROCESS_PERIODIC_NAME,MAX_NAME_LENGTH);
	CREATE_PROCESS(&PROCESS_PERIODIC_ATTR,&PROCESS_PERIODIC_ID,	&RETURN_CODE);



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

	//start the process PERIODIC
	START(PROCESS_APERIODIC_ID,&RETURN_CODE);

	//Setting partition mode. This will start the partition.
	LOGGER(APP,"Setting Partition State", APP);
	SET_PARTITION_MODE(NORMAL,&RETURN_CODE);

	return 0; 

}