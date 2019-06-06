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

#ifndef APEX_TYPES_H
#define APEX_TYPES_H
#include <sched.h>
#include <string.h>
//! ACM_USER namespace is installed and given to USERS. 
namespace ACM_USER{
enum DIRECTION_TYPE{PORT_SOURCE,PORT_DEST,PORT_SOURCE_DEST};
#define PARTITION_MANAGER_PRIORITY sched_get_priority_max(SCHED_FIFO)//!< General definition for partition MANAGER. Usually 99
#define MODULE_MANAGER_PRIORITY PARTITION_MANAGER_PRIORITY //!< Module Manager is the new name for partition manager
#define CPU_CORE_TO_USE 0 //!<By default a partition uses core 0. This can be specifically changed using module manager configuration file.
#define CONTROLLER_PROCESS_PRIORITY 97 //!<This is the highest priority in a partition (Scheduler)
#define PARTITION_PROCESS_HIGHPRIORITY 96 //!<All high priority processes run at this level.
#define HMPRIORITY PARTITION_PROCESS_HIGHPRIORITY  //!<All high priority processes run at this level.
#define PARTITION_ORB_PROCESS_HIGH_PRIORITY PARTITION_PROCESS_HIGHPRIORITY  //!<All high priority processes run at this level.

#define PARTITION_PROCESS_LOWPRIORITY 83	  //!<Lowest priority in a partition
#define DUMMY_PROCESS_PRIORITY (PARTITION_PROCESS_LOWPRIORITY+1)//!< priority for the dummy process. Anything less than the dummy process priority will be automaticall suspended.	
#define MINIMUM_TIME_CAPACITY 1 //!< MINIMUM_TIME_CAPACITY is in NANO SECONDS
#define SUSPEND_PROCESS_PRIORITY PARTITION_PROCESS_LOWPRIORITY

		/* PARTITION */
		//controller thread priority - highest in the partition
//#define  CONTROLLER_THREAD_PRIORITY PARTITION_PROCESS_HIGHPRIORITY
		//sleeping priority for the partition's threads
//#define PARTITION_THREAD_LOWPRIORITY 82
		//workerthread base priority
		//priority levels are from 0-9 which means effective priority will be from 85-94
//#define WORKER_THREAD_BASE_PRIORITY 85
		
		
#define WORKER_THREAD_RESTART_PRIORITY 95//!<restart priority level for worker thread.when a worker thread has to be restarted, it's priority will be raised to this level to boost up the restart process




	//! \name Domain Dependent Limits
	//@{
	/* !
	* These values define the domain limits and are implementation dependent
	*/
#define SYSTEM_LIMIT_NUMBER_OF_PARTITIONS		32 //!< -- module scope
#define SYSTEM_LIMIT_NUMBER_OF_MESSAGES			512 //!< -- module scope
#define SYSTEM_LIMIT_MESSAGE_SIZE				8192 //!< -- module scope
#define SYSTEM_LIMIT_NUMBER_OF_PROCESSES		128 //!<-- partition scope
#define SYSTEM_LIMIT_NUMBER_OF_SAMPLING_PORTS	512 //!< -- partition scope
#define SYSTEM_LIMIT_NUMBER_OF_QUEUING_PORTS	512 //!< -- partition scope
#define SYSTEM_LIMIT_NUMBER_OF_BUFFERS			256 //!< -- partition scope
#define SYSTEM_LIMIT_NUMBER_OF_BLACKBOARDS		256 //!< -- partition scope
#define SYSTEM_LIMIT_NUMBER_OF_SEMAPHORES		256 //!< -- partition scope
#define SYSTEM_LIMIT_NUMBER_OF_EVENTS			256 //!< -- partition scope


	//! \name Base APEX types
	//@{
	/* !
	* 
	*/
	typedef unsigned char APEX_BYTE;		//!< 8-bit unsigned
	typedef long APEX_INTEGER;				//!< 32-bit signed
	typedef unsigned long APEX_UNSIGNED;	//!< 32-bit unsigned
	typedef long long APEX_LONG_INTEGER;	//!< 64-bit signed

	//! \name  Prefix Definition
	//@{
	/* !
	* 
	*/
#define NANO 1000000000					//!<9 zeros
#define MILLI 1000						//!<3 zeros
#define MICRO 1000000					//!<6 zeros


