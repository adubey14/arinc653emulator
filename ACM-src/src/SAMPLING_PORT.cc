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

#ifndef _ACM_SAMPLING_PORT_HELPER_CC
#define _ACM_SAMPLING_PORT_HELPER_CC
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

namespace ACM_INTERNAL{
	SAMPLING_PORT_HELPER::~SAMPLING_PORT_HELPER()
	{
		if (this->SAMPLING_DESCRIPTOR!=-1){
			close(SAMPLING_DESCRIPTOR);
			shm_unlink(this->S_NAME.c_str());
		}

	}
	SAMPLING_PORT_HELPER::SAMPLING_PORT_HELPER(std::string Name,MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,DIRECTION_TYPE PORT_DIRECTION,SYSTEM_TIME_TYPE REFRESH_PERIOD, RETURN_CODE_TYPE* RETURN_CODE, int FLAGS):S_NAME(Name),S_MAX_MESSAGE_SIZE(MAX_MESSAGE_SIZE),S_PORT_DIRECTION(PORT_DIRECTION),S_REFRESH_PERIOD(REFRESH_PERIOD),SAMPLING_DESCRIPTOR(-1),READY_OR_NOT(false),LAST_VALIDITY(VALID)
	{

		*RETURN_CODE=NO_ERROR;
		S_EXTRA_MEMORY_REQUIRED=sizeof(SYSTEM_TIME_TYPE)+sizeof(MESSAGE_SIZE_TYPE);
		S_REAL_MAX_MESSAGE_SIZE=MAX_MESSAGE_SIZE+S_EXTRA_MEMORY_REQUIRED;

		//LOGGER(INFO,"S_REAL_MAX_MESSAGE_SIZE %d, S_EXTRA_MEMORY_REQUIRED %d",S_REAL_MAX_MESSAGE_SIZE,S_EXTRA_MEMORY_REQUIRED);
		if(MAX_MESSAGE_SIZE>SYSTEM_LIMIT_MAX_SAMPLING_MESSAGE_SIZE)
		{
			LOGGER(ERROR,"for sampling port %s; MAX_MESSAGE_SIZE>SYSTEM_LIMIT_MAX_SAMPLING_MESSAGE_SIZE",Name.c_str());
			*RETURN_CODE=INVALID_CONFIG;
			return;
		}
		switch (S_PORT_DIRECTION)
		{
		case ACM_USER::PORT_SOURCE:
			{
				this->SAMPLING_DESCRIPTOR = shm_open(this->S_NAME.c_str(), O_RDWR | FLAGS , 0777);
				break;
			}
		case ACM_USER::PORT_DEST:
			{
				this->SAMPLING_DESCRIPTOR = shm_open(this->S_NAME.c_str(), O_RDWR| FLAGS, 0777);
				break;
			}
		case ACM_USER::PORT_SOURCE_DEST:
			{
				this->SAMPLING_DESCRIPTOR = shm_open(this->S_NAME.c_str(), O_RDWR | FLAGS , 0777);
				break;
			}
		}
		if (this->SAMPLING_DESCRIPTOR==-1){
			if(errno ==EEXIST){
				*RETURN_CODE=NO_ACTION; //Queue exists
				LOGGER(ERROR,"PORT %s already exists ",this->S_NAME.c_str());
				return;
			}
			else{
				*RETURN_CODE=INVALID_CONFIG;
				LOGGER(ERROR,"PORT %s invalid Configuration ",this->S_NAME.c_str());
				return;
			}
		}
		/* Set the size of the SHM to be the size of the struct. */
		if (ftruncate(SAMPLING_DESCRIPTOR, S_REAL_MAX_MESSAGE_SIZE) ==-1)
		{
			LOGGER (ERROR,"ERROR While seting the shared memory size, %s",strerror(errno));
			*RETURN_CODE=NO_ACTION; 
			return;

		}
		if((shared_memory = (SAMPLING_DATA *) mmap(0, S_REAL_MAX_MESSAGE_SIZE, (PROT_READ | PROT_WRITE),MAP_SHARED, SAMPLING_DESCRIPTOR, 0)) == MAP_FAILED) 
		{
			LOGGER(ERROR,"FAILED TO map to memory %s",strerror(errno) );
			*RETURN_CODE=NO_ACTION; 
			return;
		}
		READY_OR_NOT=true;

		if(S_PORT_DIRECTION == ACM_USER::PORT_DEST)
		{
			shared_memory->ABSOLUTE_TIME_WRITTEN=0;
			shared_memory->MESSAGE_SIZE=0;
			//initialized it
		}

		LOGGER(INFO,"From constructor size of shared memory %d", sizeof(*shared_memory));


	}


