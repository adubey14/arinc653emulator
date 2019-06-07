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
#ifndef APEX_BUFFER_CC
#define APEX_BUFFER_CC
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
using namespace ACM_USER;
namespace ACM_INTERNAL {
////////////////////////////////////////BUFFER
///RELATED//////////////////////////////////////////////////////

void APEX::CREATE_BUFFER(BUFFER_NAME_TYPE BUFFER_NAME,
                         MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
                         MESSAGE_RANGE_TYPE MAX_NB_MESSAGE,
                         QUEUING_DISCIPLINE_TYPE QUEUING_DISCIPLINE,
                         BUFFER_ID_TYPE *BUFFER_ID,
                         RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE_AND_CREATE_IF_DOES_NOT_EXIST;
  if (partition_instance->CURRENT_OPERATING_MODE == NORMAL) {
    LOGGER(
        ERROR,
        "From CREATE_BUFFER: Trying to %s while partition is in NORMAL state",
        BUFFER_NAME);
    *RETURN_CODE = INVALID_MODE;
    return;
  }
  if (partition_instance->BUFFER_VECTOR.size() >=
      SYSTEM_LIMIT_NUMBER_OF_BUFFERS) {
    LOGGER(ERROR,
           "From CREATE_BUFFER %s: exceed SYSTEM_LIMIT_NUMBER_OF_BUFFERS",
           BUFFER_NAME);
    *RETURN_CODE = INVALID_CONFIG;
    return;
  }

  switch (QUEUING_DISCIPLINE) {
  case FIFO:
  case PRIORITY:
    LOGGER(WARNING,
           "%s, QUEUING_DISCIPLINE of PRIORITY for buffers is not supported.",
           BUFFER_NAME);
    break;
  default:
    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  if (MAX_MESSAGE_SIZE <= 0) {
    LOGGER(ERROR, "From CREATE_BUFFER: %s: MAX_MESSAGE_SIZE is <=0",
           BUFFER_NAME);
    *RETURN_CODE = INVALID_CONFIG;
    return;
  }

  if (MAX_NB_MESSAGE <= 0 || MAX_NB_MESSAGE > SYSTEM_LIMIT_NUMBER_OF_MESSAGES) {
    LOGGER(ERROR, "From CREATE_BUFFER: %s: MAX_NB_MESSAGE is <=0 or it is > "
                  "than SYSTEM_LIMIT_NUMBER_OF_MESSAGES",
           BUFFER_NAME);
    *RETURN_CODE = INVALID_CONFIG;
    return;
  }

  // Ensure that the SAMPLING_PORT_NAME is unique
  for (unsigned int i = 0; i < partition_instance->BUFFER_VECTOR.size(); i++) {
    if (partition_instance->BUFFER_VECTOR[i]->BUFFER_NAME ==
        std::string(BUFFER_NAME)) {
      LOGGER(ERROR, "From CREATE_BUFFER: NAME %s is not unique", BUFFER_NAME);
      *RETURN_CODE = NO_ACTION;
      return;
    }
  }
  // check port direction
  // create the sampling port
  *BUFFER_ID = partition_instance->BUFFER_VECTOR.size();
  APEX_BUFFER_TYPE *APEX_BUFFER = new APEX_BUFFER_TYPE(
      BUFFER_NAME, *BUFFER_ID, MAX_MESSAGE_SIZE, MAX_NB_MESSAGE);
  partition_instance->BUFFER_VECTOR.push_back(APEX_BUFFER);
  *RETURN_CODE = NO_ERROR;
  LOGGER(INFO, "From CREATE_BUFFER: Successful");
  return;
}
void APEX::SEND_BUFFER(/*in */ BUFFER_ID_TYPE BUFFER_ID,
                       /*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
                       /* by reference */ /*in */ MESSAGE_SIZE_TYPE LENGTH,
                       /*in */ SYSTEM_TIME_TYPE TIME_OUT,
                       /*out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;
  if ((unsigned int)BUFFER_ID >= partition_instance->BUFFER_VECTOR.size()) {
    LOGGER(ERROR, "CANNOT FIND CURRENT BUFFER ID %u", BUFFER_ID);
    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  if (TIME_OUT > (SYSTEM_TIME_TYPE)MAX_TIME_OUT) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  APEX_BUFFER_TYPE *APEX_BUFFER = partition_instance->BUFFER_VECTOR[BUFFER_ID];

  APEX_BUFFER->SEND_MESSAGE(MESSAGE_ADDR, LENGTH, TIME_OUT, RETURN_CODE);
  return;
}

void APEX::RECEIVE_BUFFER(/*in */ BUFFER_ID_TYPE BUFFER_ID,
                          /*in */ SYSTEM_TIME_TYPE TIME_OUT,
                          /*out*/ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
                          /*out*/ MESSAGE_SIZE_TYPE *LENGTH,
                          /*out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;
  if ((unsigned int)BUFFER_ID >= partition_instance->BUFFER_VECTOR.size()) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  if (TIME_OUT > (SYSTEM_TIME_TYPE)MAX_TIME_OUT) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  APEX_BUFFER_TYPE *APEX_BUFFER = partition_instance->BUFFER_VECTOR[BUFFER_ID];
  APEX_BUFFER->RECEIVE_MESSAGE(TIME_OUT, MESSAGE_ADDR, LENGTH, RETURN_CODE);
  return;
}

void APEX::GET_BUFFER_ID(/*in */ BUFFER_NAME_TYPE BUFFER_NAME,
                         /*out*/ BUFFER_ID_TYPE *BUFFER_ID,
                         /*out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;
  *RETURN_CODE = INVALID_CONFIG;
  // Ensure that the SAMPLING_PORT_NAME is unique
  for (unsigned int i = 0; i < partition_instance->BUFFER_VECTOR.size(); i++) {
    if (partition_instance->BUFFER_VECTOR[i]->BUFFER_NAME ==
        std::string(BUFFER_NAME)) {
      *BUFFER_ID = i;
      *RETURN_CODE = NO_ERROR;
      return;
    }
  }
}

void APEX::GET_BUFFER_STATUS(/*in */ BUFFER_ID_TYPE BUFFER_ID,
                             /*out*/ BUFFER_STATUS_TYPE *BUFFER_STATUS,
                             /*out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;

  if ((unsigned int)BUFFER_ID >= partition_instance->BUFFER_VECTOR.size()) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  APEX_BUFFER_TYPE *APEX_BUFFER = partition_instance->BUFFER_VECTOR[BUFFER_ID];
  APEX_BUFFER->GET_BUFFER_STATUS(BUFFER_STATUS, RETURN_CODE);

  return;
}
}
#endif // APEX_BUFFER_CC