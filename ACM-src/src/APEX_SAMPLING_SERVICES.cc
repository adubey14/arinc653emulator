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
#ifndef APEX_SAMPLING_SERVICES
#define APEX_SAMPLING_SERVICES
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
using namespace ACM_USER;
namespace ACM_INTERNAL {
//////////////////////////////////////////////////SAMPLING  PORTS//////////////////////////////////

	//! \name SAMPLING PORT Management Services
	//@{
	/* ! 
	* 
	*/
	void APEX::CREATE_SAMPLING_PORT (
		/*in */ SAMPLING_PORT_NAME_TYPE SAMPLING_PORT_NAME,
		/*in */ MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
		/*in */ PORT_DIRECTION_TYPE PORT_DIRECTION,
		/*in */ SYSTEM_TIME_TYPE REFRESH_PERIOD,
		/*out*/ SAMPLING_PORT_ID_TYPE *SAMPLING_PORT_ID,
		/*out*/ RETURN_CODE_TYPE *RETURN_CODE )
	{
		__GET_SINGLETON_INSTANCE_AND_CREATE_IF_DOES_NOT_EXIST;
		if (partition_instance->CURRENT_OPERATING_MODE == NORMAL)
		{
			LOGGER( ERROR, "From CREATE_SAMPLING_PORT: Trying to %s while partition is in NORMAL state",SAMPLING_PORT_NAME );
			*RETURN_CODE=INVALID_MODE;
			return;
		}
		if (partition_instance->SAMPLING_PORT_VECTOR.size() >= SYSTEM_LIMIT_NUMBER_OF_SAMPLING_PORTS)
		{
			LOGGER(  ERROR, "From CREATE_SAMPLING_PORT: exceed sampling port number limit");		
			*RETURN_CODE=INVALID_CONFIG;
			return;

		}
		//We do not check the max message size right now(MAX_MESSAGE_SIZE> (int) ACM_USER::SYSTEM_LIMIT_MAX_SAMPLING_MESSAGE_SIZE)

		if (MAX_MESSAGE_SIZE==0 || MAX_MESSAGE_SIZE>SYSTEM_LIMIT_MAX_SAMPLING_MESSAGE_SIZE)
		{
			//LOGGER(  ERROR, "From CREATE_SAMPLING_PORT: %s: MAX_MESSAGE_SIZE %d is either 0 or > SYSTEM_LIMIT_MAX_SAMPLING_MESSAGE_SIZE %d",SAMPLING_PORT_NAME,MAX_MESSAGE_SIZE,ACM_USER::SYSTEM_LIMIT_MAX_SAMPLING_MESSAGE_SIZE );
			*RETURN_CODE=INVALID_CONFIG;
			return;
		}		
		if(REFRESH_PERIOD==0||REFRESH_PERIOD > MAX_TIME_OUT)
		{
			LOGGER(  ERROR, "From CREATE_SAMPLING_PORT: %s: REFRESH_PERIOD==0||REFRESH_PERIOD > MAX_TIME_OUT",SAMPLING_PORT_NAME );
			*RETURN_CODE=INVALID_CONFIG;
			return;
		}
		//Ensure that the SAMPLING_PORT_NAME is unique
		for(unsigned int i = 0; i <partition_instance->SAMPLING_PORT_VECTOR.size();i++)
		{
			if (partition_instance->SAMPLING_PORT_VECTOR[i]->S_NAME== std::string(SAMPLING_PORT_NAME)){				
				LOGGER(  ERROR, "From CREATE_SAMPLING_PORT: NAME %s is not unique",SAMPLING_PORT_NAME);				
				*RETURN_CODE=NO_ACTION;
				return;
			}
		}
		//check port direction

		//create the sampling port
		*SAMPLING_PORT_ID=partition_instance->SAMPLING_PORT_VECTOR.size();
		ACM_USER::DIRECTION_TYPE new_port_direction;
		if (PORT_DIRECTION==ACM_USER::SOURCE) new_port_direction=ACM_USER::PORT_SOURCE;
		if (PORT_DIRECTION==ACM_USER::DESTINATION) new_port_direction=ACM_USER::PORT_DEST;
		SAMPLING_PORT_HELPER *sampling_port = new SAMPLING_PORT_HELPER(SAMPLING_PORT_NAME,MAX_MESSAGE_SIZE,new_port_direction,REFRESH_PERIOD,RETURN_CODE,O_CREAT);
		if (*RETURN_CODE==NO_ERROR) partition_instance->SAMPLING_PORT_VECTOR.push_back(sampling_port);
		return;
	}
	void APEX::WRITE_SAMPLING_MESSAGE (
		/*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
		/*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR, /* by reference */
		/*in */ MESSAGE_SIZE_TYPE LENGTH,
		/*out*/ RETURN_CODE_TYPE *RETURN_CODE )
	{
		__GET_SINGLETON_INSTANCE;

		if((unsigned int)SAMPLING_PORT_ID>=partition_instance->SAMPLING_PORT_VECTOR.size())
		{
			*RETURN_CODE=INVALID_PARAM;
			return;
		}

		SAMPLING_PORT_HELPER *sampling_port = partition_instance->SAMPLING_PORT_VECTOR[SAMPLING_PORT_ID];
		sampling_port->WRITE_SAMPLING_MESSAGE(MESSAGE_ADDR,LENGTH,RETURN_CODE);
		return;
	}


