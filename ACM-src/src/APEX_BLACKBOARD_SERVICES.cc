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
#ifndef APEX_BLACKBOARD_SERVICES
#define APEX_BLACKBOARD_SERVICES
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
using namespace ACM_USER;
namespace ACM_INTERNAL {
/*======================================	BLACKBOARD BUSINESS				==========================================================*/
	void APEX::CREATE_BLACKBOARD (
		/*in */ BLACKBOARD_NAME_TYPE BLACKBOARD_NAME,
		/*in */ MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
		/*out*/ BLACKBOARD_ID_TYPE *BLACKBOARD_ID,
		/*out*/ RETURN_CODE_TYPE *RETURN_CODE )
	{
		__GET_SINGLETON_INSTANCE_AND_CREATE_IF_DOES_NOT_EXIST;
		assert(partition_instance!=NULL);
		//LOGGER(INFO,"From CREATE_BLACKBOARD: Trying to create blackboard %s  ",BLACKBOARD_NAME );



		// Can Initialize only if the state is COLD_START or WARM_START
		if(partition_instance->CURRENT_OPERATING_MODE==NORMAL){

			LOGGER(  ERROR, "From CREATE_BLACKBOARD: Trying to create blackboard %s while partition is in NORMAL state",BLACKBOARD_NAME );

			*RETURN_CODE=INVALID_MODE;
			return;
		}
		//Only one thread should be in create_blackboard/any create at a time
		//AUTO_LOCK lock (partition_instance->PROCESS_TABLE_MUTEX);	
		
		if (MAX_MESSAGE_SIZE<=0){
			*RETURN_CODE=INVALID_PARAM;
			return;

		}



		//Ensure that the BLACKBOARD_NAME is unique

		for(unsigned int i = 0; i <partition_instance->BLACKBOARD_VECTOR.size();i++){
			if (partition_instance->BLACKBOARD_VECTOR[i]->BLACKBOARD_NAME== std::string(BLACKBOARD_NAME)){				
				LOGGER(  ERROR, "From CREATE_BLACKBOARD: NAME %s is not unique",BLACKBOARD_NAME);				
				*RETURN_CODE=NO_ACTION;
				return;
			}
		}




		*BLACKBOARD_ID=partition_instance->BLACKBOARD_VECTOR.size();
		//LOGGER(INFO,"From CREATE_BLACKBOARD: Trying to call new on blackboard");
		APEX_BLACKBOARD_TYPE* APEX_BLACKBOARD = new APEX_BLACKBOARD_TYPE(BLACKBOARD_NAME,*BLACKBOARD_ID,MAX_MESSAGE_SIZE);
		partition_instance->BLACKBOARD_VECTOR.push_back(APEX_BLACKBOARD);
		*RETURN_CODE=NO_ERROR;
		LOGGER(INFO,"From CREATE_BLACKBOARD: Successful");
	} //end_create_blackboard;



