/*
Test of the CREATE_BLACKBOARD service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <common.h>
using namespace ACM_USER;
using namespace ACM_EXAMPLE;
BUFFER_NAME_TYPE bufferName= "myBuffer";
BUFFER_ID_TYPE myBufferID;
PROCESS_ID_TYPE PROCESS_APERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME="PROCESS_APERIODIC";

void PROCESS_APERIODIC (void){
	RETURN_CODE_TYPE RETURN_CODE;
	BUFFER_ID_TYPE myBufferID2;
	BUFFER_NAME_TYPE bufferName2= "myBuffer2";
	CREATE_BUFFER (
		/*!in */ bufferName2,
		/*!in */ sizeof(Payload),
		/*!in */ 10,
		/*!in */ FIFO,
		/*!out*/ &myBufferID2,
		/*!out*/ &RETURN_CODE);
	if(RETURN_CODE == INVALID_MODE){
		LOGGER(APP,"Invalid partition mode buffer test Succeeded!");  
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE); 
	}
	else{
		LOGGER(APP,"Invalid partition mode buffer test Failed."); 
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);  
	}
}

int main (){
	//SET_PARTITION_NAME. Required for Log Identification
	SET_PARTITION_NAME("TESTING_PROCESS_SCHEDULING");

	RETURN_CODE_TYPE RETURN_CODE;
	//Tests that no error is returned when correctly creating a buffer
	CREATE_BUFFER (
		/*!in */ bufferName,
		/*!in */ sizeof(Payload),
		/*!in */ 10,
		/*!in */ FIFO,
		/*!out*/ &myBufferID,
		/*!out*/ &RETURN_CODE);
	if(RETURN_CODE == NO_ERROR){
		LOGGER(APP,"Create buffer test Succeeded!");  
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE); 
	}
	else{
		LOGGER(APP,"Create buffer test Failed."); 
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);  
	}
	//Tests that the correct code is returned when attempting to create a buffer with  the name of an existing buffer
	BUFFER_ID_TYPE secondBufferID;
	CREATE_BUFFER (
		/*!in */ bufferName,
		/*!in */ sizeof(Payload),
		/*!in */ 10,
		/*!in */ FIFO,
		/*!out*/ &myBufferID,
		/*!out*/ &RETURN_CODE);
	if(RETURN_CODE == NO_ACTION){
		LOGGER(APP,"Create redundant buffer test Succeeded!");  
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE); 
	}
	else{
		LOGGER(APP,"Create redundant buffer test Failed."); 
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE);  
	}

	//Tests that the correct code is returned when the given maximum message size is out of range
	BUFFER_ID_TYPE myBufferID3;
	BUFFER_NAME_TYPE bufferName3= "mybb3";
	CREATE_BUFFER (
		/*!in */ bufferName3,
		/*!in */ MESSAGE_SIZE_TYPE(2147483648),
		/*!in */ 10,
		/*!in */ FIFO,
		/*!out*/ &myBufferID3,
		/*!out*/ &RETURN_CODE);
	if(RETURN_CODE == INVALID_PARAM){
		LOGGER(APP,"Max size out of range buffer test Succeeded!");  
		LOGGER(APP,"Return code is: %d" ,RETURN_CODE); 
	}
	else{
		LOGGER(APP,"Max size out of range buffer test Failed."); 
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
	CREATE_PROCESS(&PROCESS_APERIODIC_ATTR,&PROCESS_APERIODIC_ID,&RETURN_CODE);

	//start the process PERIODIC
	START(PROCESS_APERIODIC_ID,&RETURN_CODE);

	//Setting partition mode. This will start the partition.
	SET_PARTITION_MODE(NORMAL,&RETURN_CODE);

	return 0;
	}
