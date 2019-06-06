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
#ifndef APEX_EVENT_SERVICES
#define APEX_EVENT_SERVICES
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
using namespace ACM_USER;
namespace ACM_INTERNAL {
/*==================================================================================================================================*/
	//! \name Event Management Services
	//@{
	/* !
	* 
	*/
	void APEX::CREATE_EVENT (	/*!in */ EVENT_NAME_TYPE EVENT_NAME,/*!out*/ EVENT_ID_TYPE *EVENT_ID,/*!out*/ RETURN_CODE_TYPE *RETURN_CODE )
	{
		//Try and obtain the instance.
		__GET_SINGLETON_INSTANCE_AND_CREATE_IF_DOES_NOT_EXIST;

		//Only one thread should be in create_process at a time
		//AUTO_LOCK lock (partition_instance->PROCESS_TABLE_MUTEX);		

		// Can Initialize only if the state is COLD_START or WARM_START
		if(partition_instance->CURRENT_OPERATING_MODE==IDLE || partition_instance->CURRENT_OPERATING_MODE==NORMAL){

			LOGGER(  ERROR, "From CREATE_EVENT: Trying to create event %s while parition is either in IDLE or NORMAL state",EVENT_NAME );

			*RETURN_CODE=INVALID_MODE;
			return;
		}

		//Ensure that the event_name is unique

		for(unsigned int i = 0; i <partition_instance->EVENT_VECTOR.size();i++){
			if (partition_instance->EVENT_VECTOR[i]->GET_NAME()== std::string(EVENT_NAME)){

				LOGGER(  ERROR, "From CREATE_EVENT: NAME %s is not unique",EVENT_NAME );

				*RETURN_CODE=INVALID_PARAM;
				return;
			}
		}

		APEX_EVENT_TYPE* new_event =APEX_EVENT_TYPE::CREATE_EVENT(EVENT_NAME);
		*EVENT_ID=partition_instance->EVENT_VECTOR.size();
		partition_instance->EVENT_VECTOR.push_back(new_event);
		*RETURN_CODE=NO_ERROR;

	}

	void APEX::SET_EVENT (EVENT_ID_TYPE EVENT_ID,RETURN_CODE_TYPE *RETURN_CODE )
	{

		//Try and obtain the instance.
		__GET_SINGLETON_INSTANCE;

		//check inputs
		if(partition_instance->EVENT_VECTOR.size()<=EVENT_ID) {
			*RETURN_CODE=INVALID_PARAM;
			return;
		}
		else{
			RETURN_CODE_TYPE temp_RETURN_CODE_TYPE;
			partition_instance->EVENT_VECTOR[EVENT_ID]->SET_EVENT(&temp_RETURN_CODE_TYPE);
			*RETURN_CODE =temp_RETURN_CODE_TYPE;
		}

	}
	void APEX::RESET_EVENT (
		/*!in */ EVENT_ID_TYPE EVENT_ID,
		/*!out*/ RETURN_CODE_TYPE *RETURN_CODE ){
			//Try and obtain the instance.
			__GET_SINGLETON_INSTANCE;

			//check inputs
			if(partition_instance->EVENT_VECTOR.size()<=EVENT_ID) {
				*RETURN_CODE=INVALID_PARAM;
				return;
			}
			else{
				RETURN_CODE_TYPE temp_RETURN_CODE_TYPE;
				partition_instance->EVENT_VECTOR[EVENT_ID]->RESET_EVENT(&temp_RETURN_CODE_TYPE);
				*RETURN_CODE =temp_RETURN_CODE_TYPE;
			}
	}
	void APEX::WAIT_EVENT (
		/*!in */ EVENT_ID_TYPE EVENT_ID,
		/*!in */ SYSTEM_TIME_TYPE TIME_OUT,
		/*!out*/ RETURN_CODE_TYPE *RETURN_CODE ){

			//Try and obtain the instance.
			__GET_SINGLETON_INSTANCE;
			//check inputs
			if(partition_instance->EVENT_VECTOR.size()<=EVENT_ID) {
				*RETURN_CODE=INVALID_PARAM;
				return;
			}
			else{
				RETURN_CODE_TYPE temp_RETURN_CODE_TYPE;
				if(TIME_OUT==INFINITE_TIME_VALUE){
					partition_instance->EVENT_VECTOR[EVENT_ID]->WAIT_EVENT(&temp_RETURN_CODE_TYPE);
				}
				else{
					partition_instance->EVENT_VECTOR[EVENT_ID]->WAIT_EVENT(TIME_OUT,&temp_RETURN_CODE_TYPE);
				}
				*RETURN_CODE =temp_RETURN_CODE_TYPE;
			}
	}
	void APEX::GET_EVENT_ID (
		/*!in */ EVENT_NAME_TYPE EVENT_NAME,
		/*!out*/ EVENT_ID_TYPE *EVENT_ID,
		/*!out*/ RETURN_CODE_TYPE *RETURN_CODE )
	{
		//Try and obtain the instance.
		__GET_SINGLETON_INSTANCE;

		*RETURN_CODE=INVALID_PARAM;

		std::string EVENT_NAME_STRING(EVENT_NAME);
		for(unsigned int i = 0; i <partition_instance->EVENT_VECTOR.size();i++){
			if (partition_instance->EVENT_VECTOR[i]->GET_NAME()== EVENT_NAME_STRING){
				*EVENT_ID=i;
				*RETURN_CODE=NO_ERROR;
				return;
			}
		}
		return;
	}
	void APEX::GET_EVENT_STATUS (
		/*!in */ EVENT_ID_TYPE EVENT_ID,
		/*!out*/ EVENT_STATUS_TYPE *EVENT_STATUS,
		/*!out*/ RETURN_CODE_TYPE *RETURN_CODE )
	{
		//Try and obtain the instance.
		__GET_SINGLETON_INSTANCE;

		//check inputs
		if(partition_instance->EVENT_VECTOR.size()<=EVENT_ID) {
			*RETURN_CODE=INVALID_PARAM;
			return;
		}
		else{
			EVENT_STATUS->EVENT_STATE=partition_instance->EVENT_VECTOR[EVENT_ID]->GET_STATE();
			EVENT_STATUS->WAITING_PROCESSES=partition_instance->EVENT_VECTOR[EVENT_ID]->GET_COUNT();
			*RETURN_CODE=NO_ERROR;
			return;
		}


	}
}
#endif