/*
Test of the SET_PARTITION_MODE service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;

PROCESS_ID_TYPE PROCESS_PERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_PERIODIC_NAME="PROCESS_PERIODIC";
RETURN_CODE_TYPE RETURN_CODE;

void PROCESS_PERIODIC (void)
{

	LOGGER(APP,"Begin PERIODIC.");	
	//Tests that no error is returned when setting the partition to the normal mode
	if (RETURN_CODE == NO_ERROR)
	{
		LOGGER(APP,"Set normal status test passed!");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}
	else
	{
		LOGGER(APP,"Set normal status test failed.");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}
	
	SET_PARTITION_MODE(NORMAL,&RETURN_CODE);
	//Tests that the correct code is returned when setting the partition to the normal mode when it is already in the normal state
	if (RETURN_CODE == NO_ACTION)
	{
		LOGGER(APP,"Already normal test passed!");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}
	else
	{
		LOGGER(APP,"Already normal test failed.");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}
	OPERATING_MODE_TYPE NONEXISTENT_MODE;
	SET_PARTITION_MODE(NONEXISTENT_MODE,&RETURN_CODE);
	//Tests that the correct code is returned when setting the partition to an undefined mode
	if (RETURN_CODE == INVALID_PARAM)
	{
		LOGGER(APP,"Undefined mode test passed!");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}
	else
	{
		LOGGER(APP,"Undefined mode test failed.");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}

	SET_PARTITION_MODE(IDLE,&RETURN_CODE);
	//Tests that no error is returned when setting the partition to the idle mode - should cause an error since we only support the normal mode
	if (RETURN_CODE == NO_ERROR)
	{
		LOGGER(APP,"Set idle test passed!");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}
	else
	{
		LOGGER(APP,"Set idle test failed.");
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);	
	}
}

int main ()
{
//SET_PARTITION_NAME. Required for Log Identification
SET_PARTITION_NAME("TESTING_PROCESS_SCHEDULING");
//! A dummy process fills in the idle gap in the scheduling window.
//It is necessary to implement suspend service properly. 
//Note: You can get rid of dummy process if you do not use suspend and resume services.
//APEX_HELPER_CREATE_AND_START_DUMMY_PROCESS(&RETURN_CODE); 

//create the process PERIODIC process
PROCESS_ATTRIBUTE_TYPE PROCESS_PERIODIC_ATTR;
PROCESS_PERIODIC_ATTR.BASE_PRIORITY=91; // This prioority should be more than the aperiodic priority. Since we are starting the aperiodic process, it is possible that it will take the resources away from the periodic process
PROCESS_PERIODIC_ATTR.PERIOD=convertSecsToNano(5);
PROCESS_PERIODIC_ATTR.TIME_CAPACITY=convertSecsToNano(5);
PROCESS_PERIODIC_ATTR.DEADLINE=HARD;
PROCESS_PERIODIC_ATTR.ENTRY_POINT=(SYSTEM_ADDRESS_TYPE)PROCESS_PERIODIC;
PROCESS_PERIODIC_ATTR.STACK_SIZE=0; //We currently do not support restricting stack size in this emulator
strncpy(PROCESS_PERIODIC_ATTR.NAME,PROCESS_PERIODIC_NAME,MAX_NAME_LENGTH);
CREATE_PROCESS(&PROCESS_PERIODIC_ATTR,&PROCESS_PERIODIC_ID,	&RETURN_CODE);

//start the process PERIODIC
START(PROCESS_PERIODIC_ID,&RETURN_CODE);

//Setting partition mode. This will start the partition.
LOGGER(APP,"Setting Partition State", APP);
SET_PARTITION_MODE(NORMAL,&RETURN_CODE);

return 0;

}