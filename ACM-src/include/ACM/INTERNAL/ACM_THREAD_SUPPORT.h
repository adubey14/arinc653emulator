/* The code in this file is a derived product based upon the code contained in
* mico/os-thread/pthread.h.
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

#ifndef _ACM_Support_h
#define _ACM_Support_h

/*! The classes in this file are derived from corresponding classes in MICO
 * library
*/

// ACM Specific Includes
#include <ACM/USER/ACM_EXCEPTION.h>
#include <ACM/USER/ACM_UTILITIES.h>
#include <ACM/USER/APEX_TYPES.h>
#include <stdio.h>

using namespace ACM_USER;
namespace ACM_INTERNAL {
#define false 0
#define true 1

class COND_VAR;
//! An internal class that provides a wrapper for working with pthread_mutex.
class MUTEX {
public:
  pthread_mutex_t mutex; //!< Pthread mutex object

public:
  friend class COND_VAR;
  /*!
  * \enum Attribute
  *
  * The Attribute enum defines a specific attribute for the MUTEX
  * class. An attribute may be either Normal or Recursive. A normal
  * mutex may only be locked once by a thread. Attempts to lock it
  * more than once will result in a deadlock. A recursive mutex may
  * be locked multiple times by the same thread.
  */
  enum Attribute { NORMAL, ERRORCHECK, RECURSIVE };

  /*!
  * \enum ERROR_TYPE
  *
  * Error types associated with a mutex.
  */
  enum ERROR_TYPE {
    NO_ERROR,       //!< No error on the mutex
    ALREADY_LOCKED, //!< The mutex is already locked.
    TRY_AGAIN,      //!< Try locking again.
    INVALID,        //!< Invalid mutex.
    FAULT,          //!< A fault occurred.
    DEADLOCK,       //!< The operation would result in deadlock
    UNKNOWN_ERROR   //!< An unknown error occurred
  };

  //! \name Constructor/Destructor
  //@{
  MUTEX(bool locked = false, Attribute attr = NORMAL);
  ~MUTEX();
  //@}

  //! \name MUTEX Operations
  //@{
  /*!
  * The trylock method is used to sample the mutex to see if it
  * is locked. If the mutex is not locked the thread will obtain
  * the lock, returning true. Otherwise, an error condition will
  * be returned, and the thread will not own the lock.
  *
  * \return  True if the thread obtains the lock.
  */
  inline long try_lock() {
    ERROR_TYPE ret = MUTEX::NO_ERROR;
    switch (pthread_mutex_trylock(&mutex)) {
    case 0:
      ret = MUTEX::NO_ERROR;
      break;
    case EBUSY:
      ret = MUTEX::ALREADY_LOCKED;
      break;
    case EAGAIN:
      ret = MUTEX::TRY_AGAIN;
      break;
    case EINVAL:
      ret = MUTEX::INVALID;
      break;
    case EFAULT:
      ret = MUTEX::FAULT;
      break;
    case EDEADLK:
      ret = MUTEX::DEADLOCK;
      break;
    default:
      ret = MUTEX::UNKNOWN_ERROR;
    }
    return ret;
  }

  inline long try_lock_without_locking() {
    long value = this->try_lock();
    if (value == MUTEX::NO_ERROR) {
      this->unlock();
    }
    return value;
  }

  /*!
  * The lock method is used to lock the mutex. A thread locking
  * the mutex will have ownership of the lock until it is unlocked.
  * Any thread trying to subsequently lock the mutex will block
  * until the owning thread has unlocked it.
  */
  inline void lock() { pthread_mutex_lock(&mutex); }

  /*!
  * The unlock method is used to release the ownership of a mutex.
  * Threads waiting on the locked mutex are re-woken and may attempt
  * to lock the mutex themselves.
  */
  inline void unlock() { pthread_mutex_unlock(&mutex); }

  inline int timedlock(const SYSTEM_TIMESPEC &value) {
    return pthread_mutex_timedlock(&mutex, &value);
  }

private:
  Attribute mutextype;

public:
  inline Attribute getMutexType() const { return this->mutextype; }
};

//! AUTO_LOCK is used to automatically set and release mutex.
/*!
* Use AUTO_LOCK to set a MUTEX inside a function
* The mutex will be automatically release when it the auto lock object goes out
* of scope
* e.g. void foo (){
* ACM::MUTEX MyMUTEX;
* ACM::AUTO_LOCK(MyMUTEX);
* } // Auto lock gets released because destructor is called
*/
class AUTO_LOCK {
  MUTEX &mymutex;

public:
  //! \name Constructor/Destructor
  //@{
  /*!
  * \param m  The mutex to lock
  * The constructor will lock the supplied mutex.
  */
  AUTO_LOCK(MUTEX &m);

