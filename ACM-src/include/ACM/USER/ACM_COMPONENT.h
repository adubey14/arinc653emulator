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

#ifndef _ACM_COMPONENT_H
#define _ACM_COMPONENT_H
 
#include <ACM/USER/APEX_TYPES.h>
#include <ACM/USER/ACM_HM_TYPES.h>
#include <ACM/USER/ACM_EXCEPTION.h>
#include <ACM/USER/ACM_UTILITIES.h>
#include <ACM/USER/ACM_LOGGER.h>

#include <ACM/USER/ACM_APEX_EXTENSIONS.h>
#include <ACM/USER/ACM_COMPONENT.h>
#include <ACM/USER/EVENT_PORT.h>
#include <stdio.h>

namespace ACM_INTERNAL{

}

namespace ACM_USER{
	//! All components used in ACM arinc emulation will inherit from ACMComponent. The functionality of this component is still to be decided.
	class ABSTRACT_PARENT_COMPONENT
	{
	public:
		NAME_TYPE BASE_COMPONENT_NAME;
	protected:
		
		SEMAPHORE_ID_TYPE RW_LOCK_ID;

		ABSTRACT_PARENT_COMPONENT(NAME_TYPE NAME,unsigned int NUM_READERS){
			strncpy(BASE_COMPONENT_NAME,NAME,MAX_NAME_LENGTH);			
			RETURN_CODE_TYPE RETURN_CODE;
			CREATE_RW_SEMAPHORE (
			/*!in */ BASE_COMPONENT_NAME,
			/*!in */ NUM_READERS,
			/*!out*/ &RW_LOCK_ID,
			/*!out*/ &RETURN_CODE );
			assert(RETURN_CODE==NO_ERROR);			
		}

		inline void readlock(SYSTEM_TIME_TYPE TIMEOUT,RETURN_CODE_TYPE* RETURN_CODE){
			READLOCK_RW_SEMAPHORE (
			/*!in */ RW_LOCK_ID,
			/*!in */ TIMEOUT,
			/*!out*/ RETURN_CODE );
		}
		inline void readunlock(RETURN_CODE_TYPE* RETURN_CODE){
			READUNLOCK_RW_SEMAPHORE (
			/*!in */ RW_LOCK_ID,
			/*!out*/ RETURN_CODE );
		}

		inline void writelock(SYSTEM_TIME_TYPE TIMEOUT,RETURN_CODE_TYPE* RETURN_CODE){
			WRITELOCK_RW_SEMAPHORE (
			/*!in */ RW_LOCK_ID,
			/*!in */ TIMEOUT,
			/*!out*/ RETURN_CODE );
		}

		inline void writeunlock(RETURN_CODE_TYPE* RETURN_CODE){
			WRITEUNLOCK_RW_SEMAPHORE (
			/*!in */ RW_LOCK_ID,
			/*!out*/ RETURN_CODE );
		}
	public:
		virtual ~ABSTRACT_PARENT_COMPONENT(){}
	};


	
}//end namespace ACM


#endif //_ACM_COMPONENT_H


//class ACM_COMPONENT_TYPE{
//
//	private:
//
//		pthread_rwlock_t	rwlock;	//!< The pthread read/write lock // A process belonging to a component must obtain either a read lock or write lock to the component before running
//		// Any interface defined with const in end will only require a read lock to be able to run.
//
//
//	public: 
//		ACM_COMPONENT_TYPE(){
//			int result = pthread_rwlock_init(&rwlock, NULL);
//			assert(!result);
//		}
//
//		virtual ~ACM_COMPONENT_TYPE(){
//			int result = pthread_rwlock_destroy(&rwlock);
//			switch (result)
//			{
//			case EBUSY:
//				LOGGER(INFO,"In ~ACM_COMPONENT_TYPE Destructor: Return Code from Destroy is EBUSY");
//				break;
//			case EINVAL:
//				LOGGER(INFO,"In ~ACM_COMPONENT_TYPE Destructor: Return Code from Destroy is EINVAL");
//				break;
//			default:
//				break;
//			}
//		}
//
//
//		/*! * The rdlock method locks the reading side of the lock. */
//		virtual inline void readlock(){
//			pthread_rwlock_rdlock(&rwlock);
//		}
//
//		/*! * The wrlock method locks the writing side of the lock.
//		*/
//		virtual inline void writelock(){
//			pthread_rwlock_wrlock(&rwlock);
//		}
//		/*!
//		* The unlock method releases the lock.
//		*/
//		virtual inline void unlock(){
//			pthread_rwlock_unlock(&rwlock);
//		}
//	};
//
//	typedef ACM_COMPONENT_TYPE* ACM_component_ptr;