	//! \name General APEX types 
	//@{
	/* !
	* 
	*/
	typedef APEX_LONG_INTEGER SYSTEM_TIME_TYPE; //!<SYSTEM_TIME_TYPE is defined using a signed 64-bit value with a resolution of 1 nanosecond.


	enum RETURN_CODE_TYPE
	{
		NO_ERROR,		//!< request valid and operation performed
		NO_ACTION,		//!< system's operational status unaffected by request
		NOT_AVAILABLE,	//!< the request cannot be performed immediately
		INVALID_PARAM,	//!< parameter specified in request invalid
		INVALID_CONFIG,	//!< parameter specified in request incompatible with current configuration (e.g., as specified by system integrator)
		INVALID_MODE,	//!< request incompatible with current mode of operation
		TIMED_OUT		//!< time-out associated with request has expired
	};

inline const char* RETURN_CODE_TO_STRING(RETURN_CODE_TYPE CODE)
{
	switch (CODE)
	{
		case NO_ERROR:
			return "NO_ERROR";
			break;
		case NO_ACTION:
			return "NO_ACTION";
			break;
		case NOT_AVAILABLE:
			return "NOT_AVAILABLE";
			break;
		case INVALID_PARAM:
			return "INVALID_PARAM";
			break;
		case INVALID_CONFIG:
			return "INVALID_CONFIG";
			break;
		case INVALID_MODE:
			return "INVALID_MODE";
			break;
		case TIMED_OUT:
			return "TIMED_OUT";
			break;
		default:
			return "Unknown value";	
		
	};
}	
	
	
	//MAX_NAME_LENGTH 30 changed to 60
#define MAX_NAME_LENGTH 100
	typedef char NAME_TYPE[MAX_NAME_LENGTH];
	typedef void (* SYSTEM_ADDRESS_TYPE);
	typedef APEX_BYTE * MESSAGE_ADDR_TYPE;
	typedef APEX_INTEGER MESSAGE_SIZE_TYPE;
	typedef APEX_INTEGER MESSAGE_RANGE_TYPE;
	typedef enum { SOURCE = 0, DESTINATION = 1 } PORT_DIRECTION_TYPE;
	typedef enum { FIFO = 0, PRIORITY = 1 } QUEUING_DISCIPLINE_TYPE;
//	typedef APEX_LONG_INTEGER SYSTEM_TIME_TYPE; //!< 64-bit signed integer with a 1 nanosecond LSB


#define MAX_TIME_OUT (SYSTEM_TIME_TYPE)315569260*NANO  //!<100 years;
//#define MAX_TIME_OUT (long long)3155692600*(long long)NANO  //!<100 years;
#define INFINITE_TIME_VALUE (SYSTEM_TIME_TYPE)-1


	//! \name PROCESS constant and type definitions and management services
	//@{
	/* !
	* 
	*/
#define MAX_NUMBER_OF_PROCESSES SYSTEM_LIMIT_NUMBER_OF_PROCESSES
#define MIN_PRIORITY_VALUE 1
#define MAX_PRIORITY_VALUE 63
#define MAX_LOCK_LEVEL 16
	typedef NAME_TYPE PROCESS_NAME_TYPE;
	//typedef APEX_INTEGER PROCESS_ID_TYPE;
	typedef APEX_UNSIGNED PROCESS_ID_TYPE;
	typedef APEX_INTEGER LOCK_LEVEL_TYPE;
	typedef APEX_UNSIGNED STACK_SIZE_TYPE;
	typedef APEX_INTEGER WAITING_RANGE_TYPE;
	typedef APEX_INTEGER PRIORITY_TYPE;

	enum PROCESS_STATE_TYPE {
		DORMANT ,
		READY ,
		RUNNING ,
		WAITING 
	} ;

	enum DEADLINE_TYPE { SOFT, HARD } ;

	//!Apex Structure for storing process attributes
	struct PROCESS_ATTRIBUTE_TYPE {
		SYSTEM_TIME_TYPE PERIOD; //!<Time Period. This Value is defined in Nanoseconds
		SYSTEM_TIME_TYPE TIME_CAPACITY; //!< Worst Case Execution Time. Defined in Nanoseconds
		SYSTEM_ADDRESS_TYPE ENTRY_POINT; //!<System address point is the entry function i.e. code to be executed
		STACK_SIZE_TYPE STACK_SIZE; //!< We do not use it currently
		PRIORITY_TYPE BASE_PRIORITY; //!< Should be between PARTITION_PROCESS_HIGHPRIORITY and PARTITION_PROCESS_LOWPRIORITY
		DEADLINE_TYPE DEADLINE; //! Deadline type, can be SOFT or HARD
		PROCESS_NAME_TYPE NAME; //! Name of the process. This name should be unique in a partition.