  /*!
  * The destructor unlocks the mutex.
  */
  ~AUTO_LOCK();
  //@}
};
//
// Conditional Variable
//

//! A condition variable is a synchronization object that associates a condition
//! with a mutex.
class COND_VAR {
public:
  pthread_cond_t cond; //!< The pthread condition variable
  MUTEX *mutex;        //!< The mutex which will be used by this class

public:
  //! \name Constructor/Destructor
  //@{
  COND_VAR(MUTEX *m);
  ~COND_VAR();
  //@}

  //! \name Condition Variable Operations
  //@{
  /*!
  * The wait method blocks the calling thread until it is woken
  * up by another thread signalling or broadcasting to the condition
  * variable.
  *
  * \return int Return code from pthread_cond_wait
  */
  int wait();

  // bool timed_wait_milli(long tmout);
  /*!
  * The timedwait method blocks the calling thread until it
  * is woken up by another thread signalling or broadcasting to
  * the condition variable. It can also be woken up after the
  * elapsed time specified by tmout.
  *
  * \param TIME_OUT  SYSTEM_TIME_TYPE (units: nanoseconds) to wait for a signal.
  * \return  [ETIMEDOUT]:  The time specified by abstime to
  * pthread_cond_timedwait() has passed. [EINVAL]  The value specified by
  * abstime is invalid.
  */

  int timed_wait(SYSTEM_TIME_TYPE TIME_OUT);
  /*!
  * The timedwait method blocks the calling thread until it
  * is woken up by another thread signalling or broadcasting to
  * the condition variable. It can also be woken up after the
  * elapsed time specified by tmout.
  *
  * \param TIME_OUT  SYSTEM_TIMESPEC (absolute time) to wait for a signal.
  * \return  [ETIMEDOUT]:  The time specified by abstime to
  * pthread_cond_timedwait() has passed. [EINVAL]  The value specified by
  * abstime is invalid.
  */
  int timed_wait(SYSTEM_TIMESPEC TIME_OUT);
  /*!
  * The broadcast method wakes up all threads waiting on the
  * condition variable.
  */
  void broadcast();

  /*!
  * The signal method wakes up exactly one thread waiting
  * on the condition variable.
  */
  void signal();
  //@}
};

//! reader/writer lock
/*!
* The RW_LOCK class implements a mutex-like object that can be locked
* for either reading or writing.
*/
class RW_LOCK {

private:
  pthread_rwlock_t rwlock; //!< The pthread read/write lock
public:
  //! \name Constructor/Destructor
  //@{
  RW_LOCK();
  ~RW_LOCK();
  //@}

  /*!
  * The rdlock method locks the reading side of the lock.
  */
  inline void rdlock() { pthread_rwlock_rdlock(&rwlock); }

  /*!
  * The wrlock method locks the writing side of the lock.
  */
  inline void wrlock() { pthread_rwlock_wrlock(&rwlock); }
  /*!
  * The unlock method releases the lock.
  */
  void unlock();
};

class RW_LOCK_SEM;

//! The semaphore class is a synchronization object. A semaphore is a  counter
//! for resources shared between threads.
class SEMAPHORE {
  MUTEX mutex;
  COND_VAR condition;
  SEMAPHORE_VALUE_TYPE counter;
  std::string SEMAPHORE_NAME;
  SEMAPHORE_VALUE_TYPE maximumvalue;
  APEX_UNSIGNED APEX_UNSIGNED_WAITING_COUNTER;
  MUTEX APEX_UNSIGNED_WAITING_COUNTER_MUTEX;
  friend class RW_LOCK_SEM;

public:
  std::string GET_NAME() { return this->SEMAPHORE_NAME; }
  void initialize() {
    // Dosomething if required
  }

public:
  enum ERROR_TYPE {
    NO_ERROR,
    NO_PERMISSION,
    TRY_AGAIN,
    SEM_INVALID,
    INTERRUPTED,
    UNKOWN_ERROR
  };

  SEMAPHORE(SEMAPHORE_VALUE_TYPE value = 0,
            SEMAPHORE_VALUE_TYPE MAX_VALUE = MAX_SEMAPHORE_VALUE);
  SEMAPHORE(SEMAPHORE_NAME_TYPE name, SEMAPHORE_VALUE_TYPE value = 0,
            SEMAPHORE_VALUE_TYPE MAX_VALUE = MAX_SEMAPHORE_VALUE);
  ~SEMAPHORE();

  APEX_UNSIGNED GET_WAITING_COUNT() {
    return this->APEX_UNSIGNED_WAITING_COUNTER;
  }
  SEMAPHORE_VALUE_TYPE GET_SEMAPHORE_COUNT() { return this->counter; }

  SEMAPHORE_VALUE_TYPE GET_MAXIMUM_VALUE() { return this->maximumvalue; }

  RETURN_CODE_TYPE wait();

