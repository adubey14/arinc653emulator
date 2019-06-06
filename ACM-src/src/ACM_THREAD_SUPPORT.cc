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
#ifndef _ACM_Thread_Support_cc
#define _ACM_Thread_Support_cc

#ifndef WIN32
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
#else
#include <ACM/INTERNAL/ACM_THREAD_SUPPORT.H>
#endif
#include <stdlib.h>

namespace ACM_INTERNAL{
	/**************************************************************************************************************************/
	MUTEX::MUTEX(bool locked, Attribute attr):mutextype(attr)
	{
		int result;

		pthread_mutexattr_t m_attr;
		result = pthread_mutexattr_init(&m_attr);
		assert(!result);
		if (attr != NORMAL) {
			switch (attr) {
		case RECURSIVE:
			result = pthread_mutexattr_settype(&m_attr,PTHREAD_MUTEX_RECURSIVE);
			assert (!result);
			break;
		case ERRORCHECK:
			result = pthread_mutexattr_settype(&m_attr,PTHREAD_MUTEX_ERRORCHECK);
			assert (!result);
			break;

		default:
			break;
			}
		}
		result = pthread_mutex_init(&mutex, &m_attr);
		assert(!result);
		result = pthread_mutexattr_destroy(&m_attr); 
		assert(!result);
		if (locked)
			this->lock();
	}

	/*!
	* The destructor frees resources used by the mutex.
	*/
	MUTEX::~MUTEX()
	{ 
		int result = pthread_mutex_destroy(&mutex);
		switch (result)
		{
		case EBUSY:
			//	LOGGER(INFO,"In Mutex Destructor: Return Code from Destroy is EBUSY");
			break;
		case EINVAL:
			//LOGGER(INFO,"In Mutex Destructor: Return Code from Destroy is EINVAL");
			break;
		default:
			break;
		}
		//assert(!result);
	}

	/**************************************************************************************************************************/
	/*!
	* Use AUTO_LOCK to set a MUTEX inside a function
	* The mutex will be automatically release when it the auto lock object goes out of scope
	* e.g. void foo (){
	* ACM::MUTEX MyMUTEX;
	* ACM::Autolock(MyMUTEX);
	* } // Auto lock gets released because destructor is called
	*/

	AUTO_LOCK::AUTO_LOCK(MUTEX& m): mymutex(m){
		mymutex.lock();
	}
	AUTO_LOCK::~AUTO_LOCK(){
		mymutex.unlock();
	}

	/**************************************************************************************************************************/
	/*!
	* A condition variable is a synchronization object that associates
	* a condition with a mutex.
	*/

	COND_VAR::COND_VAR(MUTEX* mut)
		: mutex(mut)
	{
		int result=0;

		pthread_condattr_t cattr;
#ifdef HAVE_PTHREAD_CONDATTR_SETCLOCK
		result = pthread_condattr_setclock(&cattr,CLOCK_REALTIME); //all conditions will use the clock_realtime
		assert(!result);
#endif
		result = pthread_cond_init(&cond, &cattr);
		assert(!result);
	}

	/*!
	* The destructor frees the resources used by the condition variable.
	*/
	COND_VAR::~COND_VAR()
	{
		int result = pthread_cond_destroy(&cond);
		switch (result)
		{
		case EBUSY:
			//LOGGER(INFO,"In COND_VAR Destructor: Return Code from Destroy is EBUSY");
			break;
		case EINVAL:
			//LOGGER(INFO,"In COND_VAR Destructor: Return Code from Destroy is EINVAL");
			break;
		default:
			break;
		}
	}
	/*!specify time out in milliseconds
	bool COND_VAR::timed_wait_milli(long tmout){
	SYSTEM_TIMESPEC timeout;
	SYSTEM_TIMESPEC now= currentTime();
	timeout=addTimeMilli(now,tmout);
	int result;
	assert((result = pthread_cond_timedwait(&cond, &mutex->mutex, &timeout)) != EINVAL);
	return (result == ETIMEDOUT);
	}*/

	int COND_VAR::timed_wait(SYSTEM_TIME_TYPE TIME_OUT){
		SYSTEM_TIMESPEC timeout ;
		timeout.tv_sec=0;timeout.tv_nsec=0;

		SYSTEM_TIMESPEC now= currentTime();
		timeout=addTimeNano(now,TIME_OUT);
		int result=0;

		//std::cout <<" time spec "<<timeout.tv_sec << "."<< timeout.tv_nsec <<std::endl;
		assert((result = pthread_cond_timedwait(&cond, &mutex->mutex, &timeout)) != EINVAL);
		return (result);
	}

	int COND_VAR::timed_wait(SYSTEM_TIMESPEC timeout){
		int result=0;
		assert((result = pthread_cond_timedwait(&cond, &mutex->mutex, &timeout)) != EINVAL);
		return (result);
	}

	int COND_VAR::wait(){
		return (pthread_cond_wait(&cond,&mutex->mutex));
	}

	void COND_VAR::broadcast(){
		pthread_cond_broadcast(&cond);
	}


	void COND_VAR::signal(){
		pthread_cond_signal(&cond);
	}

	/**************************************************************************************************************************/
	/*!
	* The RW_LOCK class implements a mutex-like object that can be locked
	* for either reading or writing.
	xxlock() and unlock() are inlined for optimal speed
	*/

	RW_LOCK::RW_LOCK(){
		int result = pthread_rwlock_init(&rwlock, NULL);
		assert(!result);
	}

