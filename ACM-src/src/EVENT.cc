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

#ifndef ACM_APEX_EVENT_CC
#define ACM_APEX_EVENT_CC
#ifndef WIN32
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
#else
#include <ACM/INTERNAL/EVENT.h>
#endif
#include <stdlib.h>

namespace ACM_INTERNAL {

APEX_EVENT_TYPE::~APEX_EVENT_TYPE() {
  // deleted;
  delete (EVENT_CONDITION_VARIABLE);
  if (this->CreatedMutex)
    delete (EVENT_CONDITION_VARIABLE_MUTEX);
}

APEX_EVENT_TYPE::APEX_EVENT_TYPE(std::string name, MUTEX *useMutex, bool locked,
                                 MUTEX::Attribute attr)
    : EVENT_STATE(DOWN), EVENT_NAME(name), APEX_UNSIGNED_COUNTER(0),
      APEX_UNSIGNED_COUNTER_MUTEX(false, MUTEX::NORMAL), CreatedMutex(false) {
  if (useMutex == 0) {
    EVENT_CONDITION_VARIABLE_MUTEX = new MUTEX(locked, attr);
    this->CreatedMutex = true;
  } else
    EVENT_CONDITION_VARIABLE_MUTEX = useMutex;

  EVENT_CONDITION_VARIABLE = new COND_VAR(EVENT_CONDITION_VARIABLE_MUTEX);

  // Do Something Here
}

std::string APEX_EVENT_TYPE::GET_NAME() { return this->EVENT_NAME; }

APEX_EVENT_TYPE *APEX_EVENT_TYPE::CREATE_EVENT(std::string name,
                                               MUTEX *useMutex, bool locked,
                                               MUTEX::Attribute attr) {
  return (new APEX_EVENT_TYPE(name, useMutex, locked, attr));
}

void APEX_EVENT_TYPE::SET_EVENT(RETURN_CODE_TYPE *RETURN_CODE) {
  if (this->EVENT_STATE == DOWN) {
    this->EVENT_STATE = UP;
    this->EVENT_CONDITION_VARIABLE_MUTEX
        ->try_lock(); // if its already locked and is Normal, it will not lock
    // LOGGER(ACM_USER::DEBUG,"Locked Mutex for Event %s",EVENT_NAME.c_str());
    this->EVENT_CONDITION_VARIABLE->broadcast();
    // LOGGER(ACM_USER::DEBUG,"Broadcast message sent for Event
    // %s",EVENT_NAME.c_str());
    this->EVENT_CONDITION_VARIABLE_MUTEX->unlock();
    // LOGGER(ACM_USER::DEBUG,"UnLocked Mutex for Event %s",EVENT_NAME.c_str());
  }

  LOGGER(ACM_USER::DEBUG, "Setting Event %s", EVENT_NAME.c_str());

  *RETURN_CODE = NO_ERROR;
}
void APEX_EVENT_TYPE::RESET_EVENT(RETURN_CODE_TYPE *RETURN_CODE) {
  this->EVENT_STATE = DOWN;

  // LOGGER(ACM_USER::DEBUG,"RESETTING Event %s",EVENT_NAME.c_str());

  *RETURN_CODE = NO_ERROR;
}
void APEX_EVENT_TYPE::WAIT_EVENT(RETURN_CODE_TYPE *RETURN_CODE) {
  {
    AUTO_LOCK lock(this->APEX_UNSIGNED_COUNTER_MUTEX);
    APEX_UNSIGNED_COUNTER++;
  }

  AUTO_LOCK another_Lock(*this->EVENT_CONDITION_VARIABLE_MUTEX);
  while (this->EVENT_STATE == DOWN) {
    this->EVENT_CONDITION_VARIABLE->wait();
  }
  {
    AUTO_LOCK lock(this->APEX_UNSIGNED_COUNTER_MUTEX);
    APEX_UNSIGNED_COUNTER--;
  }
  *RETURN_CODE = NO_ERROR;
}

void APEX_EVENT_TYPE::WAIT_EVENT(SYSTEM_TIME_TYPE TIME_OUT,
                                 RETURN_CODE_TYPE *RETURN_CODE) {
  SYSTEM_TIMESPEC timeout;
  SYSTEM_TIMESPEC now = currentTime();
  timeout = addTimeNano(now, TIME_OUT);

  int result = 0;
  AUTO_LOCK another_Lock(*this->EVENT_CONDITION_VARIABLE_MUTEX);

  while (this->EVENT_STATE == DOWN) {
    result = this->EVENT_CONDITION_VARIABLE->timed_wait(
        timeout); // wait until timeout
    if (result == ETIMEDOUT) {
      break;
    }
  }

  {
    AUTO_LOCK lock(this->APEX_UNSIGNED_COUNTER_MUTEX);
    APEX_UNSIGNED_COUNTER--;
  }

  *RETURN_CODE = (result == ETIMEDOUT) ? TIMED_OUT : NO_ERROR;
}

void APEX_EVENT_TYPE::WAIT_EVENT(SYSTEM_TIMESPEC timeout,
                                 RETURN_CODE_TYPE *RETURN_CODE) {

  int result = 0;
  AUTO_LOCK another_Lock(*this->EVENT_CONDITION_VARIABLE_MUTEX);

  while (this->EVENT_STATE == DOWN) {
    result = this->EVENT_CONDITION_VARIABLE->timed_wait(
        timeout); // wait until timeout
    if (result == ETIMEDOUT) {
      break;
    }
  }

  {
    AUTO_LOCK lock(this->APEX_UNSIGNED_COUNTER_MUTEX);
    APEX_UNSIGNED_COUNTER--;
  }

  *RETURN_CODE = (result == ETIMEDOUT) ? TIMED_OUT : NO_ERROR;
}

void APEX_EVENT_TYPE::WAIT_AND_LOCK_EVENT(SYSTEM_TIMESPEC TIME_OUT,
                                          RETURN_CODE_TYPE *RETURN_CODE) {
  // when you enter this method process_table mutex should be unlocked;
  //	assert(this->EVENT_CONDITION_VARIABLE_MUTEX->try_lock_without_locking()
  //== MUTEX::NO_ERROR);
  int result = 0;
  // AUTO_LOCK another_Lock (*this->EVENT_CONDITION_VARIABLE_MUTEX);
  //<!Lock
  this->EVENT_CONDITION_VARIABLE_MUTEX->lock();
  //<!UNlock and Lock
  while (this->EVENT_STATE == DOWN) {
    result = this->EVENT_CONDITION_VARIABLE->timed_wait(
        TIME_OUT); // wait until timeout
    if (result == ETIMEDOUT) {
      this->EVENT_CONDITION_VARIABLE_MUTEX->lock();
      break;
    }
  }

  {
    AUTO_LOCK lock(this->APEX_UNSIGNED_COUNTER_MUTEX);
    APEX_UNSIGNED_COUNTER--;
  }
  // finally

  //	assert(this->EVENT_CONDITION_VARIABLE_MUTEX->try_lock_without_locking()
  //== MUTEX::ALREADY_LOCKED);

  *RETURN_CODE = (result == ETIMEDOUT) ? TIMED_OUT : NO_ERROR;
}

APEX_UNSIGNED APEX_EVENT_TYPE::GET_COUNT() {
  return this->APEX_UNSIGNED_COUNTER;
}

EVENT_STATE_TYPE APEX_EVENT_TYPE::GET_STATE() { return this->EVENT_STATE; }

void APEX_EVENT_TYPE::LOCK_EVENT_MUTEX() {
  this->EVENT_CONDITION_VARIABLE_MUTEX->lock();
}
void APEX_EVENT_TYPE::UNLOCK_EVENT_MUTEX() {
  this->EVENT_CONDITION_VARIABLE_MUTEX->unlock();
}
}
#endif // ACM_APEX_EVENT_CC