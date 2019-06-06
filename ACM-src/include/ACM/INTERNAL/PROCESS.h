

/* The code in this file is a derived product based upon the code contained in mico/os-thread/pthread.h. 
*   The license of the original  MICO code is as follows.
 *  MICO --- an Open Source CORBA implementation
 *  Copyright (c) 1997-2008 by The Mico Team
 * 
 *  OSThread: An abstract Thread class for MICO
 *  Copyright (C) 1999 Andy Kersting & Andreas Schultz
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  For more information, visit the MICO Home Page at
 *  http://www.mico.org/
 *
 *  support for POSIX Draft 10ish Pthreads for Linux as developed by
 *  Xavier LeRoy@inria.fr
 *
 */

//  Here is the copy right statment for the vanderbilt modifications.
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

#ifndef _Arinc_Emulated_Process_h
#define _Arinc_Emulated_Process_h
//
// C++ Interface: ArincEmulatedProcess or PROCESS for Short
//
// Description: 
//
//
// Author:  <dabhishe@durip01>, 
// This is based on the MICOMT:Thread and Worker Thread Class
//
//

// ACM Specific Includes
#include <ACM/INTERNAL/ACM_THREAD_SUPPORT.h>
#include <ACM/USER/ACM_COMPONENT.h>
#include <ACM/USER/ACM_LOGGER.h>
#include <list>

//namespace  ACM_USER{
//class APEX;
//}


namespace ACM_INTERNAL{
	class APEX;

	//! Internal class that encapsulates Apex Processes.
	class PROCESS{

#ifdef TESTING_PROCESS_CLASS
	public:
#else
	public:
#endif


		typedef pthread_t ThreadID; //!< A Thread id 
		typedef pthread_key_t Thread_key;
		//! \name Enum Definitions
		//@{
		/* !
		* 
		*/

		/*!
		*  
		* This enum is internal to PROCESS and will be mapped to external state as defined by PROCESS_STATE_TYPE
		*/
		enum PROCESS_state
		{
			DORMANT,
			READY,
			RUNNING,
			WAITING_ON_RESOURCE,
			WAITING_ON_PERIOD,
			WAITING_PARTITION_NOT_NORMAL,
			RESTARTING,
			WAITING_SUSPENDED_OTHER,
			WAITING_SUSPENDED_SELF,
			SUSPENDED_FOR_HEALTH_REASONS
		};	

		/*!
		*
		*
		* A thread may either be created Detached or NotDetached (attached)
		* to its parent thread. The DETACH_FLAG is used to determine the
		* threads behavior when it exits.
		*/
		enum Detach_flag
		{
			DETACHED,		//!< Detached threads are capable of
			//!< reclaiming their resources on exit. 
			NOT_DETACHED		//!< Attached threads must be re-joined with
			//!< the parent thread, allowing on-exit
			//!< synchronization.
		};


		/*!
		*
		*
		* Error types associated with the Thread class.
		*/
		enum ERROR_TYPE
		{
			NO_ERROR,		//!< No error on the thread
			THREAD_FAILURE,		//!< The thread has failed
			INVALID_PRIORITY,	//!< The thread was given an invalid priority
			MUTEX_ALREADY_LOCKED,	//!< MUTEX is already locked
			NOT_STARTED,		//!< The thread has not started
			UNKNOWN_ERROR		//!< Unknown error.
		};


		friend class ACM_INTERNAL::APEX; //!<PROCESS is only exposed to APEX
		//! \name Member Data Types
		//@{
		/* !
		* 
		*/
		ThreadID id;		//!< The system thread id
		const PROCESS_ID_TYPE process_id; //!< A unique id used by Arinc Partition Controller to identify the process
		PROCESS_ATTRIBUTE_TYPE attribute;//!< PROCESS_ATTRIBUTE_TYPE has to be initialized at the time of construction.
		const Detach_flag thread_detached;	//!< Thread attached or detached?. Thread is always joinable
		ERROR_TYPE start_error;     //!< Used to carry starting error from ctor to start method
		PROCESS_state state;
		MUTEX		mutex_op_critical;	//!< Critical section mutex. Any change to the state must first aquire this mutex. This will happen in the setState method.
		SEMAPHORE	sem_state;	//!< State semaphore
		//MUTEX notificationMUTEX; //< Used by APC (Arinc Partition Controller) to get notified about the completion of this process
		//COND_VAR notificationCOND_VAR; //< Used by APC (Arinc Partition Controller) to get notified about the completion of this process

		PROCESS_RESPONSE_TYPE PROCESS_RESPONSE;
		void INITIALIZE_PROCESS_RESPONSE();

		std::list<SEMAPHORE_ID_TYPE> READLOCKS;
		std::list<SEMAPHORE_ID_TYPE> WRITELOCKS;

		 

		//! \name Constructors/Destructor
		//@{
		PROCESS(PROCESS_ATTRIBUTE_TYPE& attr,PROCESS_ID_TYPE unique_process_id);//!< PROCESS_attribute has to be initialized at the time of construction.
		virtual ~PROCESS();

		//! \name Getters
		//@{

		/*!
		* Method used to obtain the current state of the PROCESS
		*
		*/
		PROCESS_state get_state() const;
		std::string get_name() const;