	/*!
	* The destructor frees the resources used by the lock.
	*/
	RW_LOCK::~RW_LOCK(){
		int result = pthread_rwlock_destroy(&rwlock);
		switch (result)
		{
		case EBUSY:
			//LOGGER(INFO,"In RW_LOCK Destructor: Return Code from Destroy is EBUSY");
			break;
		case EINVAL:
			//LOGGER(INFO,"In RW_LOCK Destructor: Return Code from Destroy is EINVAL");
			break;
		default:
			break;
		}
	}

	void RW_LOCK::unlock(){
		pthread_rwlock_unlock(&rwlock);
	}


	/**************************************************************************************************************************/
	/*!The semaphore class is a synchronization object. A semaphore is a
	* counter for resources shared between threads.
	*/

	SEMAPHORE::SEMAPHORE(SEMAPHORE_VALUE_TYPE value,SEMAPHORE_VALUE_TYPE MAX_VALUE): mutex(false,MUTEX::ERRORCHECK),condition(&mutex),counter(value),SEMAPHORE_NAME("NO_NAME"),maximumvalue(MAX_VALUE),APEX_UNSIGNED_WAITING_COUNTER(0),APEX_UNSIGNED_WAITING_COUNTER_MUTEX(false,MUTEX::NORMAL)
	{

	}
	SEMAPHORE::SEMAPHORE(SEMAPHORE_NAME_TYPE name,SEMAPHORE_VALUE_TYPE value,SEMAPHORE_VALUE_TYPE MAX_VALUE ): mutex(false,MUTEX::ERRORCHECK),condition(&mutex),counter(value),SEMAPHORE_NAME(name),maximumvalue(MAX_VALUE),APEX_UNSIGNED_WAITING_COUNTER(0),APEX_UNSIGNED_WAITING_COUNTER_MUTEX(false,MUTEX::NORMAL)
	{
	}
	SEMAPHORE::~SEMAPHORE(){

	}
	RETURN_CODE_TYPE SEMAPHORE::wait(SYSTEM_TIME_TYPE timeout)
	{
		SYSTEM_TIMESPEC timeout_spec;
		SYSTEM_TIMESPEC now= currentTime();
		timeout_spec=addTimeNano(now,timeout);
		RETURN_CODE_TYPE RETURNVALUE=ACM_USER::NO_ERROR;
		int result =0;

		if(timeout== INFINITE_TIME_VALUE)
		{
			return this->wait();
		}
		else
		{
			{
				AUTO_LOCK lock (this->APEX_UNSIGNED_WAITING_COUNTER_MUTEX);
				APEX_UNSIGNED_WAITING_COUNTER ++;
			}
			AUTO_LOCK t_lock(mutex);
			while(counter == 0){
				result = condition.timed_wait(timeout_spec);
				if (result==ETIMEDOUT)
				{
					RETURNVALUE=ACM_USER::TIMED_OUT;
					break;
				}
			}
			if(RETURNVALUE!=ACM_USER::TIMED_OUT)
			{
				counter--;
			}
			{
				AUTO_LOCK lock (this->APEX_UNSIGNED_WAITING_COUNTER_MUTEX);
				APEX_UNSIGNED_WAITING_COUNTER --;
			}
			return RETURNVALUE;

		}
	}
	RETURN_CODE_TYPE SEMAPHORE::wait(SYSTEM_TIMESPEC& timeout_spec)
	{
		RETURN_CODE_TYPE RETURNVALUE=ACM_USER::NO_ERROR;
		int result =0;

		
			{
				AUTO_LOCK lock (this->APEX_UNSIGNED_WAITING_COUNTER_MUTEX);
				APEX_UNSIGNED_WAITING_COUNTER ++;
			}
			AUTO_LOCK t_lock(mutex);
			while(counter == 0){
				result = condition.timed_wait(timeout_spec);
				if (result==ETIMEDOUT)
				{
					RETURNVALUE=ACM_USER::TIMED_OUT;
					break;
				}
			}
			if(RETURNVALUE!=ACM_USER::TIMED_OUT)
			{
				counter--;
			}
			{
				AUTO_LOCK lock (this->APEX_UNSIGNED_WAITING_COUNTER_MUTEX);
				APEX_UNSIGNED_WAITING_COUNTER --;
			}
			return RETURNVALUE;

		
	}

	RETURN_CODE_TYPE SEMAPHORE::wait()
	{
		{
			AUTO_LOCK lock (this->APEX_UNSIGNED_WAITING_COUNTER_MUTEX);
			APEX_UNSIGNED_WAITING_COUNTER ++;
		}
		AUTO_LOCK t_lock(mutex);
		while(counter == 0){
			condition.wait();
		}
		counter--;
		{
			AUTO_LOCK lock (this->APEX_UNSIGNED_WAITING_COUNTER_MUTEX);
			APEX_UNSIGNED_WAITING_COUNTER --;
		}
		return ACM_USER::NO_ERROR;
	}

	RETURN_CODE_TYPE SEMAPHORE::try_lock() 
	{
		AUTO_LOCK T_lock(mutex);
		if (counter == 0){
			return NOT_AVAILABLE;
		}
		return ACM_USER::NO_ERROR;
	}
	RETURN_CODE_TYPE SEMAPHORE::post()
	{
		//LOGGER(INFO,"INSIDE SEMAPHORE POST");
		{
			AUTO_LOCK t_lock(mutex);
			//LOGGER(INFO,"LOCKED MUTEX INSIDE POST");
			if(counter>=maximumvalue){
				//LOGGER(INFO,"PROBLEM IN THE SEMAPHOR POST");
				return NOT_AVAILABLE;
			}
			counter++;
		}
		//LOGGER(INFO,"INSide semaphore post: signalled condition");
		condition.signal();
		return ACM_USER::NO_ERROR;
	}



}//ACM

#endif //_ACM_Support_cc