		PROCESS_ATTRIBUTE_TYPE(){
		//constructed
		};
		PROCESS_ATTRIBUTE_TYPE(const PROCESS_ATTRIBUTE_TYPE& copy):
				PERIOD(copy.PERIOD),
				TIME_CAPACITY(copy.TIME_CAPACITY),
				ENTRY_POINT(copy.ENTRY_POINT),
				STACK_SIZE(copy.STACK_SIZE),
				BASE_PRIORITY(copy.BASE_PRIORITY),
				DEADLINE (copy.DEADLINE)
					
		{
			//constructed
			strcpy(NAME,copy.NAME);
		}
	} ;

	typedef PROCESS_ATTRIBUTE_TYPE* 	PROCESS_ATTRIBUTE_TYPE_PTR;
//!Apex Structure for inquiring the status of a process.
	struct PROCESS_STATUS_TYPE {
		SYSTEM_TIME_TYPE DEADLINE_TIME;
		PRIORITY_TYPE CURRENT_PRIORITY;
		PROCESS_STATE_TYPE PROCESS_STATE;
		PROCESS_ATTRIBUTE_TYPE ATTRIBUTES;
	} ;



	//! \name PARTITION constant and type definitions and management services
	//@{
	/* !
	* 
	*/
	enum OPERATING_MODE_TYPE {
		IDLE ,
		COLD_START ,
		WARM_START ,
		NORMAL 
	} ;

	typedef APEX_INTEGER PARTITION_ID_TYPE;

	enum START_CONDITION_TYPE {
		NORMAL_START,
		PARTITION_RESTART,
		HM_MODULE_RESTART,
		HM_PARTITION_RESTART
	};
//!Apex Structure
	struct PARTITION_STATUS_TYPE {
		SYSTEM_TIME_TYPE PERIOD;
		SYSTEM_TIME_TYPE DURATION;
		PARTITION_ID_TYPE IDENTIFIER;
		LOCK_LEVEL_TYPE LOCK_LEVEL;
		OPERATING_MODE_TYPE OPERATING_MODE;
		START_CONDITION_TYPE START_CONDITION;
	};

	//! \name SEMAPHORE constant and type definitions and management services
	//@{
	/* !
	* 
	*/
#define MAX_NUMBER_OF_SEMAPHORES SYSTEM_LIMIT_NUMBER_OF_SEMAPHORES
#define MAX_SEMAPHORE_VALUE 32767
	typedef NAME_TYPE SEMAPHORE_NAME_TYPE;
	//typedef APEX_INTEGER SEMAPHORE_ID_TYPE;
	typedef APEX_UNSIGNED SEMAPHORE_ID_TYPE;
	typedef APEX_INTEGER SEMAPHORE_VALUE_TYPE;

	//!Apex Structure
	struct SEMAPHORE_STATUS_TYPE {
		SEMAPHORE_VALUE_TYPE CURRENT_VALUE;
		SEMAPHORE_VALUE_TYPE MAXIMUM_VALUE;
		WAITING_RANGE_TYPE WAITING_PROCESSES;
	} ;


	//! \name EVENT constant and type definitions and management services
	//@{
	/* ! Events are similar to condition Variables
	* 
	*/
#define MAX_NUMBER_OF_EVENTS SYSTEM_LIMIT_NUMBER_OF_EVENTS
	typedef NAME_TYPE EVENT_NAME_TYPE;
//	typedef APEX_INTEGER EVENT_ID_TYPE;
		typedef APEX_UNSIGNED EVENT_ID_TYPE;
	enum EVENT_STATE_TYPE { DOWN  , UP  } ;

	//!Apex Structure
	struct EVENT_STATUS_TYPE {
		EVENT_STATE_TYPE EVENT_STATE;
		WAITING_RANGE_TYPE WAITING_PROCESSES;
	} ;


