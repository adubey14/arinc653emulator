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

#ifndef ACM_APEX_BLACKBOARD_H
#define ACM_APEX_BLACKBOARD_H

#include <ACM/INTERNAL/ACM_THREAD_SUPPORT.h>

namespace ACM_INTERNAL {
class APEX;
}
using namespace ACM_USER;
namespace ACM_INTERNAL {
//! An internal class used that provides facilities to create and manage
//! blackboards.
class APEX_BLACKBOARD_TYPE {
  friend class ACM_INTERNAL::APEX;

public:
  EVENT_ID_TYPE BLACKBOARD_EVENT_ID;
  std::string BLACKBOARD_NAME;
  BLACKBOARD_ID_TYPE BLACKBOARD_ID;
  MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE;
  MESSAGE_SIZE_TYPE CURRENT_MESSAGE_SIZE;
  MUTEX BLACKBOARD_MUTEX;
  void *PAYLOAD;
  RW_LOCK rw_lock;

  APEX_BLACKBOARD_TYPE(BLACKBOARD_NAME_TYPE NAME, BLACKBOARD_ID_TYPE ID,
                       MESSAGE_SIZE_TYPE size);

  void ERASE_BLACKBOARD(RETURN_CODE_TYPE *RETURN_CODE);

  void WRITE_ON_BLACKBOARD(MESSAGE_ADDR_TYPE MESSAGE_ADDR,
                           MESSAGE_SIZE_TYPE LENGTH,
                           RETURN_CODE_TYPE *RETURN_CODE);
  void READ_FROM_BLACKBOARD(MESSAGE_ADDR_TYPE MESSAGE_ADDR,
                            MESSAGE_SIZE_TYPE *LENGTH,
                            RETURN_CODE_TYPE *RETURN_CODE);

public:
  ~APEX_BLACKBOARD_TYPE();
};
}
#endif // ACM_APEX_BLACKBOARD_H
