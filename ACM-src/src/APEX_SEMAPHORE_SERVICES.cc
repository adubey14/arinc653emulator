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
#ifndef APEX_SEMAPHORE_SERVICES
#define APEX_SEMAPHORE_SERVICES

#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
using namespace ACM_USER;
namespace ACM_INTERNAL {
/************************** RW SEMAPHORE**************************************/
//!\name RW_LOCK_SEMAPHORE management services
void APEX::CREATE_RW_SEMAPHORE(
    /*!in */ SEMAPHORE_NAME_TYPE SEMAPHORE_NAME,
    /*!in */ SEMAPHORE_VALUE_TYPE MAXIMUM_READERS,
    /*!out*/ SEMAPHORE_ID_TYPE *SEMAPHORE_ID,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  // Try and obtain the instance.
  __GET_SINGLETON_INSTANCE_AND_CREATE_IF_DOES_NOT_EXIST;
  if (partition_instance->CURRENT_OPERATING_MODE == IDLE ||
      partition_instance->CURRENT_OPERATING_MODE == NORMAL) {

    LOGGER(ERROR, "From CREATE_RW_SEMAPHORE: Trying to create process %s while "
                  "parition is either in IDLE or NORMAL state",
           SEMAPHORE_NAME);

    *RETURN_CODE = INVALID_MODE;
    return;
  }

  // Ensure that the SEMAPHORE_NAME_STRING is unique
  std::string SEMAPHORE_NAME_STRING(SEMAPHORE_NAME);
  for (unsigned int i = 0; i < partition_instance->RW_LOCK_SEM_VECTOR.size();
       i++) {
    if (partition_instance->RW_LOCK_SEM_VECTOR[i]->GET_NAME() ==
        SEMAPHORE_NAME_STRING) {

      LOGGER(ERROR, "From CREATE_RW_SEMAPHORE: NAME %s is not unique",
             SEMAPHORE_NAME);

      *RETURN_CODE = INVALID_PARAM;
      return;
    }
  }

  // Parameters are valid
  RW_LOCK_SEM *semaphore = new RW_LOCK_SEM(SEMAPHORE_NAME, MAXIMUM_READERS);
  *SEMAPHORE_ID = partition_instance->RW_LOCK_SEM_VECTOR.size();
  partition_instance->RW_LOCK_SEM_VECTOR.push_back(semaphore);
  *RETURN_CODE = NO_ERROR;
}

void APEX::GET_RW_SEMAPHORE_ID(
    /*!in */ SEMAPHORE_NAME_TYPE SEMAPHORE_NAME,
    /*!out*/ SEMAPHORE_ID_TYPE *SEMAPHORE_ID,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;

  *RETURN_CODE = INVALID_PARAM;
  std::string SEMAPHORE_NAME_STRING(SEMAPHORE_NAME);
  for (unsigned int i = 0; i < partition_instance->RW_LOCK_SEM_VECTOR.size();
       i++) {
    if (partition_instance->RW_LOCK_SEM_VECTOR[i]->GET_NAME() ==
        SEMAPHORE_NAME_STRING) {
      *SEMAPHORE_ID = i;
      *RETURN_CODE = NO_ERROR;
      return;
    }
  }
  return;
}

//! Currently ignore Time_Out
void APEX::READLOCK_RW_SEMAPHORE(
    /*!in */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,
    /*!in */ SYSTEM_TIME_TYPE TIME_OUT,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  // Try and obtain the instance.
  __GET_SINGLETON_INSTANCE;
  *RETURN_CODE = NO_ERROR;

  // check inputs
  if (partition_instance->RW_LOCK_SEM_VECTOR.size() <= SEMAPHORE_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  }
  partition_instance->RW_LOCK_SEM_VECTOR[SEMAPHORE_ID]->readlock(TIME_OUT,
                                                                 RETURN_CODE);

  PROCESS_ID_TYPE PROCESS_ID = 0;
  RETURN_CODE_TYPE RETURN_CODE_PROCESS_ID;
  APEX::GET_MY_ID(&PROCESS_ID, &RETURN_CODE_PROCESS_ID);
  if (RETURN_CODE_PROCESS_ID != NO_ERROR)
    return;

  // add to the process readlock descriptor..

  if (*RETURN_CODE == NO_ERROR) {
    // LOGGER(APP,"partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->THE_PROCESS->READLOCKS.push_back(SEMAPHORE_ID);");
    partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
        ->THE_PROCESS->READLOCKS.push_back(SEMAPHORE_ID);
  }

  // timeout is ignored
}

void APEX::READUNLOCK_RW_SEMAPHORE(
    /*!in */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  // Try and obtain the instance.
  __GET_SINGLETON_INSTANCE;
  *RETURN_CODE = NO_ERROR;

  // check inputs
  if (partition_instance->RW_LOCK_SEM_VECTOR.size() <= SEMAPHORE_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  }
  partition_instance->RW_LOCK_SEM_VECTOR[SEMAPHORE_ID]->readunlock();

  PROCESS_ID_TYPE PROCESS_ID = 0;
  RETURN_CODE_TYPE RETURN_CODE_PROCESS_ID;
  APEX::GET_MY_ID(&PROCESS_ID, &RETURN_CODE_PROCESS_ID);
  if (RETURN_CODE_PROCESS_ID != NO_ERROR)
    return;

  for (std::list<SEMAPHORE_ID_TYPE>::iterator iter =
           partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
               ->THE_PROCESS->READLOCKS.begin();
       iter !=
       partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
           ->THE_PROCESS->READLOCKS.end();
       iter++) {
    // LOGGER(HMEVENT,"PROCESS %s stopped. But it still holds writelock to
    // sempahore id %ld",current->attribute.NAME,*iter);
    if (*iter == SEMAPHORE_ID) {
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
          ->THE_PROCESS->READLOCKS.erase(iter);
      break;
    }
  }

  // timeout is ignored
}

void APEX::WRITELOCK_RW_SEMAPHORE(
    /*!in */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,
    /*!in */ SYSTEM_TIME_TYPE TIME_OUT,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  // Try and obtain the instance.
  __GET_SINGLETON_INSTANCE;
  *RETURN_CODE = NO_ERROR;

  // check inputs
  if (partition_instance->RW_LOCK_SEM_VECTOR.size() <= SEMAPHORE_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  }
  partition_instance->RW_LOCK_SEM_VECTOR[SEMAPHORE_ID]->writelock(TIME_OUT,
                                                                  RETURN_CODE);

  PROCESS_ID_TYPE PROCESS_ID = 0;
  RETURN_CODE_TYPE RETURN_CODE_PROCESS_ID;
  APEX::GET_MY_ID(&PROCESS_ID, &RETURN_CODE_PROCESS_ID);
  if (RETURN_CODE_PROCESS_ID != NO_ERROR)
    return;

  if (*RETURN_CODE == NO_ERROR)
    partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
        ->THE_PROCESS->WRITELOCKS.push_back(SEMAPHORE_ID);

  // timeout is ignored
}

void APEX::WRITEUNLOCK_RW_SEMAPHORE(
    /*!in */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,

    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE)

{
  // Try and obtain the instance.
  __GET_SINGLETON_INSTANCE;
  *RETURN_CODE = NO_ERROR;

  // check inputs
  if (partition_instance->RW_LOCK_SEM_VECTOR.size() <= SEMAPHORE_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  }
  partition_instance->RW_LOCK_SEM_VECTOR[SEMAPHORE_ID]->writeunlock();

  PROCESS_ID_TYPE PROCESS_ID = 0;
  RETURN_CODE_TYPE RETURN_CODE_PROCESS_ID;
  APEX::GET_MY_ID(&PROCESS_ID, &RETURN_CODE_PROCESS_ID);
  if (RETURN_CODE_PROCESS_ID != NO_ERROR)
    return;

  for (std::list<SEMAPHORE_ID_TYPE>::iterator iter =
           partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
               ->THE_PROCESS->WRITELOCKS.begin();
       iter !=
       partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
           ->THE_PROCESS->WRITELOCKS.end();
       iter++) {
    // LOGGER(HMEVENT,"PROCESS %s stopped. But it still holds writelock to
    // sempahore id %ld",current->attribute.NAME,*iter);
    if (*iter == SEMAPHORE_ID) {
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
          ->THE_PROCESS->WRITELOCKS.erase(iter);
      break;
    }
  }

  // timeout is ignored
}

/*===================================Semaphore
 * Services===============================================================*/

void APEX::CREATE_SEMAPHORE(
    /*!in */ SEMAPHORE_NAME_TYPE SEMAPHORE_NAME,
    /*!in */ SEMAPHORE_VALUE_TYPE CURRENT_VALUE,
    /*!in */ SEMAPHORE_VALUE_TYPE MAXIMUM_VALUE,
    /*!in */ QUEUING_DISCIPLINE_TYPE QUEUING_DISCIPLINE,
    /*!out*/ SEMAPHORE_ID_TYPE *SEMAPHORE_ID,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  // Try and obtain the instance.
  __GET_SINGLETON_INSTANCE_AND_CREATE_IF_DOES_NOT_EXIST;

  // Only one thread should be in create_process at a time
  // AUTO_LOCK lock (partition_instance->PROCESS_TABLE_MUTEX);

  // Can Initialize only if the state is COLD_START or WARM_START
  if (partition_instance->CURRENT_OPERATING_MODE == IDLE ||
      partition_instance->CURRENT_OPERATING_MODE == NORMAL) {

    LOGGER(ERROR, "From CREATE_SEMAPHORE: Trying to create process %s while "
                  "parition is either in IDLE or NORMAL state",
           SEMAPHORE_NAME);

    *RETURN_CODE = INVALID_MODE;
    return;
  }

  // Ensure that the event_name is unique
  std::string SEMAPHORE_NAME_STRING(SEMAPHORE_NAME);
  for (unsigned int i = 0; i < partition_instance->SEMAPHORE_VECTOR.size();
       i++) {
    if (partition_instance->SEMAPHORE_VECTOR[i]->GET_NAME() ==
        SEMAPHORE_NAME_STRING) {

      LOGGER(ERROR, "From CREATE_SEMAPHORE: NAME %s is not unique",
             SEMAPHORE_NAME);

      *RETURN_CODE = INVALID_PARAM;
      return;
    }
  }

  // Parameters are valid
  SEMAPHORE *semaphore =
      new SEMAPHORE(SEMAPHORE_NAME, CURRENT_VALUE, MAXIMUM_VALUE);
  *SEMAPHORE_ID = partition_instance->SEMAPHORE_VECTOR.size();
  partition_instance->SEMAPHORE_VECTOR.push_back(semaphore);
  *RETURN_CODE = NO_ERROR;
}

void APEX::WAIT_SEMAPHORE(SEMAPHORE_ID_TYPE SEMAPHORE_ID,
                          SYSTEM_TIME_TYPE TIME_OUT,
                          RETURN_CODE_TYPE *RETURN_CODE) {
  // Try and obtain the instance.
  __GET_SINGLETON_INSTANCE;

  // check inputs
  if (partition_instance->SEMAPHORE_VECTOR.size() <= SEMAPHORE_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  }
  *RETURN_CODE =
      partition_instance->SEMAPHORE_VECTOR[SEMAPHORE_ID]->wait(TIME_OUT);
}
void APEX::SIGNAL_SEMAPHORE(SEMAPHORE_ID_TYPE SEMAPHORE_ID,
                            RETURN_CODE_TYPE *RETURN_CODE) {
  // Try and obtain the instance.
  // if(APEX::_pinstance==0) APEX::getInstance();
  __GET_SINGLETON_INSTANCE;

  // check inputs
  if (partition_instance->SEMAPHORE_VECTOR.size() <= SEMAPHORE_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  }
  *RETURN_CODE = partition_instance->SEMAPHORE_VECTOR[SEMAPHORE_ID]->post();
}

void APEX::GET_SEMAPHORE_ID(SEMAPHORE_NAME_TYPE SEMAPHORE_NAME,
                            SEMAPHORE_ID_TYPE *SEMAPHORE_ID,
                            RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;

  *RETURN_CODE = INVALID_PARAM;
  std::string SEMAPHORE_NAME_STRING(SEMAPHORE_NAME);
  for (unsigned int i = 0; i < partition_instance->SEMAPHORE_VECTOR.size();
       i++) {
    if (partition_instance->SEMAPHORE_VECTOR[i]->GET_NAME() ==
        SEMAPHORE_NAME_STRING) {
      *SEMAPHORE_ID = i;
      *RETURN_CODE = NO_ERROR;
      return;
    }
  }
  return;
}
void APEX::GET_SEMAPHORE_STATUS(
    /*!in */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,
    /*!out*/ SEMAPHORE_STATUS_TYPE *SEMAPHORE_STATUS,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;

  // check inputs
  if (partition_instance->SEMAPHORE_VECTOR.size() <= SEMAPHORE_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  } else {
    SEMAPHORE_STATUS->WAITING_PROCESSES =
        partition_instance->SEMAPHORE_VECTOR[SEMAPHORE_ID]->GET_WAITING_COUNT();
    SEMAPHORE_STATUS->CURRENT_VALUE =
        partition_instance->SEMAPHORE_VECTOR[SEMAPHORE_ID]
            ->GET_SEMAPHORE_COUNT();
    SEMAPHORE_STATUS->MAXIMUM_VALUE =
        partition_instance->SEMAPHORE_VECTOR[SEMAPHORE_ID]->GET_MAXIMUM_VALUE();
    *RETURN_CODE = NO_ERROR;

    return;
  }
}
}
#endif