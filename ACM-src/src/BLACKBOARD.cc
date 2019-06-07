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

#ifndef ACM_APEX_BLACKBOARD_CC
#define ACM_APEX_BLACKBOARD_CC
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>

#include <stdlib.h>

namespace ACM_INTERNAL {

/**********************************************************************************************************************/

APEX_BLACKBOARD_TYPE::APEX_BLACKBOARD_TYPE(BLACKBOARD_NAME_TYPE NAME,
                                           BLACKBOARD_ID_TYPE ID,
                                           MESSAGE_SIZE_TYPE size)
    : BLACKBOARD_EVENT_ID(0), BLACKBOARD_NAME(NAME), BLACKBOARD_ID(ID),
      MAX_MESSAGE_SIZE(size), CURRENT_MESSAGE_SIZE(0), BLACKBOARD_MUTEX(),
      PAYLOAD(0), rw_lock() {
  // Black Board has been created. it will be initialized now.
  PAYLOAD = malloc(MAX_MESSAGE_SIZE);
  RETURN_CODE_TYPE RETURN_CODE;
  APEX::CREATE_EVENT(NAME, &BLACKBOARD_EVENT_ID, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);
}

APEX_BLACKBOARD_TYPE::~APEX_BLACKBOARD_TYPE() {
  if (PAYLOAD != 0) {
    // LOGGER(ACM_USER::DEBUG,"Deleting payload for black board
    // %s",BLACKBOARD_NAME.c_str());
    free(PAYLOAD);
    // LOGGER(ACM_USER::DEBUG,"Deleted payload for black board
    // %s",BLACKBOARD_NAME.c_str());
  }
  LOGGER(ACM_USER::DEBUG, "Deleted black board %s", BLACKBOARD_NAME.c_str());
}
void APEX_BLACKBOARD_TYPE::ERASE_BLACKBOARD(RETURN_CODE_TYPE *RETURN_CODE) {
  AUTO_LOCK lock(BLACKBOARD_MUTEX);
  this->rw_lock.wrlock();
  memset(PAYLOAD, '0', MAX_MESSAGE_SIZE);
  this->rw_lock.unlock();
  // reset event
  APEX::RESET_EVENT(BLACKBOARD_EVENT_ID, RETURN_CODE);
  // RESET MESSAGE SIZE
  CURRENT_MESSAGE_SIZE = 0;
}

void APEX_BLACKBOARD_TYPE::WRITE_ON_BLACKBOARD(MESSAGE_ADDR_TYPE MESSAGE_ADDR,
                                               MESSAGE_SIZE_TYPE LENGTH,
                                               RETURN_CODE_TYPE *RETURN_CODE) {
  assert(LENGTH <= MAX_MESSAGE_SIZE);
  AUTO_LOCK lock(BLACKBOARD_MUTEX);
  this->rw_lock.wrlock();
  memcpy(PAYLOAD, MESSAGE_ADDR, LENGTH);
  this->rw_lock.unlock();
  *RETURN_CODE = NO_ERROR;
  CURRENT_MESSAGE_SIZE = LENGTH;
  APEX::SET_EVENT(BLACKBOARD_EVENT_ID, RETURN_CODE);
  return;
}

void APEX_BLACKBOARD_TYPE::READ_FROM_BLACKBOARD(MESSAGE_ADDR_TYPE MESSAGE_ADDR,
                                                MESSAGE_SIZE_TYPE *LENGTH,
                                                RETURN_CODE_TYPE *RETURN_CODE) {
  this->rw_lock.rdlock();
  *LENGTH = this->CURRENT_MESSAGE_SIZE;
  memcpy(MESSAGE_ADDR, this->PAYLOAD, *LENGTH);
  this->rw_lock.unlock();
  *RETURN_CODE = NO_ERROR;
  return;
}

/**************************************************************************************************************************/
}
#endif // ACM_APEX_BLACKBOARD_H