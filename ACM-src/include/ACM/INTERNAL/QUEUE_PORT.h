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

#ifndef _ACM_QUEUE_HELPER_H
#define _ACM_QUEUE_HELPER_H
#include <ACM/USER/ACM_EXCEPTION.h>
#include <ACM/USER/ACM_LOGGER.h>
#include <ACM/USER/ACM_UTILITIES.h>
#include <ACM/USER/APEX_INTERFACE.h>
#include <ACM/USER/APEX_TYPES.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdlib.h>
#include <string>
#include <sys/mman.h>

#include <string.h>

using namespace ACM_USER;
namespace ACM_INTERNAL {
class MESSAGE_QUEUE_TYPE {

public:
  int ref;

public:
  std::string Q_NAME;
  MESSAGE_SIZE_TYPE Q_MAX_MESSAGE_SIZE;
  MESSAGE_RANGE_TYPE Q_MAX_NB_MESSAGE;
  DIRECTION_TYPE Q_PORT_DIRECTION;

private:
  bool READY_OR_NOT;
  char *INTERNAL_BUFFER;
  std::string REAL_PLATFORM_NAME;

  //		MUTEX MyMutex;

public:
  mqd_t Q_MQDES; //! descriptor

  bool MESSAGE_LIMIT_EXCEEDED() {

    struct mq_attr mq_stat;
    if (mq_getattr(this->Q_MQDES, &mq_stat) == -1) {
      LOGGER(ERROR, "ERROR in mq_getattr(this->Q_MQDES,&mqstat) for queue %s",
             this->Q_NAME.c_str());
      return false;
    }
    return (mq_stat.mq_curmsgs >= this->Q_MAX_NB_MESSAGE);
  }

  bool IS_EMPTY() {

    struct mq_attr mq_stat;
    if (mq_getattr(this->Q_MQDES, &mq_stat) == -1) {
      LOGGER(ERROR, "ERROR in mq_getattr(this->Q_MQDES,&mqstat) for queue %s",
             this->Q_NAME.c_str());
      return false;
    }
    return (mq_stat.mq_curmsgs == 0);
  }

  int NUMBER_OF_MESSAGES() {

    struct mq_attr mq_stat;
    if (mq_getattr(this->Q_MQDES, &mq_stat) == -1) {
      LOGGER(ERROR, "ERROR in mq_getattr(this->Q_MQDES,&mqstat) for queue %s",
             this->Q_NAME.c_str());
      return false;
    }
    return mq_stat.mq_curmsgs;
  }

public:
  MESSAGE_QUEUE_TYPE(std::string Name, MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
                     MESSAGE_RANGE_TYPE MAX_NB_MESSAGE,
                     DIRECTION_TYPE PORT_DIRECTION,
                     RETURN_CODE_TYPE *RETURN_CODE, int FLAGS = O_CREAT);
  ~MESSAGE_QUEUE_TYPE();
  mqd_t GET_QUEUING_PORT_ID() const { return this->Q_MQDES; }
  std::string GET_NAME() { return this->Q_NAME; }
  void SEND_MESSAGE(MESSAGE_ADDR_TYPE msg_ptr, MESSAGE_SIZE_TYPE LENGTH,
                    SYSTEM_TIME_TYPE TIME_OUT, RETURN_CODE_TYPE *RETURN_CODE);

  void SEND_MESSAGE_FROM_BUFFER_READ_FROM_SOCKET(MESSAGE_ADDR_TYPE msg_ptr,
                                                 MESSAGE_SIZE_TYPE LENGTH,
                                                 SYSTEM_TIME_TYPE TIME_OUT,
                                                 RETURN_CODE_TYPE *RETURN_CODE);

  void RECEIVE_MESSAGE(MESSAGE_ADDR_TYPE msg_ptr, MESSAGE_SIZE_TYPE *LENGTH,
                       SYSTEM_TIME_TYPE TIME_OUT,
                       RETURN_CODE_TYPE *RETURN_CODE);
  void GET_QUEUING_PORT_STATUS(QUEUING_PORT_STATUS_TYPE *QUEUING_PORT_STATUS,
                               RETURN_CODE_TYPE *RETURN_CODE);
};
}
#endif //_ACM_QUEUE_HELPER_HS