	void SAMPLING_PORT_HELPER::WRITE_SAMPLING_MESSAGE(MESSAGE_ADDR_TYPE msg_ptr,MESSAGE_SIZE_TYPE  LENGTH,RETURN_CODE_TYPE* RETURN_CODE)
	{
		if (!READY_OR_NOT){
			*RETURN_CODE=INVALID_MODE;
			return;
		}
		//LOGGER(INFO,"TRYING TO WRITE TO SHARED MEMORY");
		*RETURN_CODE=NO_ERROR;
		if(this->S_PORT_DIRECTION==ACM_USER::PORT_DEST)
		{
			LOGGER(ACM_USER::ERROR,"Sampling Port %s is to be used only for receiving message. Cannot write to it",this->S_NAME.c_str());
			*RETURN_CODE=INVALID_MODE;
			return;
		}
		if(LENGTH>this->S_MAX_MESSAGE_SIZE)
		{
			*RETURN_CODE=INVALID_CONFIG;
			LOGGER(INFO,"LENGTH>this->S_MAX_MESSAGE_SIZE");
			return;
		}		

		AUTO_LOCK l(myMutex);

		shared_memory->ABSOLUTE_TIME_WRITTEN=currentTimeInNanoSeconds();
		shared_memory->MESSAGE_SIZE=LENGTH;		
		//LOGGER(INFO,"size of shared memory %d", sizeof(*shared_memory));		
		memcpy( &(shared_memory->data[0]),msg_ptr,LENGTH);		
		//LOGGER(INFO,"time %lld",shared_memory->ABSOLUTE_TIME_WRITTEN);
	//	LOGGER(INFO,"time %d",shared_memory->MESSAGE_SIZE);		
	}

	void SAMPLING_PORT_HELPER::WRITE_CONTENTS_FROM_DATA_BUFFER(MESSAGE_ADDR_TYPE msg_ptr,MESSAGE_SIZE_TYPE  READ_LENGTH,RETURN_CODE_TYPE* RETURN_CODE)
	{
		
		if (!READY_OR_NOT){
			*RETURN_CODE=INVALID_MODE;
			return;
		}
		if(this->S_PORT_DIRECTION==ACM_USER::PORT_SOURCE)
		{
			LOGGER(ACM_USER::ERROR,"Sampling Port %s is to be used only for receiving message. Cannot write to it from a socket",this->S_NAME.c_str());
			*RETURN_CODE=INVALID_MODE;
			return;
		}
		*RETURN_CODE=NO_ERROR;
		AUTO_LOCK l(myMutex);
		
		if(READ_LENGTH<sizeof(SYSTEM_TIME_TYPE)+sizeof(MESSAGE_SIZE_TYPE))
		{
			LOGGER(ACM_USER::ERROR,"Sampling Port %s Read data size %lld is small.",this->S_NAME.c_str(),READ_LENGTH);
			*RETURN_CODE=NO_ACTION;
			return;
		}

		unsigned int written=0;
		memcpy(&(this->shared_memory->ABSOLUTE_TIME_WRITTEN),msg_ptr,sizeof(SYSTEM_TIME_TYPE));
		//std::cout<<this->shared_memory->ABSOLUTE_TIME_WRITTEN<<endl;
		written+=sizeof(SYSTEM_TIME_TYPE);
		memcpy(&(this->shared_memory->MESSAGE_SIZE),msg_ptr+written,sizeof(MESSAGE_SIZE_TYPE));			
		assert(this->shared_memory->MESSAGE_SIZE>=0);
		written+=sizeof(MESSAGE_SIZE_TYPE);
		memcpy(&(this->shared_memory->data[0]),msg_ptr+written,this->shared_memory->MESSAGE_SIZE);
	}