	void APEX::DISPLAY_BLACKBOARD (
		/*in */ BLACKBOARD_ID_TYPE BLACKBOARD_ID,
		/*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR, /* by reference */
		/*in */ MESSAGE_SIZE_TYPE LENGTH,
		/*out*/ RETURN_CODE_TYPE *RETURN_CODE )
	{
		__GET_SINGLETON_INSTANCE;

		if (partition_instance->BLACKBOARD_VECTOR.size()<=BLACKBOARD_ID)
		{
			*RETURN_CODE=INVALID_PARAM;
			return;

		}
		APEX_BLACKBOARD_TYPE* BBOARD=partition_instance->BLACKBOARD_VECTOR[BLACKBOARD_ID];

		if (BBOARD->MAX_MESSAGE_SIZE<LENGTH)
		{
			*RETURN_CODE=INVALID_PARAM;
			return;

		}
		APEX::CLEAR_BLACKBOARD (BLACKBOARD_ID,RETURN_CODE);

		if(*RETURN_CODE!=NO_ERROR)
		{
			return;
		}



		BBOARD->WRITE_ON_BLACKBOARD(MESSAGE_ADDR,LENGTH,RETURN_CODE);
		return;
		//copy



	}
	void APEX::READ_BLACKBOARD (
		/*in */ BLACKBOARD_ID_TYPE BLACKBOARD_ID,
		/*in */ SYSTEM_TIME_TYPE TIME_OUT,
		/*out*/ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
		/*out*/ MESSAGE_SIZE_TYPE *LENGTH,
		/*out*/ RETURN_CODE_TYPE *RETURN_CODE )
	{
				__GET_SINGLETON_INSTANCE;

			if (partition_instance->BLACKBOARD_VECTOR.size()<=BLACKBOARD_ID)
			{
				*RETURN_CODE=INVALID_PARAM;
				return;

			}

			if(TIME_OUT > (SYSTEM_TIME_TYPE)MAX_TIME_OUT){
				*RETURN_CODE=INVALID_PARAM;
				return;
			}
			//PROCESS STATE SHOULD BE SET TO WAITING
			APEX_BLACKBOARD_TYPE* BBOARD=partition_instance->BLACKBOARD_VECTOR[BLACKBOARD_ID];
//			LOGGER(INFO,"FOUND Black Board");
			if(BBOARD->CURRENT_MESSAGE_SIZE==0){
				//WE will wait

				//LOGGER(INFO,"MESSAGE_SIZE=0");

				if(TIME_OUT==0){
					*RETURN_CODE=NOT_AVAILABLE;
					return;
				}
				APEX::WAIT_EVENT(BBOARD->BLACKBOARD_EVENT_ID,TIME_OUT,RETURN_CODE);
				if(*RETURN_CODE==TIMED_OUT)
					return;
			}
			//PROCESS STATE SHOULD BE SET TO READY AND RUNNING

			BBOARD->READ_FROM_BLACKBOARD(MESSAGE_ADDR,LENGTH,RETURN_CODE);
			//LOGGER(INFO,"FINISHED READING FROM BBOARD");
			//*LENGTH=BBOARD->CURRENT_MESSAGE_SIZE;

			//memcpy(MESSAGE_ADDR,BBOARD->PAYLOAD,*LENGTH);
			*RETURN_CODE=NO_ERROR;
	}
	void APEX::CLEAR_BLACKBOARD (
		/*in */ BLACKBOARD_ID_TYPE BLACKBOARD_ID,
		/*out*/ RETURN_CODE_TYPE *RETURN_CODE ){


			__GET_SINGLETON_INSTANCE;

			if (partition_instance->BLACKBOARD_VECTOR.size()<=BLACKBOARD_ID)
			{
				*RETURN_CODE=INVALID_PARAM;
				return;

			}
			partition_instance->BLACKBOARD_VECTOR[BLACKBOARD_ID]->ERASE_BLACKBOARD(RETURN_CODE);

			return;
	}
	void APEX::GET_BLACKBOARD_ID (
		/*in */ BLACKBOARD_NAME_TYPE BLACKBOARD_NAME,
		/*out*/ BLACKBOARD_ID_TYPE *BLACKBOARD_ID,
		/*out*/ RETURN_CODE_TYPE *RETURN_CODE )
	{
		__GET_SINGLETON_INSTANCE;
		*RETURN_CODE=NO_ERROR;
		for(unsigned int i = 0; i <partition_instance->BLACKBOARD_VECTOR.size();i++){
			if (partition_instance->BLACKBOARD_VECTOR[i]->BLACKBOARD_NAME== std::string(BLACKBOARD_NAME)){				
				*BLACKBOARD_ID=i;				
				return;
			}
		}

		*RETURN_CODE=INVALID_CONFIG;
	}


	void APEX::GET_BLACKBOARD_STATUS (
		/*in */ BLACKBOARD_ID_TYPE BLACKBOARD_ID,
		/*out*/ BLACKBOARD_STATUS_TYPE *BLACKBOARD_STATUS,
		/*out*/ RETURN_CODE_TYPE *RETURN_CODE )
	{

		__GET_SINGLETON_INSTANCE;

		if (partition_instance->BLACKBOARD_VECTOR.size()<=BLACKBOARD_ID)
		{
			*RETURN_CODE=INVALID_PARAM;
			return;

		}
		APEX_BLACKBOARD_TYPE* BBOARD=partition_instance->BLACKBOARD_VECTOR[BLACKBOARD_ID];
		BLACKBOARD_STATUS->EMPTY_INDICATOR=(BBOARD->CURRENT_MESSAGE_SIZE==0)?EMPTY:OCCUPIED;
		BLACKBOARD_STATUS->MAX_MESSAGE_SIZE=BBOARD->MAX_MESSAGE_SIZE;
		EVENT_STATUS_TYPE EVENT_STATUS;
		APEX::GET_EVENT_STATUS(BBOARD->BLACKBOARD_EVENT_ID,&EVENT_STATUS,RETURN_CODE);
		BLACKBOARD_STATUS->WAITING_PROCESSES=EVENT_STATUS.WAITING_PROCESSES;
		return;				
	}
}
#endif 