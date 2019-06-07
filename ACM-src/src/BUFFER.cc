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

#ifndef ACM_APEX_BUFFER_CC
#define ACM_APEX_BUFFER_CC
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>

#include <stdlib.h>

namespace ACM_INTERNAL {

/**********************************************************************************************************************/

APEX_BUFFER_TYPE::APEX_BUFFER_TYPE(BUFFER_NAME_TYPE NAME, BUFFER_ID_TYPE ID,
                                   MESSAGE_SIZE_TYPE size,
                                   MESSAGE_RANGE_TYPE MAX_NB_MESSAGE)
    : BUFFER_NAME(NAME), BUFFER_ID(ID), BUFFER_MAX_MESSAGE_SIZE(size),
      BUFFER_MAX_NB_MESSAGE(MAX_NB_MESSAGE), CURRENT_MESSAGE_SIZE(0),
      BUFFER_HEAD(0), BUFFER_TAIL(0), PAYLOAD(0),
      BUFFER_READ_SEMAPHORE((ACM_USER::SEMAPHORE_VALUE_TYPE)0,
                            (ACM_USER::SEMAPHORE_VALUE_TYPE)MAX_NB_MESSAGE),
      BUFFER_WRITE_SEMAPHORE((ACM_USER::SEMAPHORE_VALUE_TYPE)MAX_NB_MESSAGE,
                             (ACM_USER::SEMAPHORE_VALUE_TYPE)MAX_NB_MESSAGE),
      BUFFER_MUTEX() {
  // Buffer has been created. it will be initialized now.
  PAYLOAD =
      (PAYLOAD_TYPE *)malloc(sizeof(PAYLOAD_TYPE) * BUFFER_MAX_NB_MESSAGE);
  for (int i = 0; i < BUFFER_MAX_NB_MESSAGE; i++) {
    PAYLOAD[i].ACTUAL_LOAD = (MESSAGE_ADDR_TYPE)malloc(BUFFER_MAX_MESSAGE_SIZE);
  }
}

APEX_BUFFER_TYPE::~APEX_BUFFER_TYPE() {

  BUFFER_MUTEX.lock();

  for (int i = 0; i < BUFFER_MAX_NB_MESSAGE; i++) {
    free(PAYLOAD[i].ACTUAL_LOAD);
  }

  if (PAYLOAD != 0) {
    free(PAYLOAD);
  }
  LOGGER(ACM_USER::DEBUG, "Deleted buffer %s", BUFFER_NAME.c_str());

  BUFFER_MUTEX.unlock();
}

void APEX_BUFFER_TYPE::RECEIVE_MESSAGE(/*in */ SYSTEM_TIME_TYPE TIME_OUT,
                                       /*out*/ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
                                       /*out*/ MESSAGE_SIZE_TYPE *LENGTH,
                                       /*out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = BUFFER_READ_SEMAPHORE.wait(TIME_OUT);
  if (*RETURN_CODE == TIMED_OUT) {
    if (TIME_OUT == 0)
      *RETURN_CODE = NOT_AVAILABLE;
    return;
  }

  AUTO_LOCK lock(BUFFER_MUTEX);

  *LENGTH = PAYLOAD[BUFFER_HEAD].LENGTH;
  memcpy(MESSAGE_ADDR, PAYLOAD[BUFFER_HEAD].ACTUAL_LOAD, *LENGTH);

  memset(PAYLOAD[BUFFER_HEAD].ACTUAL_LOAD, '0', BUFFER_MAX_MESSAGE_SIZE);
  PAYLOAD[BUFFER_HEAD].LENGTH = 0;

  BUFFER_HEAD += 1;
  if (BUFFER_HEAD == BUFFER_MAX_NB_MESSAGE)
    BUFFER_HEAD = 0;

  CURRENT_MESSAGE_SIZE -= 1;

  BUFFER_WRITE_SEMAPHORE.post(); // set signal to read.

  *RETURN_CODE = NO_ERROR;

  return;
}

void APEX_BUFFER_TYPE::SEND_MESSAGE(
    /*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
    /* by reference */ /*in */ MESSAGE_SIZE_TYPE LENGTH,
    /*in */ SYSTEM_TIME_TYPE TIME_OUT, /*out*/ RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = BUFFER_WRITE_SEMAPHORE.wait(TIME_OUT);
  if (*RETURN_CODE == TIMED_OUT) {
    if (TIME_OUT == 0)
      *RETURN_CODE = NOT_AVAILABLE;
    return;
  }

  AUTO_LOCK lock(BUFFER_MUTEX);

  memcpy(PAYLOAD[BUFFER_TAIL].ACTUAL_LOAD, MESSAGE_ADDR, LENGTH);
  PAYLOAD[BUFFER_TAIL].LENGTH = LENGTH;
  CURRENT_MESSAGE_SIZE += 1;

  BUFFER_TAIL += 1;
  if (BUFFER_TAIL == BUFFER_MAX_NB_MESSAGE)
    BUFFER_TAIL = 0;

  BUFFER_READ_SEMAPHORE.post(); // set signal to read.

  *RETURN_CODE = NO_ERROR;

  return;
}

void APEX_BUFFER_TYPE::GET_BUFFER_STATUS(
    /*out*/ BUFFER_STATUS_TYPE *BUFFER_STATUS,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE) {

  AUTO_LOCK lock(BUFFER_MUTEX);

  BUFFER_STATUS->NB_MESSAGE = CURRENT_MESSAGE_SIZE;
  BUFFER_STATUS->MAX_NB_MESSAGE = BUFFER_MAX_NB_MESSAGE;
  BUFFER_STATUS->MAX_MESSAGE_SIZE = BUFFER_MAX_MESSAGE_SIZE;
  BUFFER_STATUS->WAITING_PROCESSES = BUFFER_READ_SEMAPHORE.GET_WAITING_COUNT() +
                                     BUFFER_WRITE_SEMAPHORE.GET_WAITING_COUNT();
  *RETURN_CODE = NO_ERROR;
}

/**************************************************************************************************************************/
}
#endif // ACM_APEX_BUFFER_CC
