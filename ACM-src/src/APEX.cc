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
#ifndef _APEX_cc
#define _APEX_cc
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

using namespace ACM_USER;
namespace ACM_INTERNAL {

//#include "APEX_BUFFER_SERVICES.cc"
//#include "APEX_QUEUING_SERVICES.cc"
//#include "APEX_SAMPLING_SERVICES.cc"
//#include "APEX_BLACKBOARD_SERVICES.cc"
//#include "APEX_PROCESS_SERVICES.cc"
//#include "APEX_EVENT_SERVICES.cc"
//#include "APEX_SEMAPHORE_SERVICES.cc"
//#include "APEX_HM_SERVICES.cc"
//#include "APEX_TIME_MANAGEMENT_SERVICES.cc"

/** AD: 11/25/2011
   Adding new function addresses for user specified signal handlers. These if
   provided will be invoked before the signal handler code implemented by the
   emulator
*/

SYSTEM_ADDRESS_TYPE APEX::USER_SIGINT_HANDLER = 0;
SYSTEM_ADDRESS_TYPE APEX::USER_SIGTERM_HANDLER = 0;
SYSTEM_ADDRESS_TYPE APEX::USER_PRE_START_HANDLER = 0;

void APEX::REATTACH_SIGNAL_HANDLERS() {
  signal(SIGINT, &APEX::sigIntHandler);
  signal(SIGTERM, &APEX::sigTermHandler);
  signal(SIGUSR2, &APEX::handle_sigusr2_suspend);
}

void APEX::PRINT_CREATED_OBJECT_LIST(/*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE_AND_CREATE_IF_DOES_NOT_EXIST;

  *RETURN_CODE = NO_ERROR;
  if (partition_instance->CURRENT_OPERATING_MODE == IDLE ||
      partition_instance->CURRENT_OPERATING_MODE == NORMAL) {
    LOGGER(ACM_USER::WARNING, "From PRINT_CREATED_OBJECT_LIST: Violated: "
                              "PARTITION STATE== IDLE || NORMAL");

    *RETURN_CODE = INVALID_MODE;
    return;
  }

  //
  // printf("===========START: LIST OF CREATED_OBJECTS IN PARTITION %s: START
  // ==================\n",APEX::PARTITION_NAME.c_str());

  // printf("===========LIST OF PROCESSES ==================\n");
  for (unsigned int count = 0;
       count < partition_instance->PROCESS_DESCRIPTOR_VECTOR.size(); count++) {
    PROCESS *p =
        partition_instance->PROCESS_DESCRIPTOR_VECTOR[count]->THE_PROCESS;
    LOGGER(INFO, "%d. ProcessName: %s,ProcessID: %ld: Period(ns):%lld "
                 "Deadline(ns):%lld : Deadline Type:%s",
           count, p->attribute.NAME, p->process_id, p->attribute.PERIOD,
           p->attribute.TIME_CAPACITY,
           (p->attribute.DEADLINE == SOFT) ? "SOFT DEADLINE" : "HARD DEADLINE");
  }
  //	printf("===========LIST OF RW_LOCK_SEMAPHORE ==================\n");
  for (unsigned int count = 0;
       count < partition_instance->RW_LOCK_SEM_VECTOR.size(); count++) {
    LOGGER(INFO, "%d. RW_LOCK_SEMAPHORE %s", count,
           partition_instance->RW_LOCK_SEM_VECTOR[count]->GET_NAME().c_str());
  }

  // printf("===========LIST OF SEMAPHORE ==================\n");
  for (unsigned int count = 0;
       count < partition_instance->SEMAPHORE_VECTOR.size(); count++) {
    LOGGER(INFO, "%d. SEMAPHORE: %s", count,
           partition_instance->SEMAPHORE_VECTOR[count]->GET_NAME().c_str());
  }

  //	printf("===========LIST OF EVENT ==================\n");
  for (unsigned int count = 0; count < partition_instance->EVENT_VECTOR.size();
       count++) {
    LOGGER(INFO, "%d. EVENT: %s", count,
           partition_instance->EVENT_VECTOR[count]->GET_NAME().c_str());
  }
  // printf("===========LIST OF BLACKBOARD ==================\n");
  for (unsigned int count = 0;
       count < partition_instance->BLACKBOARD_VECTOR.size(); count++) {
    LOGGER(
        INFO, "%d. BLACKBOARD: %s: MAX_MESSAGE_SIZE %ld BYTES", count,
        partition_instance->BLACKBOARD_VECTOR[count]->BLACKBOARD_NAME.c_str(),
        partition_instance->BLACKBOARD_VECTOR[count]->MAX_MESSAGE_SIZE);
  }
  // printf("===========LIST OF BUFFER ==================\n");
  for (unsigned int count = 0; count < partition_instance->BUFFER_VECTOR.size();
       count++) {
    LOGGER(INFO,
           "%d. BUFFER: %s MAX_MESSAGE_SIZE %ld BYTES: MAX_NUM_MESSAGES:%ld",
           count, partition_instance->BUFFER_VECTOR[count]->GET_NAME().c_str(),
           partition_instance->BUFFER_VECTOR[count]->BUFFER_MAX_MESSAGE_SIZE,
           partition_instance->BUFFER_VECTOR[count]->BUFFER_MAX_NB_MESSAGE);
  }

  // printf("===========LIST OF SAMPLING_PORT ==================\n");
  for (unsigned int count = 0;
       count < partition_instance->SAMPLING_PORT_VECTOR.size(); count++) {
    LOGGER(INFO, "%d. SAMPLING_PORT: %s :MAX_MESSAGE_SIZE %ld BYTES", count,
           partition_instance->SAMPLING_PORT_VECTOR[count]->GET_NAME().c_str(),
           partition_instance->SAMPLING_PORT_VECTOR[count]->S_MAX_MESSAGE_SIZE);
  }
  // printf("===========LIST OF QUEUING_PORT ==================\n");
  for (unsigned int count = 0;
       count < partition_instance->QUEUING_PORT_VECTOR.size(); count++) {
    LOGGER(INFO, "%d. QUEUING_PORT: %s: MAX_MESSAGE_SIZE %ld BYTES: "
                 "MAX_NUM_MESSAGES:%ld",
           count,
           partition_instance->QUEUING_PORT_VECTOR[count]->GET_NAME().c_str(),
           partition_instance->QUEUING_PORT_VECTOR[count]->Q_MAX_MESSAGE_SIZE,
           partition_instance->QUEUING_PORT_VECTOR[count]->Q_MAX_NB_MESSAGE);
  }
  // printf("===========END: LIST OF CREATED_OBJECTS IN PARTITION %s
  // ==================\n",APEX::PARTITION_NAME.c_str());
  return;
}

/*======================================	SINGLETON BUSINESS
 * ==========================================================*/
APEX *APEX::_pinstance = 0;
ACM_INTERNAL::MUTEX APEX::singletonInstanceMutex(false,
                                                 ACM_INTERNAL::MUTEX::NORMAL);

APEX *APEX::getInstance() {
  // Get the lock to critical section
  AUTO_LOCK lock(singletonInstanceMutex);
  // check if instance is null
  if (_pinstance == 0) {
    LOGGER(INFO, "Tyring to Create a new APEX");
    // construct a new one
    _pinstance = new APEX();
  }
  assert(_pinstance != 0);
  return _pinstance;
}

/*======================================	//!partition name business
 * ==========================================================*/

std::string APEX::PARTITION_NAME = "";

std::string APEX::GET_PARTITION_NAME() { return APEX::PARTITION_NAME; }
void APEX::SET_PARTITION_NAME(const std::string &name) {
  APEX::PARTITION_NAME = name;
}

bool APEX::checkTerminateStatus() {
  if (APEX::_pinstance == 0) {
    return false;
  } else {
    return APEX::_pinstance->flagToTerminate;
  }
}

/*======================================	//!Constructor and Desctructor
 * ==========================================================*/
APEX::APEX()
    : PROCESS_TABLE_MUTEX(false, MUTEX::ERRORCHECK),
      CURRENT_OPERATING_MODE(ACM_USER::COLD_START), flagToTerminate(false) {

  LOG_INIT();
  // LOGGER(INFO,"INSIDE APEX_CONSTRUCTOR");
  APEX_SCHEDULING_EVENT = APEX_EVENT_TYPE::CREATE_EVENT(
      "APEX_SCHEDULING_EVENT", &(this->PROCESS_TABLE_MUTEX));
  PROCESS_DESCRIPTOR_VECTOR.clear();
  PROCESS_SCHEDULING_SORTED_VECTOR.clear();
  EVENT_VECTOR.clear();
  BLACKBOARD_VECTOR.clear();
  PROCESS_ID_TO_PTHREAD_ID_MAP.clear();
  SEMAPHORE_VECTOR.clear();
  PRIORITY_LEVEL = LOW_LEVEL;

  APEX::getParentInfo(); // get Parent Info first.
  if (!APEX::moduleManagerIsParent) {
    APEX::SET_FIFO_POLICY();
  } else {
    APEX::SET_PARTITION_PRIORITY();
  }

  // LOGGER(INFO,"signal(SIGINT, &APEX::sigIntHandler )");
  signal(SIGINT, &APEX::sigIntHandler);
  signal(SIGTERM, &APEX::sigTermHandler);
  signal(SIGUSR2, &APEX::handle_sigusr2_suspend);
  // Disabling sigsegv handling so that core dump is generated
  // signal(SIGSEGV, &APEX::sigsegvHandler);

  // LOGGER(INFO,"Getting out of APEX_CONSTRUCTOR");
}

void APEX::CLEAR_VECTORS() {
  APEX *partition_instance = APEX::_pinstance;
  if (partition_instance == NULL)
    return;
  // clear events
  for (unsigned int count = 0; count < _pinstance->RW_LOCK_SEM_VECTOR.size();
       count++) {
    delete (_pinstance->RW_LOCK_SEM_VECTOR[count]);
  }
  _pinstance->RW_LOCK_SEM_VECTOR.clear();
  LOGGER(ACM_USER::DEBUG, "_pinstance->RW_LOCK_SEM_VECTOR.clear();");

  for (unsigned int count = 0; count < _pinstance->SEMAPHORE_VECTOR.size();
       count++) {
    delete (_pinstance->SEMAPHORE_VECTOR[count]);
  }
  _pinstance->SEMAPHORE_VECTOR.clear();
  LOGGER(ACM_USER::DEBUG, "_pinstance->SEMAPHORE_VECTOR.clear();");

  for (unsigned int count = 0; count < _pinstance->EVENT_VECTOR.size();
       count++) {
    APEX_EVENT_TYPE *apex_event = _pinstance->EVENT_VECTOR[count];
    delete (apex_event); // this will call the terminate and then wait for
                         // termination;
  }
  LOGGER(ACM_USER::DEBUG, "_pinstance->EVENT_VECTOR.clear();");
  _pinstance->EVENT_VECTOR.clear();

  for (unsigned int count = 0; count < _pinstance->BLACKBOARD_VECTOR.size();
       count++) {
    APEX_BLACKBOARD_TYPE *bboard = _pinstance->BLACKBOARD_VECTOR[count];
    delete (bboard);
  }
  LOGGER(ACM_USER::DEBUG, "_pinstance->BLACKBOARD_VECTOR.clear();");
  _pinstance->BLACKBOARD_VECTOR.clear();

  for (unsigned int count = 0; count < _pinstance->SAMPLING_PORT_VECTOR.size();
       count++) {
    delete (_pinstance->SAMPLING_PORT_VECTOR[count]);
  }
  LOGGER(ACM_USER::DEBUG, "_pinstance->SAMPLING_PORT_VECTOR.clear();");
  _pinstance->SAMPLING_PORT_VECTOR.clear();

  for (unsigned int count = 0; count < _pinstance->QUEUING_PORT_VECTOR.size();
       count++) {
    delete (_pinstance->QUEUING_PORT_VECTOR[count]);
  }
  LOGGER(ACM_USER::DEBUG, "_pinstance->QUEUING_PORT_VECTOR.clear();");
  _pinstance->QUEUING_PORT_VECTOR.clear();

  for (unsigned int count = 0; count < _pinstance->BUFFER_VECTOR.size();
       count++) {
    delete (_pinstance->BUFFER_VECTOR[count]);
  }
  LOGGER(ACM_USER::DEBUG, "_pinstance->BUFFER_VECTOR.clear();");
  _pinstance->BUFFER_VECTOR.clear();
}
//! Destructor
void APEX::TERMINATE() {
  AUTO_LOCK lock(
      singletonInstanceMutex); //<!Cannot create a singleton instance anymore
  if (_pinstance == 0) {
    // LOGGER(ACM_USER::DEBUG,"From APEX Terminate: Nothing to do");
    return;
  }
  //_pinstance->PROCESS_TABLE_MUTEX.unlock();
  _pinstance->PROCESS_TABLE_MUTEX.lock();

  // Now go through process descriptor and stop all processes

  for (unsigned int count = 0;
       count < _pinstance->PROCESS_DESCRIPTOR_VECTOR.size(); count++) {
    PROCESS *p = _pinstance->PROCESS_DESCRIPTOR_VECTOR[count]->THE_PROCESS;
    p->set_priority(WORKER_THREAD_RESTART_PRIORITY);
    delete (_pinstance->PROCESS_DESCRIPTOR_VECTOR[count]); // this will call the
                                                           // terminate and then
                                                           // wait for
                                                           // termination for
                                                           // the process,
                                                           // delete the
                                                           // attribute
    // and then delete the descriptor
  }

  LOGGER(ACM_USER::DEBUG, "_pinstance->PROCESS_DESCRIPTOR_VECTOR.clear();");
  _pinstance->PROCESS_DESCRIPTOR_VECTOR.clear();

  LOGGER(ACM_USER::DEBUG,
         "_pinstance->PROCESS_SCHEDULING_SORTED_VECTOR.clear();");
  _pinstance->PROCESS_SCHEDULING_SORTED_VECTOR.clear();

  APEX::CLEAR_VECTORS();

  LOGGER(ACM_USER::INFO, "Terminate successful;");
  _pinstance->PROCESS_TABLE_MUTEX.unlock();
  delete (_pinstance);
}
APEX::~APEX() {
  LOGGER(ACM_USER::DEBUG, "Deleting APEX_SCHEDULING_EVENT");
  delete (APEX_SCHEDULING_EVENT);
  LOGGER(ACM_USER::DEBUG, "Deleted _pinstance");
  LOG2FILE();
  return;
}

// This code doesnot take into consideration the case when a process could not
// run, because the necessary locks are not available.
// If this portion of the code needs to be added as per the ARINC specs, the
// PROCESS should probably handle this in the set_status method
// upon trying to set the state of process to ready. Setting the status to ready
// should fail and the scheduler code should be updated to
// take this into account.

/*bool APEX::GetNextCriticalTime(int i, long long& rem_ns)//process is in state
- waiting to schedule
{
rem_ns = 0;
PROCESS* process =
partition_instance->PROCESS_DESCRIPTOR_VECTOR[i]->THE_PROCESS;
PROCESS::PROCESS_state status =process->get_state();
if ((status == PROCESS::READY)  || (status == PROCESS::WAITING_ON_PERIOD))
{
rem_ns =
subtractTime2(partition_instance->PROCESS_DESCRIPTOR_VECTOR[i]->ABSOLUTE_NEXT_SCHEDULED_TIME,
actualTime);
char message[100];
LOGGER(message," Computed next launch time = %lld \n",rem_ns);
LOGGER(message);
return true;
}
else if ((status == PROCESS::RUNNING) || (status ==
PROCESS::WAITING_ON_RESOURCE))
{
rem_ns =
subtractTime2(partition_instance->PROCESS_DESCRIPTOR_VECTOR[i]->DEADLINE_TIME,
actualTime);
char message[100];
LOGGER(message," Computed next deadline detection time = %lld \n",rem_ns);
LOGGER(message);
return true;
}

return false;
}*/

void APEX::DoSchedulabilityAnalysis() {
  return;

  // checks if the processes in the partition are schedulable at all
  bool schedulable = true;

  APEX *partition_instance = APEX::_pinstance;
  if (partition_instance == NULL) {
    return;
  }

  // TODO::implement sched analysis

  if (!schedulable) {
    LOGGER(ERROR,
           "Schedulability analysis failed for the given process definitions");
    throw ACM_BASE_EXCEPTION_TYPE(
        "Schedulability analysis failed for the given process definitions");
  }
}

/*======================================	Partition management
 * ==========================================================*/
void APEX::GET_PARTITION_STATUS(
    /*!out*/ PARTITION_STATUS_TYPE *PARTITION_STATUS,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE_AND_CREATE_IF_DOES_NOT_EXIST;

  PARTITION_STATUS->OPERATING_MODE = partition_instance->CURRENT_OPERATING_MODE;
  *RETURN_CODE = NO_ERROR;
}

void APEX::SET_PARTITION_MODE(
    /*!in */ OPERATING_MODE_TYPE OPERATING_MODE,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE_AND_CREATE_IF_DOES_NOT_EXIST;

  // checking state transitions that are not valid
  switch (partition_instance->CURRENT_OPERATING_MODE) {
  case COLD_START:
    if (OPERATING_MODE == WARM_START) {
      *RETURN_CODE = INVALID_MODE;
      return;
    }
    break;
  case IDLE:
    /*if ((OPERATING_MODE==IDLE) ||(OPERATING_MODE==NORMAL))
    {
            *RETURN_CODE = INVALID_MODE;
            return;
    }*/
    //*RETURN_CODE=NO_ERROR;
    // partition_instance->CURRENT_OPERATING_MODE = OPERATING_MODE;
    return;
    break;
  case NORMAL:
    if (OPERATING_MODE == NORMAL) {
      *RETURN_CODE = NO_ACTION;
      return;
    }
    break;
  default:
    break;
  }

  // valid state transition

  *RETURN_CODE = NO_ERROR;

  if (OPERATING_MODE == NORMAL) {
    // Transition to mode - normal, implies all the partition initializations
    // are completed.
    // The partition is initialized for scheduling and then the mode is swithced
    // to normal.
    partition_instance->InitPartitionScheduler(RETURN_CODE);
  } else if (partition_instance->CURRENT_OPERATING_MODE == NORMAL) {
    // The scheduler needs to be stopped.
    APEX::TERMINATE();
  }

  partition_instance->CURRENT_OPERATING_MODE = OPERATING_MODE;
  return;
}

} // ACM_USER

#endif //_APEX_cc