	void APEX::READ_SAMPLING_MESSAGE (
		/*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
		/*out*/ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
		/*out*/ MESSAGE_SIZE_TYPE *LENGTH,
		/*out*/ VALIDITY_TYPE *VALIDITY,
		/*out*/ RETURN_CODE_TYPE *RETURN_CODE )
	{
		__GET_SINGLETON_INSTANCE;

		if((unsigned int)SAMPLING_PORT_ID>=partition_instance->SAMPLING_PORT_VECTOR.size())
		{
			*RETURN_CODE=INVALID_PARAM;
			return;
		}
		SAMPLING_PORT_HELPER *sampling_port = partition_instance->SAMPLING_PORT_VECTOR[SAMPLING_PORT_ID];
		sampling_port->READ_SAMPLING_MESSAGE(MESSAGE_ADDR,LENGTH,VALIDITY,RETURN_CODE);
		return;
	}

	void APEX::GET_SAMPLING_PORT_ID (
		/*in */ SAMPLING_PORT_NAME_TYPE SAMPLING_PORT_NAME,
		/*out*/ SAMPLING_PORT_ID_TYPE *SAMPLING_PORT_ID,
		/*out*/ RETURN_CODE_TYPE *RETURN_CODE )
	{
		__GET_SINGLETON_INSTANCE;

		*RETURN_CODE=INVALID_CONFIG;
		//Ensure that the SAMPLING_PORT_NAME is unique
		for(unsigned int i = 0; i <partition_instance->SAMPLING_PORT_VECTOR.size();i++)
		{
			if (partition_instance->SAMPLING_PORT_VECTOR[i]->S_NAME== std::string(SAMPLING_PORT_NAME)){				
				*SAMPLING_PORT_ID=i;			
				*RETURN_CODE=NO_ERROR;
				return;
			}
		}
	}
	void APEX::GET_SAMPLING_PORT_STATUS (
		/*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
		/*out*/ SAMPLING_PORT_STATUS_TYPE *SAMPLING_PORT_STATUS,
		/*out*/ RETURN_CODE_TYPE *RETURN_CODE )
	{
		__GET_SINGLETON_INSTANCE;

		if((unsigned int)SAMPLING_PORT_ID>=partition_instance->SAMPLING_PORT_VECTOR.size())
		{
			*RETURN_CODE=INVALID_PARAM;
			return;
		}
		SAMPLING_PORT_HELPER *sampling_port = partition_instance->SAMPLING_PORT_VECTOR[SAMPLING_PORT_ID];
		sampling_port->GET_SAMPLING_PORT_STATUS(SAMPLING_PORT_STATUS,RETURN_CODE);
		return;
	}



	void APEX::WRITE_CONTENTS_FROM_DATA_BUFFER(SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,MESSAGE_ADDR_TYPE msg_ptr,MESSAGE_SIZE_TYPE  LENGTH,RETURN_CODE_TYPE* RETURN_CODE)
	{
		__GET_SINGLETON_INSTANCE;
		if((unsigned int)SAMPLING_PORT_ID>=partition_instance->SAMPLING_PORT_VECTOR.size())
		{
			*RETURN_CODE=INVALID_PARAM;
			return;
		}
		SAMPLING_PORT_HELPER *sampling_port = partition_instance->SAMPLING_PORT_VECTOR[SAMPLING_PORT_ID];
		sampling_port->WRITE_CONTENTS_FROM_DATA_BUFFER(msg_ptr,LENGTH,RETURN_CODE);
	}

	void APEX::READ_CONTENTS_TO_DATA_BUFFER(SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,MESSAGE_ADDR_TYPE msg_ptr,MESSAGE_SIZE_TYPE  MAX_LENGTH,MESSAGE_SIZE_TYPE*  READ_LENGTH,RETURN_CODE_TYPE* RETURN_CODE)
	{
		__GET_SINGLETON_INSTANCE;
		if((unsigned int)SAMPLING_PORT_ID>=partition_instance->SAMPLING_PORT_VECTOR.size())
		{
			*RETURN_CODE=INVALID_PARAM;
			return;
		}
		SAMPLING_PORT_HELPER *sampling_port = partition_instance->SAMPLING_PORT_VECTOR[SAMPLING_PORT_ID];
		sampling_port->READ_CONTENTS_TO_DATA_BUFFER(msg_ptr,MAX_LENGTH,READ_LENGTH,RETURN_CODE);
	}
	



}
#endif