	//! \name SAMPLING PORT constant and type definitions and management services
	//@{
	/* ! 
	* 
	*/

#define SYSTEM_LIMIT_MAX_SAMPLING_MESSAGE_SIZE 262144
#define MAX_NUMBER_OF_SAMPLING_PORTS SYSTEM_LIMIT_NUMBER_OF_SAMPLING_PORTS
	typedef NAME_TYPE SAMPLING_PORT_NAME_TYPE;
	typedef APEX_UNSIGNED SAMPLING_PORT_ID_TYPE;
	enum VALIDITY_TYPE { INVALID  , VALID } ;

	
	struct SAMPLING_PORT_STATUS_TYPE {
		SYSTEM_TIME_TYPE REFRESH_PERIOD;
		MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE;
		PORT_DIRECTION_TYPE PORT_DIRECTION;
		VALIDITY_TYPE LAST_MSG_VALIDITY;
	} ;

	//! \name QUEUING PORT constant and type definitions and management services
	//@{
	/* ! 
	* 
	*/

#define MAX_NUMBER_OF_QUEUING_PORTS SYSTEM_LIMIT_NUMBER_OF_QUEUING_PORTS
	typedef NAME_TYPE QUEUING_PORT_NAME_TYPE;
	typedef APEX_UNSIGNED QUEUING_PORT_ID_TYPE;

	//!Apex Structure. Not yet implmented
	struct QUEUING_PORT_STATUS_TYPE  {
		MESSAGE_RANGE_TYPE NB_MESSAGE;
		MESSAGE_RANGE_TYPE MAX_NB_MESSAGE;
		MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE;
		PORT_DIRECTION_TYPE PORT_DIRECTION;
		WAITING_RANGE_TYPE WAITING_PROCESSES;
	} ;

	//! \name ERROR constant and type definitions 
	//@{
	/* ! 
	* 
	*/

#define MAX_ERROR_MESSAGE_SIZE 64
	typedef APEX_INTEGER ERROR_MESSAGE_SIZE_TYPE;
	typedef APEX_BYTE ERROR_MESSAGE_TYPE[MAX_ERROR_MESSAGE_SIZE];
	
	//! Probably needs to be expanded.
	enum ERROR_CODE_TYPE{			
			DEADLINE_MISSED ,
			APPLICATION_ERROR,
			NUMERIC_ERROR ,
			ILLEGAL_REQUEST,
			STACK_OVERFLOW,
			MEMORY_VIOLATION ,
			HARDWARE_FAULT,
			POWER_FAIL 
	} ;
	//!Apex Structure
	struct ERROR_STATUS_TYPE  {
		ERROR_CODE_TYPE ERROR_CODE;
		ERROR_MESSAGE_SIZE_TYPE LENGTH;
		PROCESS_ID_TYPE FAILED_PROCESS_ID;
		SYSTEM_ADDRESS_TYPE FAILED_ADDRESS;
		ERROR_MESSAGE_TYPE MESSAGE;
	} ;


	//! \name BLACKBOARD constant and type definitions 
	//@{
	/* ! 
	* 
	*/

	#define MAX_NUMBER_OF_BLACKBOARDS SYSTEM_LIMIT_NUMBER_OF_BLACKBOARDS
typedef NAME_TYPE BLACKBOARD_NAME_TYPE;
//typedef APEX_INTEGER BLACKBOARD_ID_TYPE;
typedef APEX_UNSIGNED BLACKBOARD_ID_TYPE;
enum EMPTY_INDICATOR_TYPE{ EMPTY = 0, OCCUPIED = 1 } ;

//!APEX structure for storing blackboard status
struct BLACKBOARD_STATUS_TYPE{
EMPTY_INDICATOR_TYPE EMPTY_INDICATOR;
MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE;
WAITING_RANGE_TYPE WAITING_PROCESSES;
} ;


//! \name BUFFER constant and type definitions 
	//@{
	/* ! 
	* 
	*/

#define MAX_NUMBER_OF_BUFFERS SYSTEM_LIMIT_NUMBER_OF_BUFFERS
typedef NAME_TYPE BUFFER_NAME_TYPE;
typedef APEX_UNSIGNED BUFFER_ID_TYPE;
 
//!Apex Structure
struct BUFFER_STATUS_TYPE{
MESSAGE_RANGE_TYPE NB_MESSAGE;
MESSAGE_RANGE_TYPE MAX_NB_MESSAGE;
MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE;
WAITING_RANGE_TYPE WAITING_PROCESSES;
} ;


} //ACM_USER
#endif //APEX_TYPES_H

