/*
Test of the DELAYED_START service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;
PROCESS_ID_TYPE PROCESS_APERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME="PROCESS_APERIODIC";
PROCESS_ID_TYPE PROCESS_PERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_PERIODIC_NAME="PROCESS_PERIODIC";
PROCESS_ID_TYPE PROCESS_MAIN_ID;
PROCESS_NAME_TYPE PROCESS_MAIN_NAME="PROCESS_MAIN";

void PROCESS_MAIN (void){
	RETURN_CODE_TYPE RETURN_CODE;
	//Tests that no error is returned when correctly starting an aperiodic process
	DELAYED_START(PROCESS_APERIODIC_ID,convertSecsToNano(3),&RETURN_CODE);
	if(RETURN_CODE == NO_ERROR) {
		LOGGER(APP,"Correct delayed start test(aperiodic) passed!");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}
	else {
		LOGGER(APP,"Correct delayed start test(aperiodic) failed.");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}

	//Tests that the corect code is returned if the specified process is not dormant
	DELAYED_START(PROCESS_APERIODIC_ID,convertSecsToNano(3),&RETURN_CODE);
	if(RETURN_CODE == NO_ACTION) {
		LOGGER(APP,"Already started test passed!");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}
	else {
		LOGGER(APP,"Already started test failed.");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}
	
	PROCESS_ID_TYPE PROCESS_NONEXISTENT_ID = 10;
	//Tests that the correct code is returned if the specified process does not exist
	DELAYED_START(PROCESS_NONEXISTENT_ID,convertSecsToNano(3),&RETURN_CODE);
	if(RETURN_CODE == INVALID_PARAM) {
		LOGGER(APP,"No such process test passed!");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}
	else {
		LOGGER(APP,"No such process test failed.");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}

	//Tests that no error is returned when correctly starting a periodic process
	DELAYED_START(PROCESS_PERIODIC_ID,convertSecsToNano(3),&RETURN_CODE);
	if(RETURN_CODE == NO_ERROR) {
		LOGGER(APP,"Correct delayed start test(periodic) passed!");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}
	else {
		LOGGER(APP,"Correct delayed start test(periodic) failed.");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}
	
	//Tests that the correct code is returned when the delay time is greater than the periodic process's period
	STOP(PROCESS_PERIODIC_ID, &RETURN_CODE);
	sleep(2);
	DELAYED_START(PROCESS_PERIODIC_ID,convertSecsToNano(7),&RETURN_CODE);
	if(RETURN_CODE == INVALID_PARAM) {
		LOGGER(APP,"Wait too long delayed start test(periodic) passed!");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}
	else {
		LOGGER(APP,"Wait too long delayed start test(periodic) failed.");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}
	
}

void PROCESS_PERIODIC (void){
	RETURN_CODE_TYPE PERIODIC_RETURN;
	LOGGER(APP,"PERIODIC STARTED");
	while( true){
		USE_INFINITE_LOOP_PROTECTION; //Always put this statement in infinite loops. It creates a thread cancellation point which is required in order to stop the process if necessary.		
		PERIODIC_WAIT(&PERIODIC_RETURN);  
	}
}


void PROCESS_APERIODIC (void){
	RETURN_CODE_TYPE APERIODIC_RETURN;
	LOGGER(APP,"APERIODIC STARTED");
	while( true){
		USE_INFINITE_LOOP_PROTECTION; //Always put this statement in infinite loops. It creates a thread cancellation point which is required in order to stop the process if necessary.		
		TIMED_WAIT(convertSecsToNano(2.0),&APERIODIC_RETURN);  
	}
}

int main (){
	//SET_PARTITION_NAME. Required for Log Identification
	SET_PARTITION_NAME("TESTING_PROCESS_SCHEDULING");

	RETURN_CODE_TYPE RETURN_CODE;

	//create the process PERIODIC process
	PROCESS_ATTRIBUTE_TYPE PROCESS_PERIODIC_ATTR;
	PROCESS_PERIODIC_ATTR.BASE_PRIORITY=91; // This priority should be more than the aperiodic priority. Since we are starting the aperiodic process, it is possible that it will take the resources away from the periodic process
	PROCESS_PERIODIC_ATTR.PERIOD=convertSecsToNano(5);
	PROCESS_PERIODIC_ATTR.TIME_CAPACITY=convertSecsToNano(5);
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

		//create the process MAIN process
	PROCESS_ATTRIBUTE_TYPE PROCESS_MAIN_ATTR;
	PROCESS_MAIN_ATTR.BASE_PRIORITY=90;
	PROCESS_MAIN_ATTR.PERIOD=INFINITE_TIME_VALUE; //MAIN
	PROCESS_MAIN_ATTR.TIME_CAPACITY=INFINITE_TIME_VALUE;
	PROCESS_MAIN_ATTR.DEADLINE=HARD;
	PROCESS_MAIN_ATTR.ENTRY_POINT=(SYSTEM_ADDRESS_TYPE)PROCESS_MAIN;
	PROCESS_MAIN_ATTR.STACK_SIZE=0; //We currently do not support restricting stack size in this emulator
	strncpy(PROCESS_MAIN_ATTR.NAME,PROCESS_MAIN_NAME,MAX_NAME_LENGTH);
	CREATE_PROCESS(&PROCESS_MAIN_ATTR,&PROCESS_MAIN_ID,	&RETURN_CODE);
	START(PROCESS_MAIN_ID, &RETURN_CODE);

	//Setting partition mode. This will start the partition.
	LOGGER(APP,"Setting Partition State", APP);
	SET_PARTITION_MODE(NORMAL,&RETURN_CODE);

	return 0;

}