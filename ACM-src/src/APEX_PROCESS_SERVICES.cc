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
#ifndef APEX_PROCESS_SERVICES
#define APEX_PROCESS_SERVICES
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
using namespace ACM_USER;
namespace ACM_INTERNAL {
/*==================================================================================================================================*/

//! \name Process management services
//@{
/* !
*
*/

void APEX::CREATE_PROCESS(PROCESS_ATTRIBUTE_TYPE *ATTRIBUTES,
                          PROCESS_ID_TYPE *PROCESS_ID,
                          RETURN_CODE_TYPE *RETURN_CODE) {
  // Try and obtain the instance.
  __GET_SINGLETON_INSTANCE_AND_CREATE_IF_DOES_NOT_EXIST;

  // Can Initialize only if the state is COLD_START or WARM_START
  if (partition_instance->CURRENT_OPERATING_MODE == IDLE ||
      partition_instance->CURRENT_OPERATING_MODE == NORMAL) {
    LOGGER(ACM_USER::FATAL, "From CREATE_PROCESS:Trying to create process %s "
                            "while parition is either in IDLE or NORMAL state",
           ATTRIBUTES->NAME);

    *RETURN_CODE = INVALID_MODE;
    return;
  }

  // check if num process less than max process

  if (partition_instance->PROCESS_DESCRIPTOR_VECTOR.size() >=
      MAX_NUMBER_OF_PROCESSES) {

    LOGGER(ACM_USER::FATAL, "From CREATE_PROCESS: Process %s cannot be "
                            "created. MAX_NUMBER_OF_PROCESSES limit exceeded ",
           ATTRIBUTES->NAME);
    *RETURN_CODE = INVALID_CONFIG;
    return;
  }

  // check validity of attributes
  if (ATTRIBUTES->BASE_PRIORITY < PARTITION_PROCESS_LOWPRIORITY ||
      ATTRIBUTES->BASE_PRIORITY > PARTITION_PROCESS_HIGHPRIORITY) {
    LOGGER(ACM_USER::FATAL, "From CREATE_PROCESS: Process %s "
                            "BASE_PRIORITY<PARTITION_PROCESS_LOWPRIORITY || "
                            "ATTRIBUTES->BASE_PRIORITY>PARTITION_PROCESS_"
                            "HIGHPRIORITY",
           ATTRIBUTES->NAME);

    *RETURN_CODE = INVALID_PARAM;
    return;
  }
  if (ATTRIBUTES->ENTRY_POINT == 0 || ATTRIBUTES->ENTRY_POINT == NULL) {
    LOGGER(ACM_USER::FATAL,
           "From CREATE_PROCESS: Process %s  Entry Point is NULL",
           ATTRIBUTES->NAME);

    *RETURN_CODE = INVALID_PARAM;
    return;
  }
  if (ATTRIBUTES->TIME_CAPACITY != INFINITE_TIME_VALUE &&
      ATTRIBUTES->TIME_CAPACITY < MINIMUM_TIME_CAPACITY) {
    LOGGER(ACM_USER::FATAL, "From CREATE_PROCESS: Process %s  TIME_CAPACITY < "
                            "MINIMUM_TIME_CAPACITY, MINIMUM_TIME_CAPACITY=1 "
                            "nanoseconds",
           ATTRIBUTES->NAME);

    *RETURN_CODE = INVALID_PARAM;
    return;
  }
  if (ATTRIBUTES->PERIOD != INFINITE_TIME_VALUE && ATTRIBUTES->PERIOD <= 0) {
    LOGGER(ACM_USER::FATAL, "From CREATE_PROCESS: Process %s  "
                            "ATTRIBUTES->PERIOD!=INFINITE_TIME_VALUE && "
                            "ATTRIBUTES->PERIOD <=0",
           ATTRIBUTES->NAME);

    *RETURN_CODE = INVALID_PARAM;
    return;
  }
  if (ATTRIBUTES->TIME_CAPACITY != INFINITE_TIME_VALUE &&
      ATTRIBUTES->TIME_CAPACITY <= 0) {
    LOGGER(ACM_USER::FATAL, "From CREATE_PROCESS: Process %s  "
                            "ATTRIBUTES->TIME_CAPACITY!=INFINITE_TIME_VALUE && "
                            "ATTRIBUTES->TIME_CAPACITY <=0",
           ATTRIBUTES->NAME);

    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  if (ATTRIBUTES->TIME_CAPACITY == INFINITE_TIME_VALUE &&
      ATTRIBUTES->PERIOD != INFINITE_TIME_VALUE) {
    LOGGER(ACM_USER::FATAL, "From CREATE_PROCESS: Process %s  "
                            "ATTRIBUTES->TIME_CAPACITY==INFINITE_TIME_VALUE && "
                            "ATTRIBUTES->PERIOD !=INFINITE_TIME_VALUE",
           ATTRIBUTES->NAME);

    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  if (ATTRIBUTES->PERIOD != INFINITE_TIME_VALUE &&
      ATTRIBUTES->PERIOD < ATTRIBUTES->TIME_CAPACITY) {

    LOGGER(ACM_USER::FATAL, "From CREATE_PROCESS: Process %s "
                            "ATTRIBUTES->PERIOD!=INFINITE_TIME_VALUE && "
                            "ATTRIBUTES->PERIOD<ATTRIBUTES->TIME_CAPACITY",
           ATTRIBUTES->NAME);

    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  if (ATTRIBUTES->PERIOD != INFINITE_TIME_VALUE &&
      ATTRIBUTES->PERIOD > (SYSTEM_TIME_TYPE)MAX_TIME_OUT) {
    LOGGER(ACM_USER::FATAL, "From CREATE_PROCESS: Process %s "
                            "ATTRIBUTES->PERIOD!=INFINITE_TIME_VALUE&& "
                            "ATTRIBUTES->PERIOD > "
                            "(SYSTEM_TIME_TYPE)MAX_TIME_OUT) ",
           ATTRIBUTES->NAME);
    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  if (ATTRIBUTES->TIME_CAPACITY != INFINITE_TIME_VALUE &&
      ATTRIBUTES->TIME_CAPACITY > (SYSTEM_TIME_TYPE)MAX_TIME_OUT) {
    LOGGER(ACM_USER::FATAL, "From CREATE_PROCESS: Process %s "
                            "ATTRIBUTES->TIME_CAPACITY!=INFINITE_TIME_VALUE&& "
                            "ATTRIBUTES->TIME_CAPACITY > "
                            "(SYSTEM_TIME_TYPE)MAX_TIME_OUT)",
           ATTRIBUTES->NAME);
    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  // Check  that the process Name is Unique

  for (unsigned int i = 0;
       i < partition_instance->PROCESS_DESCRIPTOR_VECTOR.size(); i++) {
    if (partition_instance->PROCESS_DESCRIPTOR_VECTOR[i]
            ->THE_PROCESS->get_name() == std::string(ATTRIBUTES->NAME)) {
      LOGGER(ACM_USER::FATAL, "From CREATE_PROCESS: Name %s is not unique",
             ATTRIBUTES->NAME);

      *RETURN_CODE = INVALID_PARAM;
      return;
    }
  }

  // Attributes have been validated. Now Create Process
  // PROCESS_ATTRIBUTE_TYPE *new_attributes= new
  // PROCESS_ATTRIBUTE_TYPE(ATTRIBUTES);

  APEX_INTEGER new_process_id = partition_instance->PROCESS_DESCRIPTOR_VECTOR
                                    .size(); // This is the process identifier;
  PROCESS *new_process = new PROCESS(*ATTRIBUTES, new_process_id);
  // Store this process in the process Descriptor Maps
  PROCESS_DESCRIPTOR *descriptor =
      new PROCESS_DESCRIPTOR(new_process, ATTRIBUTES->NAME);

  //! create process Notification Blackboard. may be this needs to be only done
  //! for a periodic processes.
  BLACKBOARD_NAME_TYPE BLACKBOARD_NAME;
  snprintf(BLACKBOARD_NAME, MAX_NAME_LENGTH, "RESPONSE_%d",
           (int)new_process_id);

  APEX::CREATE_BLACKBOARD(BLACKBOARD_NAME, MAX_PROCESS_RESPONSE_MESSAGE_SIZE,
                          &(descriptor->PROCESS_NOTIFICATION_RESPONSE_BOARD_ID),
                          RETURN_CODE);

  partition_instance->PROCESS_DESCRIPTOR_VECTOR.push_back(descriptor);
  partition_instance->PROCESS_SCHEDULING_SORTED_VECTOR.push_back(descriptor);

  // Prepare the out parameters
  *PROCESS_ID = new_process_id;
  partition_instance->PROCESS_ID_TO_PTHREAD_ID_MAP[new_process_id] =
      &(new_process->id);

  LOGGER(ACM_USER::TRACE, "CREATE_PROCESS: %s", ATTRIBUTES->NAME);
  return;
} // Finished APEX::CREATE_PROCESS

void APEX::GET_PROCESS_ID(PROCESS_NAME_TYPE PROCESS_NAME,
                          PROCESS_ID_TYPE *PROCESS_ID,
                          RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;

  std::string PROCESS_NAME_STRING(PROCESS_NAME);
  for (unsigned int i = 0;
       i < partition_instance->PROCESS_DESCRIPTOR_VECTOR.size(); i++) {
    if (partition_instance->PROCESS_DESCRIPTOR_VECTOR[i]
            ->THE_PROCESS->get_name() == PROCESS_NAME_STRING) {
      *PROCESS_ID = i;
      *RETURN_CODE = NO_ERROR;
      return;
    }
  }

  LOGGER(ERROR,
         "From GET_PROCESS_ID: Process id %d Does not Exist in Partition",
         *PROCESS_ID);
  *RETURN_CODE = INVALID_CONFIG;
  return;
}
void APEX::GET_MY_ID(
    /*!out*/ PROCESS_ID_TYPE *PROCESS_ID,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {

  __GET_SINGLETON_INSTANCE;

  *RETURN_CODE = NO_ERROR;
  PROCESS_ID_TO_PTHREAD_ID_MAP_TYPE::iterator iter =
      partition_instance->PROCESS_ID_TO_PTHREAD_ID_MAP.begin();
  PROCESS::ThreadID my_id = PROCESS::self();
  for (; iter != partition_instance->PROCESS_ID_TO_PTHREAD_ID_MAP.end();
       iter++) {
    if (*(iter->second) == my_id) {
      *PROCESS_ID = iter->first;
      LOGGER(INFO, "ID of PROCESS NAME %s is %d",
             partition_instance->PROCESS_DESCRIPTOR_VECTOR[*PROCESS_ID]
                 ->THE_PROCESS->get_name()
                 .c_str(),
             *PROCESS_ID);
      break;
    }
  }

  if (iter == partition_instance->PROCESS_ID_TO_PTHREAD_ID_MAP.end())
    *RETURN_CODE = INVALID_MODE;

  return;
}

void APEX::handle_sigusr2_suspend(int sig) {

  PROCESS_NAME_TYPE my_name;
  RETURN_CODE_TYPE return_code;

  APEX::GET_MY_NAME(/*!out*/ &my_name, /*!out*/ &return_code);
  if (return_code != NO_ERROR) {
    LOGGER(ERROR, "Recieved signal handler in process %d", pthread_self());
    return;
  }

  return_code = NO_ERROR;
  LOGGER(TRACE, "Recieved signal handler in process %s", my_name);
  APEX::SUSPEND_SELF(INFINITE_TIME_VALUE, &return_code);
  LOGGER(TRACE, "out of suspend signal handler in process %s", my_name);
}

void APEX::SUSPEND(
    /*!in */ PROCESS_ID_TYPE PROCESS_ID,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {

  __GET_SINGLETON_INSTANCE;
  *RETURN_CODE = NO_ERROR;
  if (partition_instance->PROCESS_DESCRIPTOR_VECTOR.size() <= PROCESS_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  } // invalid process id

  PROCESS *process =
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->THE_PROCESS;
  if (process->get_state() == PROCESS::DORMANT) {
    *RETURN_CODE = INVALID_MODE;
    return;
  } // process priority can not be changed in dormant mode

  // check if process is periodic

  if (process->isPeriodic()) {
    LOGGER(ERROR, "Trying to Suspend a periodic process");
    *RETURN_CODE = INVALID_MODE;
    return;
  }

  // check current priority
  PROCESS::PROCESS_state pstate = process->get_state();
  if (pstate == PROCESS::WAITING_SUSPENDED_OTHER ||
      pstate == PROCESS::WAITING_SUSPENDED_SELF) {
    LOGGER(ACM_USER::WARNING, "Trying to Suspend an already suspended process");
    *RETURN_CODE = NO_ACTION;
    return;
  }

  int retval = pthread_kill(process->thread_id(), SIGUSR2);
  if (retval == 0) {
    LOGGER(INFO, "SUSPEND messae sent successfully to %s. ",
           process->attribute.NAME);
  } else {
    if (retval == ESRCH) {
      LOGGER(ERROR, "SUSPEND Error  on %s. No thread could be found "
                    "corresponding to that specified by the given thread ID.",
             process->attribute.NAME);
    } else if (retval == EINVAL) {
      LOGGER(ERROR, "SUSPEND Error on %s. The value of the sig argument is an "
                    "invalid or unsupported signal number.",
             process->attribute.NAME);
    } else
      LOGGER(ERROR, "SUSPEND Error %d on %s. ", retval,
             process->attribute.NAME);
  }
  /* process->set_priority(SUSPEND_PROCESS_PRIORITY);

  process->set_state(PROCESS::WAITING_SUSPENDED_OTHER);
  *RETURN_CODE=NO_ERROR;

  */

  // LOGGER(INFO,"SUSPEND SUCCESSFUL on %s. Priority set to
  // %ld",process->attribute.NAME,process->get_priority());

  return;
}

void APEX::SUSPEND_SELF(
    /*!in */ SYSTEM_TIME_TYPE TIME_OUT,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  //			SYSTEM_TIMESPEC actualtime=currentTime();
  __GET_SINGLETON_INSTANCE;

  *RETURN_CODE = NO_ERROR;

  if (TIME_OUT != INFINITE_TIME_VALUE &&
      TIME_OUT > (SYSTEM_TIME_TYPE)MAX_TIME_OUT) {
    LOGGER(ERROR, "TIME_OUT %lld > (SYSTEM_TIME_TYPE)MAX_TIME_OUT)", TIME_OUT);
    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  PROCESS_ID_TYPE MY_APEX_PROCESS_ID;

  APEX::GET_MY_ID(&MY_APEX_PROCESS_ID, RETURN_CODE);
  if (*RETURN_CODE != NO_ERROR) {
    return;
  }

  PROCESS *process =
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[MY_APEX_PROCESS_ID]
          ->THE_PROCESS;
  if (process->isPeriodic()) {
    LOGGER(ERROR, "Trying to Suspend a periodic process");
    *RETURN_CODE = INVALID_MODE;
    return;
  }

  if (process->isPeriodic()) {
    LOGGER(ERROR, "Trying to Suspend a periodic process");
    *RETURN_CODE = INVALID_MODE;
    return;
  }

  PROCESS::PROCESS_state pstate = process->get_state();
  if (pstate == PROCESS::WAITING_SUSPENDED_OTHER ||
      pstate == PROCESS::WAITING_SUSPENDED_SELF) {
    LOGGER(ACM_USER::WARNING, "Trying to Suspend an already suspended process");
    *RETURN_CODE = NO_ACTION;
    return;
  }
  PROCESS::PROCESS_state state = process->get_state();
  process->set_state(PROCESS::WAITING_SUSPENDED_SELF);
  // SYSTEM_TIMESPEC absolute_time=addTimeNano(actualtime,TIME_OUT);
  partition_instance->PROCESS_DESCRIPTOR_VECTOR[MY_APEX_PROCESS_ID]
      ->SUSPENSION_EVENT.WAIT_EVENT(TIME_OUT, RETURN_CODE);
  process->set_state(state);
  RETURN_CODE_TYPE another_code;
  partition_instance->PROCESS_DESCRIPTOR_VECTOR[MY_APEX_PROCESS_ID]
      ->SUSPENSION_EVENT.RESET_EVENT(&another_code);

  // APEX::SUSPEND(MY_APEX_PROCESS_ID,RETURN_CODE);

  // CURRENTLY SUSPEND_SELF is implemented using suspend

  // for (PROCESS_ID_TO_PTHREAD_ID_MAP_TYPE::iterator iter =)

  return;

  // schedRetValue = pthread_setschedparam(PROCESS::self(), ACM_SCHED_POLICY,
  // &sp_Thread);
}

void APEX::RESUME(
    /*!in */ PROCESS_ID_TYPE PROCESS_ID,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;
  *RETURN_CODE = NO_ERROR;
  if (partition_instance->PROCESS_DESCRIPTOR_VECTOR.size() <= PROCESS_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  } // invalid process id

  PROCESS *process =
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->THE_PROCESS;
  if (process->get_state() == PROCESS::DORMANT) {
    *RETURN_CODE = INVALID_MODE;
    return;
  } // process priority can not be changed in dormant mode

  // check if process is periodic

  if (process->isPeriodic()) {
    LOGGER(ERROR, "Trying to resume a periodic process");
    *RETURN_CODE = INVALID_MODE;
    return;
  }
  PROCESS::PROCESS_state pstate = process->get_state();

  if (pstate != PROCESS::WAITING_SUSPENDED_OTHER &&
      pstate != PROCESS::WAITING_SUSPENDED_SELF) {
    LOGGER(WARNING, "Trying to resume a process that is not suspended");
    *RETURN_CODE = NO_ACTION;
    return;
    //
  }

  if (pstate == PROCESS::WAITING_SUSPENDED_OTHER) {
    process->set_priority(
        partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
            ->CURRENT_PRIORITY);
    process->set_state(PROCESS::RUNNING);
    *RETURN_CODE = NO_ERROR;
    return;
  }

  if (pstate == PROCESS::WAITING_SUSPENDED_SELF) {
    *RETURN_CODE = NO_ERROR;
    partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
        ->SUSPENSION_EVENT.SET_EVENT(RETURN_CODE);
  }
}

void APEX::GET_MY_NAME(/*!out*/ PROCESS_NAME_TYPE *PROCESS_NAME,
                       /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;
  PROCESS_ID_TYPE PROCESS_ID;
  APEX::GET_MY_ID(&PROCESS_ID, RETURN_CODE);
  if (*RETURN_CODE != NO_ERROR)
    return;

  // std::cout<<partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->THE_PROCESS->attribute.NAME<<std::endl;

  strcpy(&(*PROCESS_NAME)[0],
         &(partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
               ->THE_PROCESS->attribute.NAME[0]));
  *RETURN_CODE = NO_ERROR;
  return;
}

void APEX::GET_PROCESS_NAME(/*!in */ PROCESS_ID_TYPE PROCESS_ID,
                            /*!out*/ PROCESS_NAME_TYPE *PROCESS_NAME,
                            /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;
  // check inputs
  if (partition_instance->PROCESS_DESCRIPTOR_VECTOR.size() <= PROCESS_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  } // invalid process id

  strcpy(&(*PROCESS_NAME)[0],
         &(partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
               ->THE_PROCESS->attribute.NAME[0]));
  *RETURN_CODE = NO_ERROR;
  return;
}

void APEX::GET_PROCESS_STATUS(
    /*!in */ PROCESS_ID_TYPE PROCESS_ID,
    /*!out*/ PROCESS_STATUS_TYPE *PROCESS_STATUS,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;

  // check inputs
  if (partition_instance->PROCESS_DESCRIPTOR_VECTOR.size() <= PROCESS_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  } // invalid process id

  PROCESS::PROCESS_state state =
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
          ->THE_PROCESS->get_state();

  switch (state) {
  case PROCESS::DORMANT:
    PROCESS_STATUS->PROCESS_STATE = DORMANT;
    break;
  case PROCESS::READY:
    PROCESS_STATUS->PROCESS_STATE = READY;
    break;

  case PROCESS::RUNNING:
    PROCESS_STATUS->PROCESS_STATE = RUNNING;
    break;
  case PROCESS::WAITING_ON_RESOURCE:
  case PROCESS::WAITING_ON_PERIOD:
  case PROCESS::WAITING_SUSPENDED_OTHER:
  case PROCESS::WAITING_SUSPENDED_SELF:
  case PROCESS::WAITING_PARTITION_NOT_NORMAL:
    PROCESS_STATUS->PROCESS_STATE = WAITING;
    break;
  // Question - What should be done when the process is in the restarting state?

  default:
    PROCESS_STATUS->PROCESS_STATE = DORMANT;
    break;
  }

  PROCESS_STATUS->CURRENT_PRIORITY =
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
          ->THE_PROCESS->get_priority();
  PROCESS_STATUS->ATTRIBUTES =
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
          ->THE_PROCESS->attribute;

  *RETURN_CODE = NO_ERROR;
}

void APEX::SET_PRIORITY(
    /*!in */ PROCESS_ID_TYPE PROCESS_ID,
    /*!in */ PRIORITY_TYPE PRIORITY,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;

  if (partition_instance->PROCESS_DESCRIPTOR_VECTOR.size() <= PROCESS_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  } // invalid process id

  if (PRIORITY < PARTITION_PROCESS_LOWPRIORITY ||
      PRIORITY > PARTITION_PROCESS_HIGHPRIORITY) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  } // invalid priority

  PROCESS *process =
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->THE_PROCESS;
  if (process->get_state() == PROCESS::DORMANT) {
    *RETURN_CODE = INVALID_MODE;
    return;
  } // process priority can not be changed in dormant mode

  process->set_priority(PRIORITY); // set the process priority
  partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->CURRENT_PRIORITY =
      PRIORITY;

  *RETURN_CODE = NO_ERROR;
}

void APEX::NOTIFY_PROCESS_FINISHED_STATUS(
    /*!in */ PROCESS_ID_TYPE PROCESS_ID,
    /*!out*/ PROCESS_RESPONSE_TYPE *PROCESS_RESPONSE,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  __GET_SINGLETON_INSTANCE;
  if (partition_instance == NULL) {
    *RETURN_CODE = INVALID_CONFIG;
    return;
  }

  if (partition_instance->PROCESS_DESCRIPTOR_VECTOR.size() <= PROCESS_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  } // invalid process id

  PROCESS_RESPONSE->TIMEOFMESSAGE = ACM_USER::currentTimeInNanoSeconds();

  APEX::DISPLAY_BLACKBOARD(
      /*in */ partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
          ->PROCESS_NOTIFICATION_RESPONSE_BOARD_ID,
      /*in */ (MESSAGE_ADDR_TYPE)PROCESS_RESPONSE, /* by reference */
      /*in */ (MESSAGE_SIZE_TYPE)sizeof(PROCESS_RESPONSE_TYPE),
      /*out*/ RETURN_CODE);

  return;
}

void APEX::GET_PROCESS_NOTIFICATION_BOARD_ID(
    /*!in */ PROCESS_ID_TYPE PROCESS_ID,
    /*!out*/ BLACKBOARD_ID_TYPE *TARGET_BOARD_ID,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;

  if (partition_instance->PROCESS_DESCRIPTOR_VECTOR.size() <= PROCESS_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  } // invalid process id

  *TARGET_BOARD_ID = partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
                         ->PROCESS_NOTIFICATION_RESPONSE_BOARD_ID;
}

void APEX::GET_PROCESS_FINISHED_STATUS(
    /*!in */ PROCESS_ID_TYPE PROCESS_ID,
    /*!in */ SYSTEM_TIME_TYPE TIME_OUT,
    /*!out*/ MESSAGE_ADDR_TYPE PROCESS_RESPONSE,
    /*!in */ MESSAGE_SIZE_TYPE PROCESS_RESPONSE_CAPACITY,
    /*!out*/ MESSAGE_SIZE_TYPE *PROCESS_RESPONSE_LENGTH,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  __GET_SINGLETON_INSTANCE;

  if (partition_instance->PROCESS_DESCRIPTOR_VECTOR.size() <= PROCESS_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  } // invalid process id

  BLACKBOARD_ID_TYPE TARGET_BOARD_ID =
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
          ->PROCESS_NOTIFICATION_RESPONSE_BOARD_ID;
  if (PROCESS_RESPONSE_CAPACITY >
      (MESSAGE_SIZE_TYPE)MAX_PROCESS_RESPONSE_MESSAGE_SIZE) {
    LOGGER(WARNING, "PROCESS_RESPONSE_CAPACITY>(MESSAGE_SIZE_TYPE)MAX_PROCESS_"
                    "RESPONSE_MESSAGE_SIZE");
    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  APEX::READ_BLACKBOARD(TARGET_BOARD_ID, TIME_OUT, PROCESS_RESPONSE,
                        PROCESS_RESPONSE_LENGTH, RETURN_CODE);
  if (*RETURN_CODE == TIMED_OUT)
    *PROCESS_RESPONSE_LENGTH = 0;
  return;
}

void APEX::START(
    /*!in */ PROCESS_ID_TYPE PROCESS_ID,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  APEX::DELAYED_START(PROCESS_ID, 0,
                      RETURN_CODE); // invoke delayed_start with a delay of 0
  //! two processes should not be allowed to call delayed start together.
}

/*
when (the process is periodic and DELAY_TIME is greater or equal to the period
of the specified process) =>
RETURN_CODE := INVALID_PARAM;
*/

void APEX::DELAYED_START(
    /*!in */ PROCESS_ID_TYPE PROCESS_ID,
    /*!in */ SYSTEM_TIME_TYPE DELAY_TIME,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  // The Partition should be created at this time. If it is not a suitable error
  // should be indicated in the return_code
  __GET_SINGLETON_INSTANCE;

  // Will prevent the possible race condition where two threads can start the
  // same process.
  static ACM_INTERNAL::MUTEX delayed_start_Mutex;
  ACM_INTERNAL::AUTO_LOCK t_lock(delayed_start_Mutex);

  if (DELAY_TIME < 0) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  // check inputs
  if (partition_instance->PROCESS_DESCRIPTOR_VECTOR.size() <= PROCESS_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  } // invalid process id

  PROCESS *process =
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->THE_PROCESS;
  if (process->get_state() != PROCESS::DORMANT) {
    *RETURN_CODE = NO_ACTION;
    return;
  } // process can be started only if it is in dormant state

  /*if(process->isPeriodic())
  {
          // check that the delayed start value is less than period
          if( DELAY_TIME>=process->attribute.PERIOD)
          {
                  *RETURN_CODE=INVALID_PARAM;
                  return;
          }

  }*/

  process->set_priority(
      process->attribute.BASE_PRIORITY); // set priority to base priority
  partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->CURRENT_PRIORITY =
      process->attribute.BASE_PRIORITY;

  if (partition_instance->CURRENT_OPERATING_MODE ==
      NORMAL) // partition in nomal operating mode
  {
    if (process->isPeriodic()) // set process state based on the nature of the
                               // process
      process->set_state(PROCESS::WAITING_ON_PERIOD); // set process state to
                                                      // waiting on period for
                                                      // periodic process.
    else {
      // clear the notification black board
      APEX::CLEAR_BLACKBOARD(
          partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
              ->PROCESS_NOTIFICATION_RESPONSE_BOARD_ID,
          RETURN_CODE);
      process->set_state(
          PROCESS::READY); // set process state to ready for aperiodic process.
    }

    partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
        ->ABSOLUTE_NEXT_SCHEDULED_TIME =
        addTimeNano(currentTime(), DELAY_TIME); // compute next release point
    SYSTEM_TIME_TYPE t = process->attribute.TIME_CAPACITY;
    partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->DEADLINE_TIME =
        addTimeNano(partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
                        ->ABSOLUTE_NEXT_SCHEDULED_TIME,
                    t); // compute deadline
    // LOGGER(INFO,"In Delayed start. Before Locking, the process table mutex
    // status is %ld",
    // partition_instance->PROCESS_TABLE_MUTEX.try_lock_without_locking());
    partition_instance->PROCESS_TABLE_MUTEX.lock();
    // LOGGER(INFO,"In Delayed start. Locked the Mutex, the process table mutex
    // status is %ld",
    // partition_instance->PROCESS_TABLE_MUTEX.try_lock_without_locking());
    partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
        ->ABSOLUTE_CRITICAL_TIME =
        partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
            ->ABSOLUTE_NEXT_SCHEDULED_TIME;
    // you want to to run it set the descriptor to release

    partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
        ->NEXT_SCHEDULING_INSTANCE = PROCESS_DESCRIPTOR::RELEASE_PROCESS;

    RETURN_CODE_TYPE return_code;
    // partition_instance->PROCESS_TABLE_MUTEX.unlock();
    // Will go in the set event with lock.
    partition_instance->APEX_SCHEDULING_EVENT->SET_EVENT(
        &return_code); // causing a release
    // Will be unlocked here.
    // LOGGER(INFO,"In Delayed start, the process table mutex status is %ld",
    // partition_instance->PROCESS_TABLE_MUTEX.try_lock());
    assert(return_code == NO_ERROR);
  } else // partition not in normal mode
  {
    process->set_state(
        PROCESS::WAITING_PARTITION_NOT_NORMAL); // set the process to
                                                // waiting_to_schedule state
    partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
        ->INITIAL_DELAY_TIME = DELAY_TIME;
  }

  *RETURN_CODE = NO_ERROR; // no error
}

void APEX::STOP_SELF() {
  PROCESS_ID_TYPE PROCESS_ID;
  RETURN_CODE_TYPE RETURN_CODE;

  APEX *partition_instance = APEX::_pinstance;
  assert(partition_instance == NULL);

  APEX::GET_MY_ID(&PROCESS_ID, &RETURN_CODE);
  if (RETURN_CODE != NO_ERROR) {
    return;
  }

  PROCESS *process =
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->THE_PROCESS;
  if (process->get_state() == PROCESS::DORMANT) {
    RETURN_CODE = NO_ACTION;
    return;
  } else {

    // Nag - Probably all that needs to be done here is to call stop on all
    // processes and set their state to dormant.
    // The process should not be restarted.

    // process->stop(false); //will not cause a restart.
    // Abhishek: Changing nag's code. We need to call restart as this will
    // ensure that the state of this process goes to dormant.

    process->set_priority(WORKER_THREAD_RESTART_PRIORITY);
    process->stop(true); // this will cause a restart. This is the only way to
                         // ensure that this thread quits what it was
    // curently doing and go to the dormant state.
    process->set_priority(process->attribute.BASE_PRIORITY);
    RETURN_CODE = NO_ERROR;
  }
}
void APEX::STOP(/*!in */ PROCESS_ID_TYPE PROCESS_ID,
                /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {

  __GET_SINGLETON_INSTANCE;

  // check inputs
  if (partition_instance->PROCESS_DESCRIPTOR_VECTOR.size() <= PROCESS_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  } // invalid process id

  PROCESS *process =
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->THE_PROCESS;
  if (process->get_state() == PROCESS::DORMANT) {
    *RETURN_CODE = NO_ACTION;
    return;
  } else {

    // Nag - Probably all that needs to be done here is to call stop on all
    // processes and set their state to dormant.
    // The process should not be restarted.

    // process->stop(false); //will not cause a restart.
    // Abhishek: Changing nag's code. We need to call restart as this will
    // ensure that the state of this process goes to dormant.

    process->set_priority(WORKER_THREAD_RESTART_PRIORITY);
    process->stop(true); // this will cause a restart. This is the only way to
                         // ensure that this thread quits what it was
    // curently doing and go to the dormant state.
    process->set_priority(process->attribute.BASE_PRIORITY);

    *RETURN_CODE = NO_ERROR;
  }
}
}
#endif