		std::string get_string_state() const{
			switch(state)
			{
			case PROCESS::DORMANT:
				return ("DORMANT");
				break;
			case PROCESS::READY:
				return("READY");
				break;

			case PROCESS::RUNNING:
				return("RUNNING");
				break;
			case PROCESS::WAITING_ON_RESOURCE:
				return ("WAITING_ON_RESOURCE");
				break;
			case PROCESS::WAITING_ON_PERIOD:
				return ("WAITING_ON_PERIOD");
				break;

			case PROCESS::WAITING_PARTITION_NOT_NORMAL:
				return("WAITING_PARTITION_NOT_NORMAL");
				break;

			case PROCESS::RESTARTING:
				return("RESTARTING");
				break;
			default:
				return("Do not know state");
			}
		}


		/*!
		* This method gets the current process_id
		*/
		PROCESS_ID_TYPE get_process_id(){
			return this->process_id;
		}


		//! \name StateModification Methods

		/*!
		* This method will allow to implment wating on period for a periodic process.
		*/
		void waiting_on_period(/*out*/ACM_USER::RETURN_CODE_TYPE* return_code);
		void set_state (PROCESS_state s);

		//! \name Event Notification Methods
		//@{

		void post_state_change();
		//bool waitOnNotificationCondition();


		//! \name Methods to support APEX APIs. Only visible from inside APC.
		//@{
		//
		/*!
		Call start() to start  periodic processes at initialization
		use start() to release aperiodic processes anytime
		*/
		void start();


		/*!
		*\code
		After Calling stop you must decide if you want to call start again on the process
		\endcode
		*/
		void stop(bool restart=true); //!< Will cause a restart of the thread
		void stop_self();
		void suspend_self();



		/*!
		* Restarts the thread
		*/
		void restart(bool doMitigation=false);

		//! Methods for implementing Priorities

		//! \name Priority Methods
		//@{
		/*
		* Get Current Priority of this Thread Object. Returns 
		*/

		virtual long get_priority();
		/*!
		* The priority method is used to set new priority of the thread
		*/
		//method that sets the priortiy of a thread to the given value

		virtual void set_priority(long priority );


		/*!\name Methods for implementing ThreadSpecificData*/
		/*!\name ThreadKey
		*
		* An opaque type for thread key types. A thread key
		* is used to create and access thread specific data.
		*/


		/*!  
		* The create_key method creates a thread specific data key. This key can
		* be associated with an arbitrary data value using set_specific()
		* method.
		* \param key  The key to create
		* \param __cleanup  A cleanup method for the key
		*/
		static void
			create_key(Thread_key& key, void (*__cleanup) (void *) = 0);

		/*!
		* The delete_key method is used to delete a key, but not the data
		* associated with it. That is done on thread exit or cancellation.
		*
		* \param key  The key to delete
		*/
		static void
			delete_key(Thread_key& key);
		/*!
		* The get_specific method is used to retrieve thread specific data
		* for a key.
		*
		* \param key  The key for which to get data
		* \return  A pointer to the thread specific data
		*/
		static void*
			get_specific(const Thread_key& key);
		/*!
		* The set_specific method is used to associate thread specific
		* data with a key.
		*
		* \param key  The key to associate with data
		* \param value  Thread specific data
		*/
		static void
			set_specific(const Thread_key& key, void *value);

		/*! \name Methods for implementing Thread id functions
		*/


		/*!  
		* This method returns the threads system process id.
		*
		* \return The System tid. For Main process it return pid
		*/
		static pid_t tid(){
			return syscall( SYS_gettid ) ; 
		}

		/*!  
		* This method returns the threads system (pthread) id.
		*
		* \return The thread's system id
		*/
		inline ThreadID thread_id() const
		{
			return id;
		}

		/*!
		* This method returns the threads system (pthread) id. This
		* method may be called without a thread object since it is
		* static.
		*
		* \return The thread's system id.
		*/
		static ThreadID  self();

		/*!
		* This method is used to compare if the PROCESS is equivalent i.e. its thread id is same to the other PROCESS
		*/
		virtual bool operator==(const PROCESS &aep) const;

		/*! \name Methods for running and creating threads
		* This Method is used to pass an address of a function pointer to the pthread_create call
		*     
		*/
		static void*  Thread_wrapper(void* arg);
		/*!
		* This method is used to actually create the thread context and
		* start the thread initialization.
		*
		* \return A thread error condition
		*/
		ERROR_TYPE create_thread();


		static void  Thread_cleanup_Handler (void* data);
		/*!
		* The run method is method which every thread executes. It's the
		* right place to do the work. The user can easily use Thread
		* class similar in Java i.e. inherit from Thread to custom class,
		* overload run method and call start on class instance.
		*/
		virtual void run() ;

		/*!
		* This method effectively cancels the thread from outside the
		* thread's context, effectively preventing cleanup.
		*/
		void terminate();




		/*!
		* This method blocks the current thread until the thread being called
		* upon exits.
		*
		* \param exitval  A place to store the exit value of the finished
		* thread
		*/
		void wait(void** exitval = NULL);

		inline bool isPeriodic()
		{
			return (attribute.PERIOD != INFINITE_TIME_VALUE);
		}

	private:
		void initialize(); 

		void delayed_start (SYSTEM_TIME_TYPE timeout);
		bool delayed_start_flag;
		SYSTEM_TIMESPEC absolute_delay_start_time;
		
		
	};

	typedef PROCESS* PROCESS_ptr;

} // ACM_INTERNAL



#endif //_Arinc_Emulated_Process_h

