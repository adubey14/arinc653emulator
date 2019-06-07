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

#ifndef ACM_APEX_BUFFER_H
#define ACM_APEX_BUFFER_H

#include <ACM/INTERNAL/ACM_THREAD_SUPPORT.h>
#include <queue>
#include <stdlib.h>
#include <string.h>
namespace ACM_INTERNAL {
class APEX;
}
using namespace ACM_USER;
namespace ACM_INTERNAL {
//! An internal class used that provides facilities to create and manage
//! blackboards.
class APEX_BUFFER_TYPE {
  friend class ACM_INTERNAL::APEX;

public:
  std::string BUFFER_NAME;
  BUFFER_ID_TYPE BUFFER_ID;
  MESSAGE_SIZE_TYPE BUFFER_MAX_MESSAGE_SIZE;
  MESSAGE_RANGE_TYPE BUFFER_MAX_NB_MESSAGE;
  MESSAGE_RANGE_TYPE CURRENT_MESSAGE_SIZE;

private:
  int BUFFER_HEAD;
  int BUFFER_TAIL;
  struct PAYLOAD_TYPE {
    MESSAGE_ADDR_TYPE ACTUAL_LOAD;
    long LENGTH;
  };

  PAYLOAD_TYPE *PAYLOAD;
  SEMAPHORE BUFFER_READ_SEMAPHORE;
  SEMAPHORE BUFFER_WRITE_SEMAPHORE;
  MUTEX BUFFER_MUTEX;

public:
  APEX_BUFFER_TYPE(BUFFER_NAME_TYPE NAME, BUFFER_ID_TYPE ID,
                   MESSAGE_SIZE_TYPE size, MESSAGE_RANGE_TYPE MAX_NB_MESSAGE);
  ~APEX_BUFFER_TYPE();
  void SEND_MESSAGE(/*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
                    /* by reference */ /*in */ MESSAGE_SIZE_TYPE LENGTH,
                    /*in */ SYSTEM_TIME_TYPE TIME_OUT,
                    /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  void RECEIVE_MESSAGE(/*in */ SYSTEM_TIME_TYPE TIME_OUT,
                       /*out*/ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
                       /*out*/ MESSAGE_SIZE_TYPE *LENGTH,
                       /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  void GET_BUFFER_STATUS(/*out*/ BUFFER_STATUS_TYPE *BUFFER_STATUS,
                         /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  std::string GET_NAME() { return this->BUFFER_NAME; }
};
}
#endif // ACM_APEX_BUFFER_H
