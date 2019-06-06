//# Copyright (c) Vanderbilt University, 2010
//# ALL RIGHTS RESERVED
//# Vanderbilt University disclaims all warranties with regard to this
//# software, including all implied warranties of merchantability
//# and fitness.  In no event shall Vanderbilt University be liable for
//# any special, indirect or consequential damages or any damages
//# whatsoever resulting from loss of use, data or profits, whether
//# in an action of contract, negligence or other tortious action,
//# arising out of or in connection with the use or performance of
//# this software.


#ifndef APEX_TIME_MANAGEMENT_SERVICES
#define APEX_TIME_MANAGEMENT_SERVICES
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
using namespace ACM_USER;
namespace ACM_INTERNAL {

//! \name Time management services
//@{
/* !
* 
*/
void APEX::TIMED_WAIT (
					   /*!in */ SYSTEM_TIME_TYPE DELAY_TIME,
					   /*!out*/ RETURN_CODE_TYPE *RETURN_CODE ){
						   //show set the state to ready

						   SYSTEM_TIMESPEC actualtime=currentTime();
						   SYSTEM_TIMESPEC absolute_time=addTimeNano(actualtime,DELAY_TIME);
						   int clockresult =1;
						   // you can spuriously get out of nanosleep
						   struct timespec rem = {0,0};
						   while ( clockresult != 0) {
							   clockresult = clock_nanosleep( CLOCK_REALTIME ,TIMER_ABSTIME, &absolute_time,&rem);
						   }
						   *RETURN_CODE=NO_ERROR;

						   //should set the state back to running
						   return;
}

void APEX::PERIODIC_WAIT (
						  /*!out*/ RETURN_CODE_TYPE *RETURN_CODE ){
							  __GET_SINGLETON_INSTANCE;
							  PROCESS_ID_TYPE PROCESS_ID=0;
							  *RETURN_CODE=NO_ERROR;
							  //PROCESS_ID_TO_PTHREAD_ID_MAP_TYPE::iterator iter=partition_instance->PROCESS_ID_TO_PTHREAD_ID_MAP.begin();
							  //PROCESS::ThreadID my_id=PROCESS::self();
							  //for(;iter!=partition_instance->PROCESS_ID_TO_PTHREAD_ID_MAP.end();iter++){
								 // if (*(iter->second)==my_id){
									//  PROCESS_ID = iter->first;

									//  //LOGGER(INFO,"Inside GET_MY_ID. My process_id is %ld",PROCESS_ID );

									//  break;
								 // }
							  //}
							  //if(iter!= partition_instance->PROCESS_ID_TO_PTHREAD_ID_MAP.end()) {
								 // LOGGER(ERROR,"Can not find process in this map");
								 // *RETURN_CODE=INVALID_MODE;
								 // return;
							  //}

							  APEX::GET_MY_ID(&PROCESS_ID,RETURN_CODE);

							  if(*RETURN_CODE != NO_ERROR)
							  {
								   LOGGER(ERROR,"Can not find process by get_my_id");
								   return;

							  }

							  PROCESS* process = partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->THE_PROCESS;
							  if (!process->isPeriodic()){
								  LOGGER(ERROR,"PROCESS %s is not periodic",partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->THE_PROCESS->attribute.NAME);
								  *RETURN_CODE=INVALID_MODE;
								  return;
							  }

							//!@abhishek : changing to make sure that the process waits on waiting on period.
							  //process->set_state(PROCESS::WAITING_ON_PERIOD);
							  process->waiting_on_period(RETURN_CODE);
							  return;
}

void APEX::GET_TIME (
					 /*!out*/ SYSTEM_TIME_TYPE *SYSTEM_TIME,
					 /*!out*/ RETURN_CODE_TYPE *RETURN_CODE ){
						 *SYSTEM_TIME = currentTimeInNanoSeconds();
						 *RETURN_CODE=NO_ERROR;
						 return;
}

void APEX::REPLENISH (/*!in */ SYSTEM_TIME_TYPE BUDGET_TIME,/*!out*/ RETURN_CODE_TYPE *RETURN_CODE )
{

__GET_SINGLETON_INSTANCE;

	if (partition_instance->CURRENT_OPERATING_MODE != NORMAL)
	{
		*RETURN_CODE=NO_ACTION;
		return;
	}

	if(BUDGET_TIME!=INFINITE_TIME_VALUE && BUDGET_TIME >  (SYSTEM_TIME_TYPE)MAX_TIME_OUT )
	{
		*RETURN_CODE=INVALID_PARAM;
		return;
	}
	PROCESS_ID_TYPE PROCESS_ID;		
	APEX::GET_MY_ID (&PROCESS_ID,RETURN_CODE);

	if (*RETURN_CODE!=NO_ERROR)
		return;

	PROCESS* process = partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->THE_PROCESS;
	if (process->get_state()== PROCESS::DORMANT)
	{
		*RETURN_CODE=NO_ACTION;
		return;
	}

	SYSTEM_TIMESPEC new_Deadline = computeDeadlineTimeSpecs(currentTime(),BUDGET_TIME);
	if (process->isPeriodic())
	{
		if (compareTime(partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->ABSOLUTE_NEXT_SCHEDULED_TIME,new_Deadline))
		{
			LOGGER(INFO,"Absolute next scheduled time is %lld and new deadline is %lld",
				convertTimeSpecToNano(partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->ABSOLUTE_NEXT_SCHEDULED_TIME),
				convertTimeSpecToNano(new_Deadline));


			*RETURN_CODE=INVALID_MODE;
			return;
		}	
		//LOGGER(INFO,"Absolute next scheduled time is okay with the new deadline");
	}
	partition_instance->PROCESS_TABLE_MUTEX.lock(); //Need to investigate
	partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->DEADLINE_TIME=new_Deadline;
	partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->ABSOLUTE_CRITICAL_TIME=new_Deadline;
	partition_instance->APEX_SCHEDULING_EVENT->SET_EVENT(RETURN_CODE); //release event. This will cause the unlock
	LOGGER(INFO,"Returning from Replenish");
	return;
}
}
#endif