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

#ifndef ACM_APEX_EVENT_H
#define ACM_APEX_EVENT_H

#include <ACM/INTERNAL/ACM_THREAD_SUPPORT.h>

namespace ACM_INTERNAL {
class APEX;
}
using namespace ACM_USER;
namespace ACM_INTERNAL {
//! Internal class for facilitating management of EVENTS
class APEX_EVENT_TYPE {
  friend class ACM_INTERNAL::APEX;

public:
  EVENT_STATE_TYPE EVENT_STATE;
  std::string EVENT_NAME;
  MUTEX *EVENT_CONDITION_VARIABLE_MUTEX;
  COND_VAR *EVENT_CONDITION_VARIABLE;
  APEX_UNSIGNED APEX_UNSIGNED_COUNTER;
  MUTEX APEX_UNSIGNED_COUNTER_MUTEX;

  APEX_EVENT_TYPE(std::string name, MUTEX *useMutex = 0, bool locked = false,
                  MUTEX::Attribute attr = MUTEX::RECURSIVE);
  bool CreatedMutex;

public:
  std::string GET_NAME();
  static APEX_EVENT_TYPE *
  CREATE_EVENT(std::string name, MUTEX *useMutex = 0, bool locked = false,
               MUTEX::Attribute attr = MUTEX::RECURSIVE);
  //	static APEX_EVENT_TYPE* CREATE_EVENT(std::string name, MUTEX* useMutex,
  //bool locked = false, MUTEX::Attribute attr = MUTEX::RECURSIVE);

  void SET_EVENT(RETURN_CODE_TYPE *RETURN_CODE);
  void RESET_EVENT(RETURN_CODE_TYPE *RETURN_CODE);
  void WAIT_EVENT(RETURN_CODE_TYPE *RETURN_CODE);
  void WAIT_EVENT(SYSTEM_TIME_TYPE TIME_OUT, RETURN_CODE_TYPE *RETURN_CODE);
  void WAIT_EVENT(SYSTEM_TIMESPEC timeout, RETURN_CODE_TYPE *RETURN_CODE);
  void WAIT_AND_LOCK_EVENT(SYSTEM_TIMESPEC TIME_OUT,
                           RETURN_CODE_TYPE *RETURN_CODE);

  APEX_UNSIGNED GET_COUNT();
  ~APEX_EVENT_TYPE();

  EVENT_STATE_TYPE GET_STATE();

  void LOCK_EVENT_MUTEX();
  void UNLOCK_EVENT_MUTEX();
};
}
#endif // ACM_APEX_EVENT_H