	void SAMPLING_PORT_HELPER::READ_CONTENTS_TO_DATA_BUFFER(MESSAGE_ADDR_TYPE msg_ptr,MESSAGE_SIZE_TYPE  MAX_LENGTH,MESSAGE_SIZE_TYPE*  READ_LENGTH,RETURN_CODE_TYPE* RETURN_CODE)
	{
		*READ_LENGTH=0;
		if (!READY_OR_NOT){
			*RETURN_CODE=INVALID_MODE;
			return;
		}
		if(this->S_PORT_DIRECTION==ACM_USER::PORT_DEST)
		{
			LOGGER(ACM_USER::ERROR,"Sampling Port %s is to be used only for receiving message. Cannot transfer its data to a socket",this->S_NAME.c_str());
			*RETURN_CODE=INVALID_MODE;
			return;
		}
		//LOGGER(INFO,"TRYING TO READ TO SHARED MEMORY");
		*RETURN_CODE=NO_ERROR;

		if(MAX_LENGTH <this->S_REAL_MAX_MESSAGE_SIZE)
		{
			LOGGER(ERROR,"Insufficient buffer length");
			*RETURN_CODE=INVALID_PARAM;
			return;
		}
		AUTO_LOCK l(myMutex);
		unsigned int written=0;
		memcpy(msg_ptr,&(this->shared_memory->ABSOLUTE_TIME_WRITTEN),sizeof(SYSTEM_TIME_TYPE));
		//std::cout<<this->shared_memory->ABSOLUTE_TIME_WRITTEN<<std::endl;
		written+=sizeof(SYSTEM_TIME_TYPE);
		memcpy(msg_ptr+written,&(this->shared_memory->MESSAGE_SIZE),sizeof(MESSAGE_SIZE_TYPE));
		written+=sizeof(MESSAGE_SIZE_TYPE);
		memcpy(msg_ptr+written,&(this->shared_memory->data[0]),this->shared_memory->MESSAGE_SIZE);
		written+=this->shared_memory->MESSAGE_SIZE;
		*READ_LENGTH=written;
		return;
	}


	void SAMPLING_PORT_HELPER::READ_SAMPLING_MESSAGE(MESSAGE_ADDR_TYPE msg_ptr, MESSAGE_SIZE_TYPE*  LENGTH, VALIDITY_TYPE* VALIDITY, RETURN_CODE_TYPE* RETURN_CODE)
	{
		if (!READY_OR_NOT){
			*RETURN_CODE=INVALID_MODE;
			return;
		}
		//LOGGER(INFO,"TRYING TO READ TO SHARED MEMORY");
		*RETURN_CODE=NO_ERROR;
		if(this->S_PORT_DIRECTION==ACM_USER::PORT_SOURCE)
		{
			LOGGER(ACM_USER::ERROR,"Sampling Port %s is to be used only for receiving message. Cannot read from it",this->S_NAME.c_str());
			*RETURN_CODE=INVALID_MODE;
			return;
		}
		//LOGGER(INFO,"time %lld",shared_memory->ABSOLUTE_TIME_WRITTEN);
		//LOGGER(INFO,"time %d",shared_memory->MESSAGE_SIZE);
		//LOGGER(INFO,"COPIED FROM SHARED MEMORY");
		//check if port is empty
	
		
		AUTO_LOCK l(myMutex);


		*LENGTH=shared_memory->MESSAGE_SIZE;
		
		if(shared_memory->MESSAGE_SIZE==0)
		{
			LOGGER(ERROR,"MESSAGE SIZE is %d. Port is empty",*LENGTH);
			*RETURN_CODE=NO_ACTION;
			*VALIDITY=INVALID;
			return;			
		}
		//LOGGER(INFO,"MESSAGE SIZE is %d",*LENGTH);
		memcpy(msg_ptr,&(shared_memory->data[0]),*LENGTH);
		//check validity
		SYSTEM_TIME_TYPE AGE = currentTimeInNanoSeconds()- shared_memory->ABSOLUTE_TIME_WRITTEN;
		if (AGE > this->S_REFRESH_PERIOD){
			*VALIDITY=INVALID;
		}
		else{
			*VALIDITY=VALID;
		}
		LAST_VALIDITY=*VALIDITY;

	}

	void SAMPLING_PORT_HELPER::GET_SAMPLING_PORT_STATUS(SAMPLING_PORT_STATUS_TYPE* SAMPLING_PORT_STATUS,RETURN_CODE_TYPE* RETURN_CODE)
	{
		if (!READY_OR_NOT){
			*RETURN_CODE=INVALID_MODE;
			return;
		}
		*RETURN_CODE=NO_ERROR;

		SAMPLING_PORT_STATUS->REFRESH_PERIOD=this->S_REFRESH_PERIOD;
		SAMPLING_PORT_STATUS->MAX_MESSAGE_SIZE=this->S_MAX_MESSAGE_SIZE;
		SAMPLING_PORT_STATUS->LAST_MSG_VALIDITY=this->LAST_VALIDITY;			
		if(this->S_PORT_DIRECTION==ACM_USER::PORT_SOURCE) 
		{
			SAMPLING_PORT_STATUS->PORT_DIRECTION=SOURCE;			
			return;
		}
		if(this->S_PORT_DIRECTION==ACM_USER::PORT_DEST || this->S_PORT_DIRECTION==ACM_USER::PORT_SOURCE_DEST) 
		{
			SAMPLING_PORT_STATUS->PORT_DIRECTION=DESTINATION;
			return;
		}
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






}



#endif //_ACM_SAMPLING_PORT_HELPER_CC
