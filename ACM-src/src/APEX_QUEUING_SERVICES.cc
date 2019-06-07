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
#ifndef APEX_QUEUING_CC
#define APEX_QUEUING_CC
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
using namespace ACM_USER;
namespace ACM_INTERNAL {

//////////////////////////////////////////////////QUEUING
///PORTS//////////////////////////////////

//! \name QUEUING PORT Management Services
//@{
/* !
*
*/
void APEX::CREATE_QUEUING_PORT(
    /*in */ QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME,
    /*in */ MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
    /*in */ MESSAGE_RANGE_TYPE MAX_NB_MESSAGE,
    /*in */ PORT_DIRECTION_TYPE PORT_DIRECTION,
    /*in */ QUEUING_DISCIPLINE_TYPE QUEUING_DISCIPLINE,
    /*out*/ QUEUING_PORT_ID_TYPE *QUEUING_PORT_ID,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE_AND_CREATE_IF_DOES_NOT_EXIST;
  if (partition_instance->CURRENT_OPERATING_MODE == NORMAL) {
    LOGGER(ERROR, "From CREATE_QUEUING_PORT: Trying to %s while partition is "
                  "in NORMAL state",
           QUEUING_PORT_NAME);
    *RETURN_CODE = INVALID_MODE;
    return;
  }
  if (partition_instance->QUEUING_PORT_VECTOR.size() >=
      SYSTEM_LIMIT_NUMBER_OF_QUEUING_PORTS) {
    LOGGER(ERROR,
           "From CREATE_QUEUING_PORT %s: exceed sampling port number limit",
           QUEUING_PORT_NAME);
    *RETURN_CODE = INVALID_CONFIG;
    return;
  }

  switch (QUEUING_DISCIPLINE) {
  case FIFO:
  case PRIORITY:
    LOGGER(WARNING, "%s, QUEUING_DISCIPLINE of PRIORITY for queuing ports is "
                    "not supported.",
           QUEUING_PORT_NAME);
    break;
  default:
    *RETURN_CODE = INVALID_PARAM;
    return;
  }

  if (MAX_MESSAGE_SIZE <= 0) {
    LOGGER(ERROR, "From CREATE_QUEUING_PORT: %s: MAX_MESSAGE_SIZE is <=0",
           QUEUING_PORT_NAME);
    *RETURN_CODE = INVALID_CONFIG;
    return;
  }

  if (MAX_NB_MESSAGE <= 0 || MAX_NB_MESSAGE > SYSTEM_LIMIT_NUMBER_OF_MESSAGES) {
    LOGGER(ERROR, "From CREATE_QUEUING_PORT: %s: MAX_NB_MESSAGE is <=0 or it "
                  "is > than SYSTEM_LIMIT_NUMBER_OF_MESSAGES",
           QUEUING_PORT_NAME);
    *RETURN_CODE = INVALID_CONFIG;
    return;
  }

  // Ensure that the SAMPLING_PORT_NAME is unique
  for (unsigned int i = 0; i < partition_instance->QUEUING_PORT_VECTOR.size();
       i++) {
    if (partition_instance->QUEUING_PORT_VECTOR[i]->Q_NAME ==
        std::string(QUEUING_PORT_NAME)) {
      LOGGER(ERROR, "From CREATE_QUEUING_PORT: NAME %s is not unique",
             QUEUING_PORT_NAME);
      *RETURN_CODE = NO_ACTION;
      return;
    }
  }
  // check port direction
  // create the sampling port
  *QUEUING_PORT_ID = partition_instance->QUEUING_PORT_VECTOR.size();
  ACM_USER::DIRECTION_TYPE new_port_direction;
  if (PORT_DIRECTION == ACM_USER::SOURCE)
    new_port_direction = ACM_USER::PORT_SOURCE;
  if (PORT_DIRECTION == ACM_USER::DESTINATION)
    new_port_direction = ACM_USER::PORT_DEST;
  MESSAGE_QUEUE_TYPE *queue_port = new MESSAGE_QUEUE_TYPE(
      QUEUING_PORT_NAME, MAX_MESSAGE_SIZE, MAX_NB_MESSAGE, new_port_direction,
      RETURN_CODE, O_CREAT);
  if (*RETURN_CODE == NO_ERROR)
    partition_instance->QUEUING_PORT_VECTOR.push_back(queue_port);
  return;
}
void APEX::SEND_QUEUING_MESSAGE(
    /*in */ QUEUING_PORT_ID_TYPE QUEUING_PORT_ID,
    /*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR, /* by reference */
    /*in */ MESSAGE_SIZE_TYPE LENGTH,
    /*in */ SYSTEM_TIME_TYPE TIME_OUT,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;

  if ((unsigned int)QUEUING_PORT_ID >=
      partition_instance->QUEUING_PORT_VECTOR.size()) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  }
  MESSAGE_QUEUE_TYPE *queue_port =
      partition_instance->QUEUING_PORT_VECTOR[QUEUING_PORT_ID];
  queue_port->SEND_MESSAGE(MESSAGE_ADDR, LENGTH, TIME_OUT, RETURN_CODE);
}

