/*
This file tests the use of read write locks. This locks are an extension to the ARINC-653 API.
*/

#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <iomanip>
#include <string.h>
using namespace ACM_USER;

PROCESS_ID_TYPE PERIODIC_PROCESS_ID_1=0;
PROCESS_ID_TYPE APERIODIC_PROCESS_ID=0;
SEMAPHORE_ID_TYPE SEMAPHORE_ID=-1;

void testperiodic(void){
	RETURN_CODE_TYPE RETURN_CODE;	
	LOGGER(APP,"testperiodic: locking read");
	READLOCK_RW_SEMAPHORE (
		/*!in */ SEMAPHORE_ID,
		/*!in */ convertSecsToNano(1),
		/*!out*/ &RETURN_CODE );

	LOGGER(APP,"From testperiodic: Finished Work in test Periodic method. Return code is %d",RETURN_CODE);

}
void anotherperiodic(void){
	static int i=0;
	i++;
	RETURN_CODE_TYPE RETURN_CODE;	
	LOGGER(APP,"anotherperiodic: locking write");
	WRITELOCK_RW_SEMAPHORE (
		/*!in */ SEMAPHORE_ID,
		/*!in */ convertSecsToNano(1),
		/*!out*/ &RETURN_CODE );
	if (i ==2) sleep(3);
	sleep (1);
	LOGGER(APP,"anotherperiodic: UNlocking write");
	if(RETURN_CODE!=TIMED_OUT)
	{
		WRITEUNLOCK_RW_SEMAPHORE (
			/*!in */ SEMAPHORE_ID,

			/*!out*/ &RETURN_CODE );

	}
	LOGGER(APP,"anotherperiodic:  Finished Work in anotherperiodic:The return code is %d", RETURN_CODE);

}


int main(int argc, char* argv []){
	//CONFIGLOG(TRACE);
	SET_PARTITION_NAME("part1");
	RETURN_CODE_TYPE RETURN_CODE;
	char SEM_NAME [30] = "APERIODIC_PROCESS" ;

	CREATE_RW_SEMAPHORE (
		/*!in */ SEM_NAME,
		/*!in */ 4,
		/*!out*/ &SEMAPHORE_ID,
		/*!out*/ &RETURN_CODE );

	PERIODIC_PROCESS_ID_1 = APEX_HELPER_CREATE_PROCESS(convertSecsToNano(2), //SYSTEM_TIME_TYPE PERIOD,
		convertSecsToNano(2), //SYSTEM_TIME_TYPE TIME_CAPACITY,
		(SYSTEM_ADDRESS_TYPE)testperiodic, //SYSTEM_ADDRESS_TYPE ENTRY_POINT,
		0,//STACK_SIZE_TYPE STACK_SIZE,
		90,//PRIORITY_TYPE BASE_PRIORITY,
		HARD,//DEADLINE_TYPE DEADLINE,
		"PERIODIC_PROCESS_ID_1", //std::string PROCESS_NAME,
		&RETURN_CODE); //RETURN_CODE_TYPE *RETURN_CODE)
	assert(RETURN_CODE==NO_ERROR);

	PROCESS_ID_TYPE PERIODIC_PROCESS_ID_2 = APEX_HELPER_CREATE_PROCESS(convertSecsToNano(2), //SYSTEM_TIME_TYPE PERIOD,
		convertSecsToNano(2), //SYSTEM_TIME_TYPE TIME_CAPACITY,
		(SYSTEM_ADDRESS_TYPE)anotherperiodic, //SYSTEM_ADDRESS_TYPE ENTRY_POINT,
		0,//STACK_SIZE_TYPE STACK_SIZE,
		90,//PRIORITY_TYPE BASE_PRIORITY,
		SOFT,//DEADLINE_TYPE DEADLINE,
		"PERIODIC_PROCESS_ID_2", //std::string PROCESS_NAME,
		&RETURN_CODE); //RETURN_CODE_TYPE *RETURN_CODE)
	assert(RETURN_CODE==NO_ERROR);
	START(PERIODIC_PROCESS_ID_1,&RETURN_CODE);
	assert(RETURN_CODE==NO_ERROR);
	START(PERIODIC_PROCESS_ID_2,&RETURN_CODE);
	assert(RETURN_CODE==NO_ERROR);

	//Setting partition mode
	LOGGER(INFO,"Setting Partition State", INFO);
	SET_PARTITION_MODE(NORMAL,&RETURN_CODE);
	return 0;
}