  RETURN_CODE_TYPE wait(SYSTEM_TIME_TYPE timeout);
  RETURN_CODE_TYPE wait(SYSTEM_TIMESPEC &timeout_spec);

  RETURN_CODE_TYPE try_lock();

  RETURN_CODE_TYPE post();
};

/*	Possible Problem
02.979391204|TRC|START_PROCESS: PERIODIC_PROCESS_ID_2
02.979407617|APP|anotherperiodic: locking write
04.979217980|HME|WARNING, Soft Deadline Violation for PERIODIC_PROCESS_ID_2:
part1
04.979244729|HME|Informing HM of 1 of deadline violation
04.979271408|HME|HM Not available for process id 1
04.979326373|TRC|START_PROCESS: PERIODIC_PROCESS_ID_1
04.979348513|APP|testperiodic: locking read
04.979454392|APP|anotherperiodic: UNlocking write
05.979289145|HME|WARNING, Soft Deadline Violation for PERIODIC_PROCESS_ID_2:
part1
05.979312612|HME|Informing HM of 1 of deadline violation
05.979328745|HME|HM Not available for process id 1
06.979188856|HME|DETECT HARD DEADLINE VIOLATION for PERIODIC_PROCESS_ID_1:part1
06.979415142|HME|PERIODIC_PROCESS_ID_1 STOPPED
06.979439167|HME|RAISE_ERROR for PERIODIC_PROCESS_ID_1: part1
06.979456208|HME|HM Not available for process id 0
06.979493015|HME|WARNING, Soft Deadline Violation for PERIODIC_PROCESS_ID_2:
part1
06.979510196|HME|Informing HM of 1 of deadline violation
06.979525421|HME|HM Not available for process id 1
06.979560481|APP|anotherperiodic:  Finished Work in anotherperiodic:The return
code is 0
06.979582830|TRC|END_PROCESS: PERIODIC_PROCESS_ID_2  */

//! A read write lock implementation using semaphore
class RW_LOCK_SEM {
private:
  MUTEX Writemutex;
  MUTEX Readmutex;
  std::string NAME;
  SEMAPHORE_VALUE_TYPE MAX_READERS;
  SEMAPHORE RW_SEM;
  SEMAPHORE WRITER_SEMAPHORE;

public:
  RW_LOCK_SEM(std::string Name, SEMAPHORE_VALUE_TYPE num_readers)
      : Writemutex(false, MUTEX::ERRORCHECK),
        Readmutex(false, MUTEX::ERRORCHECK), NAME(Name),
        MAX_READERS(num_readers), RW_SEM(num_readers, num_readers),
        WRITER_SEMAPHORE(1, 1) {}
  void readlock(SYSTEM_TIME_TYPE TIMEOUT, RETURN_CODE_TYPE *RETURN_CODE) {
    // AUTO_LOCK T_LOCK(Readmutex);
    *RETURN_CODE = RW_SEM.wait(TIMEOUT);
  }
  std::string GET_NAME() { return this->NAME; }

  void readunlock() {
    // AUTO_LOCK T_LOCK(Readmutex);
    RW_SEM.post();
  }

  void writelock(SYSTEM_TIME_TYPE TIMEOUT, RETURN_CODE_TYPE *RETURN_CODE) {
    SYSTEM_TIMESPEC timeout_spec;
    SYSTEM_TIMESPEC now = currentTime();
    timeout_spec = addTimeNano(now, TIMEOUT);

    // AUTO_LOCK T_LOCK(Writemutex);
    *RETURN_CODE = WRITER_SEMAPHORE.wait(
        timeout_spec); // wait for writer token to be available.
    if (*RETURN_CODE == TIMED_OUT)
      return;

    int readcount = 0;
    int successcount = 0;
    for (readcount = 0; readcount < MAX_READERS; readcount++) {

      *RETURN_CODE = RW_SEM.wait(timeout_spec);
      if (*RETURN_CODE == TIMED_OUT) {
        LOGGER(DEBUG, "Got a time out in writelock \n");

        break;
      }
      successcount++;
      LOGGER(DEBUG, "successcount is %d \n", successcount);
    }

    if (*RETURN_CODE == TIMED_OUT) {
      {
        AUTO_LOCK T1_LOCK(RW_SEM.mutex);
        RW_SEM.counter += successcount;
      }
      RW_SEM.condition.signal();
    }

    WRITER_SEMAPHORE.post();
  }

  void writeunlock() {
    // AUTO_LOCK T_LOCK(Writemutex);
    // AUTO_LOCK T1_LOCK(Readmutex);
    {
      AUTO_LOCK T1_LOCK(RW_SEM.mutex);
      RW_SEM.counter = MAX_READERS;
    }
    RW_SEM.condition.signal();
  }
};

} // ACM
#endif //_ACM_Support_h
