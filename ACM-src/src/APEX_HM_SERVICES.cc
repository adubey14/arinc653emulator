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
#ifndef APEX_HM_SERVICES
#define APEX_HM_SERVICES
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
using namespace ACM_USER;
namespace ACM_INTERNAL {
/////////////////////////// ERROR RELATED ////////////////////////////////////

void APEX::RAISE_APPLICATION_ERROR_AND_GET_RESPONSE(
    /*!in */ ERROR_CODE_TYPE ERROR_CODE,
    /*!in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
    /*!in */ ERROR_MESSAGE_SIZE_TYPE LENGTH,
    /*!in */ SYSTEM_TIME_TYPE
        TIME_OUT, // Added Timeout in response to the discussion with Gabor
    /*!out*/ HM_RESPONSE_TYPE *HM_RESPONSE,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  MESSAGE_SIZE_TYPE HM_RESPONSE_LENGTH;
  APEX::RAISE_APPLICATION_ERROR_AND_GET_RESPONSE(
      ERROR_CODE, MESSAGE_ADDR, LENGTH, (MESSAGE_ADDR_TYPE)HM_RESPONSE,
      sizeof(HM_RESPONSE_TYPE), TIME_OUT, &HM_RESPONSE_LENGTH, RETURN_CODE);
}

void APEX::RAISE_APPLICATION_ERROR_AND_GET_RESPONSE(
    /*!in */ ERROR_CODE_TYPE ERROR_CODE,
    /*!in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
    /*!in */ ERROR_MESSAGE_SIZE_TYPE LENGTH,
    /*!out*/ MESSAGE_ADDR_TYPE HM_RESPONSE,
    /*!in */ MESSAGE_SIZE_TYPE HM_RESPONSE_CAPACITY,
    /*!in */ SYSTEM_TIME_TYPE
        TIME_OUT, // Added Timeout in response to the discussion with Gabor
    /*!out*/ MESSAGE_SIZE_TYPE *HM_RESPONSE_LENGTH,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  __GET_SINGLETON_INSTANCE;

  // find my corresponding blackboard and read from it
  PROCESS_ID_TYPE PROCESS_ID;
  APEX::GET_MY_ID(&PROCESS_ID, RETURN_CODE);

  HM_DESCRIPTOR_ID_TYPE HM_DESCRIPTOR_ID =
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
          ->HM_DESCRIPTOR_ID;
  if (HM_DESCRIPTOR_ID == -1) {
    *RETURN_CODE = NOT_AVAILABLE;
    LOGGER(HMEVENT, "HM Not available for process id %d", PROCESS_ID);
    return;
  }

  BLACKBOARD_ID_TYPE TARGET_BOARD_ID =
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
          ->HM_RESPONSE_BOARD_ID;
  APEX::CLEAR_BLACKBOARD(TARGET_BOARD_ID, RETURN_CODE);
  APEX::RAISE_APPLICATION_ERROR(/*!in */ ERROR_CODE, /*!in */ MESSAGE_ADDR,
                                /*!in */ LENGTH, /*!out*/ RETURN_CODE);

  if (HM_RESPONSE_CAPACITY > (MESSAGE_SIZE_TYPE)MAX_HM_RESPONSE_MESSAGE_SIZE) {
    LOGGER(
        WARNING,
        "HM_RESPONSE_CAPACITY>(MESSAGE_SIZE_TYPE)MAX_HM_RESPONSE_MESSAGE_SIZE");
    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  APEX::READ_BLACKBOARD(TARGET_BOARD_ID, TIME_OUT, HM_RESPONSE,
                        HM_RESPONSE_LENGTH, RETURN_CODE);
  // if(*RETURN_CODE!=TIMED_OUT)
  //				APEX::CLEAR_BLACKBOARD(TARGET_BOARD_ID,RETURN_CODE);
  return;
}

void APEX::GET_PROCESS_HM_BOARD_ID(/*!in */ PROCESS_ID_TYPE PROCESS_ID,
                                   /*!out*/ BLACKBOARD_ID_TYPE *HM_BOARD_ID,
                                   /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  __GET_SINGLETON_INSTANCE;

  if (partition_instance->PROCESS_DESCRIPTOR_VECTOR.size() <= PROCESS_ID) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  } // invalid process id

  HM_DESCRIPTOR_ID_TYPE HM_DESCRIPTOR_ID =
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
          ->HM_DESCRIPTOR_ID;
  if (HM_DESCRIPTOR_ID == -1) {
    *RETURN_CODE = NOT_AVAILABLE;
    LOGGER(INFO, "HM Not available for process id %d", PROCESS_ID);
    return;
  }

  *HM_BOARD_ID = partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
                     ->HM_RESPONSE_BOARD_ID;
}

void APEX::RAISE_APPLICATION_ERROR(/*!in */ ERROR_CODE_TYPE ERROR_CODE,
                                   /*!in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
                                   /*!in */ ERROR_MESSAGE_SIZE_TYPE LENGTH,
                                   /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  __GET_SINGLETON_INSTANCE;
  PROCESS_ID_TYPE PROCESS_ID;
  partition_instance->GET_MY_ID(&PROCESS_ID, RETURN_CODE);
  if (*RETURN_CODE != NO_ERROR)
    return;

  HM_DESCRIPTOR_ID_TYPE HM_DESCRIPTOR_ID =
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
          ->HM_DESCRIPTOR_ID;
  if (HM_DESCRIPTOR_ID == -1) {
    *RETURN_CODE = NOT_AVAILABLE;
    LOGGER(HMEVENT, "HM Not available for process id %d", PROCESS_ID);
    return;
  }

  ERROR_STATUS_TYPE ERROR_STATUS;
  ERROR_STATUS.ERROR_CODE = ERROR_CODE;
  ERROR_STATUS.FAILED_PROCESS_ID = PROCESS_ID;
  ERROR_STATUS.LENGTH = LENGTH;
  memcpy(&(ERROR_STATUS.MESSAGE[0]), MESSAGE_ADDR, LENGTH);

  BUFFER_ID_TYPE BUFFER_ID =
      partition_instance->HM_VECTOR[HM_DESCRIPTOR_ID].HM_BUFFER_ID;
  SEND_BUFFER(
      /*in */ BUFFER_ID,
      /*in */ MESSAGE_ADDR_TYPE(&ERROR_STATUS), /* by reference */
      /*in */ sizeof(ERROR_STATUS),
      /*in */ INFINITE_TIME_VALUE,
      /*out*/ RETURN_CODE);

  return;
}

void APEX::RAISE_APPLICATION_ERROR(
    /*!in*/ PROCESS_ID_TYPE FAULTY_PROCESS_ID,
    /*!in */ ERROR_CODE_TYPE ERROR_CODE,
    /*!in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
    /*!in */ ERROR_MESSAGE_SIZE_TYPE LENGTH,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  __GET_SINGLETON_INSTANCE;

  HM_DESCRIPTOR_ID_TYPE HM_DESCRIPTOR_ID =
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[FAULTY_PROCESS_ID]
          ->HM_DESCRIPTOR_ID;
  if (HM_DESCRIPTOR_ID == -1) {
    *RETURN_CODE = NOT_AVAILABLE;
    LOGGER(HMEVENT, "HM Not available for process id %d", FAULTY_PROCESS_ID);
    return;
  }

  ERROR_STATUS_TYPE ERROR_STATUS;
  ERROR_STATUS.ERROR_CODE = ERROR_CODE;
  ERROR_STATUS.FAILED_PROCESS_ID = FAULTY_PROCESS_ID;
  ERROR_STATUS.LENGTH = LENGTH;
  memcpy(&(ERROR_STATUS.MESSAGE[0]), MESSAGE_ADDR, LENGTH);

  BUFFER_ID_TYPE BUFFER_ID =
      partition_instance->HM_VECTOR[HM_DESCRIPTOR_ID].HM_BUFFER_ID;
  SEND_BUFFER(
      /*in */ BUFFER_ID,
      /*in */ MESSAGE_ADDR_TYPE(&ERROR_STATUS), /* by reference */
      /*in */ sizeof(ERROR_STATUS),
      /*in */ INFINITE_TIME_VALUE,
      /*out*/ RETURN_CODE);

  return;
}

void APEX::CREATE_HEALTH_MANAGER_PROCESS(
    /*in*/ SYSTEM_ADDRESS_TYPE ENTRY_POINT,
    /*in*/ PROCESS_NAME_TYPE PROCESS_NAME,
    /*in*/ STACK_SIZE_TYPE STACK_SIZE,
    /*out*/ HM_DESCRIPTOR_ID_TYPE *HM_ID,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  __GET_SINGLETON_INSTANCE_AND_CREATE_IF_DOES_NOT_EXIST;
  if (partition_instance->CURRENT_OPERATING_MODE == IDLE ||
      partition_instance->CURRENT_OPERATING_MODE == NORMAL) {
    LOGGER(ACM_USER::FATAL, "From CREATE_HEALTH_MANAGER_PROCESS:Trying to "
                            "create HM %s while partition is either in IDLE or "
                            "NORMAL state",
           PROCESS_NAME);

    *RETURN_CODE = INVALID_MODE;
    return;
  }

  PROCESS_ID_TYPE HM_PROC_ID;

  HM_PROC_ID = APEX_HELPER_CREATE_PROCESS(
      INFINITE_TIME_VALUE, // SYSTEM_TIME_TYPE PERIOD,
      INFINITE_TIME_VALUE, // SYSTEM_TIME_TYPE TIME_CAPACITY,
      ENTRY_POINT,         // SYSTEM_ADDRESS_TYPE ENTRY_POINT,
      STACK_SIZE,          // STACK_SIZE_TYPE STACK_SIZE,
      HMPRIORITY,          // PRIORITY_TYPE BASE_PRIORITY,
      HARD,                // DEADLINE_TYPE DEADLINE,
      PROCESS_NAME,        // std::string PROCESS_NAME,
      RETURN_CODE);        // RETURN_CODE_TYPE *RETURN_CODE)
  if (*RETURN_CODE != NO_ERROR)
    return;

  partition_instance->PROCESS_DESCRIPTOR_VECTOR[HM_PROC_ID]->IS_HEALTH_MANAGER =
      true;

  BUFFER_NAME_TYPE BUFFER_NAME;
  strcpy(BUFFER_NAME, PROCESS_NAME);
  strcat(BUFFER_NAME, "_HMB");

  BUFFER_ID_TYPE HM_BUFFER_ID;

  CREATE_BUFFER(BUFFER_NAME, sizeof(ERROR_STATUS_TYPE), MAX_HM_BUFFER_MESSGAES,
                FIFO, &HM_BUFFER_ID, RETURN_CODE);

  if (*RETURN_CODE != NO_ERROR)
    return;
  HM_DESCRIPTOR_TYPE HM_DESCRIPTOR;
  HM_DESCRIPTOR.HM_BUFFER_ID = HM_BUFFER_ID;
  HM_DESCRIPTOR.HM_PROCESS_ID = HM_PROC_ID;
  HM_DESCRIPTOR.CURRENTLY_REGISTERED_PROCS = -1;

  *HM_ID = partition_instance->HM_VECTOR.size();
  partition_instance->HM_VECTOR.push_back(HM_DESCRIPTOR);
  return;
}

void APEX::HM_GET_MY_BUFFER_ID(/*OUT*/ BUFFER_ID_TYPE *BUFFER_ID,
                               /*out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  __GET_SINGLETON_INSTANCE;
  PROCESS_ID_TYPE MY_PRC_ID;
  APEX::GET_MY_ID(&MY_PRC_ID, RETURN_CODE);
  if (*RETURN_CODE != NO_ERROR)
    return;

  if (!partition_instance->PROCESS_DESCRIPTOR_VECTOR[MY_PRC_ID]
           ->IS_HEALTH_MANAGER) {
    *RETURN_CODE = NOT_AVAILABLE;
    return;
  }

  HM_DESCRIPTOR_ID_TYPE HM_DESCRIPTOR_ID = -1;
  for (unsigned int count = 0; count < partition_instance->HM_VECTOR.size();
       count++) {
    if (partition_instance->HM_VECTOR[count].HM_PROCESS_ID == MY_PRC_ID) {
      HM_DESCRIPTOR_ID = count;
      break;
    }
  }
  if (HM_DESCRIPTOR_ID == -1) {
    *RETURN_CODE = INVALID_CONFIG;
    LOGGER(HMEVENT, "FATAL: HM_GET_MY_BUFFER_ID cannot find HM descriptor for "
                    "HM process %d",
           MY_PRC_ID);
    return;
  }

  *BUFFER_ID = partition_instance->HM_VECTOR[HM_DESCRIPTOR_ID].HM_BUFFER_ID;
  return;
}

void APEX::HM_GET_MY_DESCRIPTOR_ID(
    /*OUT*/ HM_DESCRIPTOR_ID_TYPE *HM_DESCRIPTOR_ID,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  __GET_SINGLETON_INSTANCE;
  PROCESS_ID_TYPE MY_PRC_ID;
  APEX::GET_MY_ID(&MY_PRC_ID, RETURN_CODE);
  if (*RETURN_CODE != NO_ERROR)
    return;

  if (!partition_instance->PROCESS_DESCRIPTOR_VECTOR[MY_PRC_ID]
           ->IS_HEALTH_MANAGER) {
    *RETURN_CODE = NOT_AVAILABLE;
    return;
  }

  *HM_DESCRIPTOR_ID = -1;
  for (unsigned int count = 0; count < partition_instance->HM_VECTOR.size();
       count++) {
    if (partition_instance->HM_VECTOR[count].HM_PROCESS_ID == MY_PRC_ID) {
      *HM_DESCRIPTOR_ID = count;
      break;
    }
  }
  if (*HM_DESCRIPTOR_ID == -1) {
    *RETURN_CODE = INVALID_CONFIG;
    LOGGER(HMEVENT, "FATAL: HM_GET_MY_BUFFER_ID cannot find HM descriptor for "
                    "HM process %d",
           MY_PRC_ID);
    return;
  }

  return;
}

void APEX::HM_NOTIFY_PROCESS(/*!in*/ PROCESS_ID_TYPE PROCESS_ID,
                             /*!in*/ HM_RESPONSE_TYPE *HM_RESPONSE,
                             /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  __GET_SINGLETON_INSTANCE;
  HM_DESCRIPTOR_ID_TYPE HM_DESCRIPTOR_ID;
  APEX::HM_GET_MY_DESCRIPTOR_ID(/*OUT*/ &HM_DESCRIPTOR_ID, /*out*/ RETURN_CODE);
  if (*RETURN_CODE != NO_ERROR) {
    return;
  }

  // check if HM is indeed the

  if (partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
          ->HM_DESCRIPTOR_ID != HM_DESCRIPTOR_ID) {
    LOGGER(HMEVENT,
           "HM id %ld Incorrectly trying to send message to process id %ld",
           HM_DESCRIPTOR_ID, PROCESS_ID);
    *RETURN_CODE = NOT_AVAILABLE;
    return;
  }
  BLACKBOARD_ID_TYPE TARGET_BOARD_ID =
      partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
          ->HM_RESPONSE_BOARD_ID;

  // write to blackboard

  APEX::DISPLAY_BLACKBOARD(TARGET_BOARD_ID, (MESSAGE_ADDR_TYPE)HM_RESPONSE,
                           MESSAGE_SIZE_TYPE(sizeof(HM_RESPONSE_TYPE)),
                           RETURN_CODE);
  return;
}

void APEX::HM_GET_PROCESS_ERROR_MESSAGE(
    /*out*/ ERROR_STATUS_TYPE *ERROR_STATUS,
    /*out*/ BLACKBOARD_ID_TYPE *TARGET_BBOARD_ID,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  __GET_SINGLETON_INSTANCE;

  BUFFER_ID_TYPE BUFFER_ID;
  HM_DESCRIPTOR_ID_TYPE HM_DESCRIPTOR_ID;

  APEX::HM_GET_MY_DESCRIPTOR_ID(/*OUT*/ &HM_DESCRIPTOR_ID, /*out*/ RETURN_CODE);
  if (*RETURN_CODE != NO_ERROR) {
    return;
  }
  BUFFER_ID = partition_instance->HM_VECTOR[HM_DESCRIPTOR_ID].HM_BUFFER_ID;
  // LOGGER(HMEVENT,"My buffer id is %d and return code is
  // %d",BUFFER_ID,*RETURN_CODE);
  MESSAGE_SIZE_TYPE LENGTH;
  APEX::RECEIVE_BUFFER(
      /*in */ BUFFER_ID,
      /*in */ INFINITE_TIME_VALUE,
      /*out*/ MESSAGE_ADDR_TYPE(ERROR_STATUS),
      /*out*/ &LENGTH,
      /*out*/ RETURN_CODE);

  if (*RETURN_CODE != NO_ERROR) {
    return;
  }
  PROCESS_ID_TYPE FAILED_ID = ERROR_STATUS->FAILED_PROCESS_ID;

  int registeredProcNumbers = partition_instance->HM_VECTOR[HM_DESCRIPTOR_ID]
                                  .CURRENTLY_REGISTERED_PROCS;

  if (partition_instance->PROCESS_DESCRIPTOR_VECTOR[FAILED_ID]
          ->HM_DESCRIPTOR_ID != HM_DESCRIPTOR_ID) {
    LOGGER(HMEVENT, "Incorrectly received error message for process id %d",
           FAILED_ID);
    *RETURN_CODE = NOT_AVAILABLE;
    return;
  }

  for (int i = 0; i <= registeredProcNumbers; i++) {
    if (partition_instance->HM_VECTOR[HM_DESCRIPTOR_ID]
            .PROC_HM_ASSOCIATION[i]
            .PROCESS_ID == FAILED_ID) {
      *TARGET_BBOARD_ID = partition_instance->HM_VECTOR[HM_DESCRIPTOR_ID]
                              .PROC_HM_ASSOCIATION[i]
                              .BLACKBOARD_ID;
      return;
    }
  }

  LOGGER(HMEVENT, "ERROR:Could not find target blackboard id");
  *RETURN_CODE = NOT_AVAILABLE;

  return;
}

void APEX::REGISTER_PROCESS_WITH_HEALTH_MANAGER(
    PROCESS_ID_TYPE PROCESS_ID, HM_DESCRIPTOR_ID_TYPE HM_DESCRIPTOR_ID,
    BLACKBOARD_ID_TYPE *BLACKBOARD_ID, /*out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  // std::cout<<HM_DESCRIPTOR_ID<<"HM_DESCRIPTOR_ID"<<std::endl;
  *RETURN_CODE = NO_ERROR;
  __GET_SINGLETON_INSTANCE;

  // CHECK PARTITION NOT NORMAL
  if (partition_instance->CURRENT_OPERATING_MODE == IDLE ||
      partition_instance->CURRENT_OPERATING_MODE == NORMAL) {
    LOGGER(ACM_USER::FATAL, "From REGISTER_PROCESS_WITH_HEALTH_MANAGER:Trying "
                            "to register process while partition is either in "
                            "IDLE or NORMAL state");

    *RETURN_CODE = INVALID_MODE;
    return;
  }

  if ((unsigned int)PROCESS_ID >=
      partition_instance->PROCESS_DESCRIPTOR_VECTOR.size()) {
    LOGGER(ACM_USER::FATAL, "From "
                            "REGISTER_PROCESS_WITH_HEALTH_MANAGER:Process_ID "
                            "%d does not exist",
           PROCESS_ID);
    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  /*if((unsigned
  int)BLACKBOARD_ID>=partition_instance->BLACKBOARD_VECTOR.size())
  {
          LOGGER( ACM_USER::FATAL,"From
  REGISTER_PROCESS_WITH_HEALTH_MANAGER:Blackboard_ID%d does not
  exist",BLACKBOARD_ID);
          *RETURN_CODE=INVALID_PARAM;
          return;
  }*/
  BLACKBOARD_NAME_TYPE BLACKBOARD_NAME;
  snprintf(BLACKBOARD_NAME, MAX_NAME_LENGTH, "Process%d", (int)PROCESS_ID);

  // strcpy(BLACKBOARD_NAME,
  // partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->PROCESS_NAME.c_str());

  // LOGGER(APP,"ASKING to create Blackboard %s",BLACKBOARD_NAME);
  // strcat(BLACKBOARD_NAME,"_HMB");

  if (HM_DESCRIPTOR_ID >= (int)partition_instance->HM_VECTOR.size()) {
    LOGGER(ACM_USER::FATAL, "From "
                            "REGISTER_PROCESS_WITH_HEALTH_MANAGER:HM_"
                            "DESCRIPTOR_ID %d does not exist",
           HM_DESCRIPTOR_ID);
    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  CREATE_BLACKBOARD(BLACKBOARD_NAME, MAX_HM_RESPONSE_MESSAGE_SIZE,
                    BLACKBOARD_ID, RETURN_CODE);
  if (*RETURN_CODE != NO_ERROR) {
    LOGGER(ERROR, "FAILED WHILE Creating blackboard %s", BLACKBOARD_NAME);
    return;
  }

  partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]->HM_DESCRIPTOR_ID =
      HM_DESCRIPTOR_ID;
  partition_instance->PROCESS_DESCRIPTOR_VECTOR[PROCESS_ID]
      ->HM_RESPONSE_BOARD_ID = *BLACKBOARD_ID;
  partition_instance->HM_VECTOR[HM_DESCRIPTOR_ID].CURRENTLY_REGISTERED_PROCS++;
  int registerednumber = partition_instance->HM_VECTOR[HM_DESCRIPTOR_ID]
                             .CURRENTLY_REGISTERED_PROCS;
  LOGGER(INFO, "REGISTERED NUMBER OF PROCESSES with health manager %s is %d",
         partition_instance
             ->PROCESS_DESCRIPTOR_VECTOR
                 [partition_instance->HM_VECTOR[HM_DESCRIPTOR_ID].HM_PROCESS_ID]
             ->THE_PROCESS->get_name()
             .c_str(),
         registerednumber + 1);

  partition_instance->HM_VECTOR[HM_DESCRIPTOR_ID]
      .PROC_HM_ASSOCIATION[registerednumber]
      .PROCESS_ID = PROCESS_ID;
  partition_instance->HM_VECTOR[HM_DESCRIPTOR_ID]
      .PROC_HM_ASSOCIATION[registerednumber]
      .BLACKBOARD_ID = *BLACKBOARD_ID;

  return;
}

void APEX::START_HM(/*!in */ HM_DESCRIPTOR_ID_TYPE HM_DESCRIPTOR_ID,
                    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  __GET_SINGLETON_INSTANCE;
  if ((HM_DESCRIPTOR_ID >= (int)partition_instance->HM_VECTOR.size()) ||
      (HM_DESCRIPTOR_ID < 0)) {
    LOGGER(ACM_USER::FATAL, "From START_HM:HM_DESCRIPTOR_ID %d does not exist",
           HM_DESCRIPTOR_ID);
    *RETURN_CODE = INVALID_PARAM;
    return;
  }
  START(partition_instance->HM_VECTOR[HM_DESCRIPTOR_ID].HM_PROCESS_ID,
        RETURN_CODE);

  return;
}
}
#endif