void APEX::MESSAGE_QUEUE_SEND_MESSAGE_FROM_BUFFER(
    QUEUING_PORT_ID_TYPE QUEUING_PORT_ID, MESSAGE_ADDR_TYPE msg_ptr,
    MESSAGE_SIZE_TYPE LENGTH, SYSTEM_TIME_TYPE TIME_OUT,
    RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;

  if ((unsigned int)QUEUING_PORT_ID >=
      partition_instance->QUEUING_PORT_VECTOR.size()) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  }
  MESSAGE_QUEUE_TYPE *queue_port =
      partition_instance->QUEUING_PORT_VECTOR[QUEUING_PORT_ID];
  queue_port->SEND_MESSAGE_FROM_BUFFER_READ_FROM_SOCKET(msg_ptr, LENGTH,
                                                        TIME_OUT, RETURN_CODE);
}

void APEX::RECEIVE_QUEUING_MESSAGE(
    /*in */ QUEUING_PORT_ID_TYPE QUEUING_PORT_ID,
    /*in */ SYSTEM_TIME_TYPE TIME_OUT,
    /*out*/ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
    /*out*/ MESSAGE_SIZE_TYPE *LENGTH,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;

  if ((unsigned int)QUEUING_PORT_ID >=
      partition_instance->QUEUING_PORT_VECTOR.size()) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  }
  MESSAGE_QUEUE_TYPE *queue_port =
      partition_instance->QUEUING_PORT_VECTOR[QUEUING_PORT_ID];

  queue_port->RECEIVE_MESSAGE(MESSAGE_ADDR, LENGTH, TIME_OUT, RETURN_CODE);

  return;
}
void APEX::GET_QUEUING_PORT_ID(
    /*in */ QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME,
    /*out*/ QUEUING_PORT_ID_TYPE *QUEUING_PORT_ID,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;
  *RETURN_CODE = INVALID_CONFIG;
  // Ensure that the SAMPLING_PORT_NAME is unique
  for (unsigned int i = 0; i < partition_instance->QUEUING_PORT_VECTOR.size();
       i++) {
    if (partition_instance->QUEUING_PORT_VECTOR[i]->Q_NAME ==
        std::string(QUEUING_PORT_NAME)) {
      *QUEUING_PORT_ID = i;
      *RETURN_CODE = NO_ERROR;
      return;
    }
  }
}
void APEX::GET_QUEUING_PORT_STATUS(
    /*in */ QUEUING_PORT_ID_TYPE QUEUING_PORT_ID,
    /*out*/ QUEUING_PORT_STATUS_TYPE *QUEUING_PORT_STATUS,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  __GET_SINGLETON_INSTANCE;

  if ((unsigned int)QUEUING_PORT_ID >=
      partition_instance->QUEUING_PORT_VECTOR.size()) {
    *RETURN_CODE = INVALID_PARAM;
    return;
  }
  MESSAGE_QUEUE_TYPE *queue_port =
      partition_instance->QUEUING_PORT_VECTOR[QUEUING_PORT_ID];
  queue_port->GET_QUEUING_PORT_STATUS(QUEUING_PORT_STATUS, RETURN_CODE);
  return;
}
}
#endif // APEX_QUEUING_CC