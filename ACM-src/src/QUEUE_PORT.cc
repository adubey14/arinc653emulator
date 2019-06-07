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

#ifndef _ACM_QUEUE_HELPER_CC
#define _ACM_QUEUE_HELPER_CC
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

namespace ACM_INTERNAL {
MESSAGE_QUEUE_TYPE::MESSAGE_QUEUE_TYPE(std::string Name,
                                       MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
                                       MESSAGE_RANGE_TYPE MAX_NB_MESSAGE,
                                       DIRECTION_TYPE PORT_DIRECTION,
                                       RETURN_CODE_TYPE *RETURN_CODE, int FLAGS)
    : ref(1), Q_NAME(Name), Q_MAX_MESSAGE_SIZE(MAX_MESSAGE_SIZE),
      Q_MAX_NB_MESSAGE(MAX_NB_MESSAGE), Q_PORT_DIRECTION(PORT_DIRECTION),
      READY_OR_NOT(false), INTERNAL_BUFFER(0) {

  REAL_PLATFORM_NAME = std::string("/") + Q_NAME;
  // REAL_PLATFORM_NAME=Q_NAME;

  *RETURN_CODE = NO_ERROR;
  struct mq_attr attr;
  attr.mq_maxmsg = Q_MAX_NB_MESSAGE;

  attr.mq_msgsize = Q_MAX_MESSAGE_SIZE;
  attr.mq_flags = 0;
  switch (Q_PORT_DIRECTION) {
  case ACM_USER::PORT_SOURCE: {
    LOGGER(INFO, ":%s:PORT_SOURCE:Qcreator\n", REAL_PLATFORM_NAME.c_str());
    this->Q_MQDES = mq_open(REAL_PLATFORM_NAME.c_str(),
                            O_RDWR /*O_WRONLY*/ | FLAGS, 0664, &attr);
    LOGGER(INFO, "%d,%s\n", errno, strerror(errno));
    break;
  }
  case ACM_USER::PORT_DEST: {
    LOGGER(INFO, ":%s:PORT_DEST:Qcreator\n", REAL_PLATFORM_NAME.c_str());
    this->Q_MQDES = mq_open(REAL_PLATFORM_NAME.c_str(),
                            /*O_RDONLY*/ O_RDWR | FLAGS, 0664, &attr);
    LOGGER(INFO, "%d,%s\n", errno, strerror(errno));
    INTERNAL_BUFFER = new char[this->Q_MAX_MESSAGE_SIZE];
    break;
  }
  case ACM_USER::PORT_SOURCE_DEST: {
    LOGGER(INFO, ":%s:PORT_SOURCE_DEST:Qcreator\n", REAL_PLATFORM_NAME.c_str());
    this->Q_MQDES =
        mq_open(REAL_PLATFORM_NAME.c_str(), O_RDWR | FLAGS, 0664, &attr);
    LOGGER(INFO, "%d,%s\n", errno, strerror(errno));
    INTERNAL_BUFFER = new char[this->Q_MAX_MESSAGE_SIZE];
    break;
  }
  }
  if (this->Q_MQDES == -1) {
    if (errno == EEXIST) {
      *RETURN_CODE = NO_ACTION; // Queue exists
      LOGGER(HMEVENT, "Queue %s, Already exists", this->Q_NAME.c_str());
    } else {
      *RETURN_CODE = INVALID_CONFIG;
      LOGGER(HMEVENT, "Queue %s, Not created. There was some problem. RealName "
                      "%s. Error %s, mq_maxmsg %d attr.mq_msgsize %d, ",
             this->Q_NAME.c_str(), REAL_PLATFORM_NAME.c_str(), strerror(errno),
             attr.mq_maxmsg, attr.mq_msgsize);
    }
  }

  if (*RETURN_CODE == NO_ERROR)
    READY_OR_NOT = true;
}
MESSAGE_QUEUE_TYPE::~MESSAGE_QUEUE_TYPE() {
  if (!READY_OR_NOT) {

    return;
  }
  // Destroy the queue
  if (this->INTERNAL_BUFFER != 0)
    delete[](INTERNAL_BUFFER);

  mq_unlink(this->REAL_PLATFORM_NAME.c_str());
  mq_close(this->Q_MQDES);
}

void MESSAGE_QUEUE_TYPE::SEND_MESSAGE(MESSAGE_ADDR_TYPE msg_ptr,
                                      MESSAGE_SIZE_TYPE LENGTH,
                                      SYSTEM_TIME_TYPE TIME_OUT,
                                      RETURN_CODE_TYPE *RETURN_CODE) {
  if (!READY_OR_NOT) {
    *RETURN_CODE = INVALID_MODE;
    return;
  }
  *RETURN_CODE = NO_ERROR;
  if (this->Q_PORT_DIRECTION == ACM_USER::PORT_DEST) {
    LOGGER(
        ACM_USER::ERROR,
        "Queue %s is to be used only for receiving message. Cannot write to it",
        this->Q_NAME.c_str());
    *RETURN_CODE = INVALID_MODE;
    return;
  }
  if (TIME_OUT == 0 && MESSAGE_LIMIT_EXCEEDED()) {
    *RETURN_CODE = TIMED_OUT;
    LOGGER(INFO, "TIME_OUT==0 and message limit exceeded");
    return;
  }
  if (TIME_OUT > MAX_TIME_OUT && TIME_OUT != INFINITE_TIME_VALUE) {
    *RETURN_CODE = INVALID_PARAM;
    LOGGER(ACM_USER::ERROR, "TIME_OUT>MAX_TIME_OUT");
    return;
  }
  if (LENGTH > this->Q_MAX_MESSAGE_SIZE) {
    *RETURN_CODE = INVALID_CONFIG;
    return;
  }

  // AUTO_LOCK l (MyMutex);

  if (TIME_OUT == INFINITE_TIME_VALUE) {
    int result = mq_send(this->Q_MQDES, (char *)msg_ptr, LENGTH,
                         POSIX_Q_MESSAGE_DEFAULT_PRIORITY);
    if (result == -1) {
      LOGGER(HMEVENT, "ERROR While sending to queue %s", this->Q_NAME.c_str());
      *RETURN_CODE = NO_ACTION;
      return;
    }
  } else {
    SYSTEM_TIMESPEC curr = currentTime();
    SYSTEM_TIMESPEC absolute = addTimeNano(curr, TIME_OUT);
    LOGGER(INFO, "*****Calling timed send on %s*******", this->Q_NAME.c_str());
    int result = mq_timedsend(this->Q_MQDES, (char *)msg_ptr, LENGTH,
                              POSIX_Q_MESSAGE_DEFAULT_PRIORITY, &absolute);

    LOGGER(INFO, "*****Result of timed send on %s %d %s*******",
           this->Q_NAME.c_str(), result, strerror(errno));
    if (result == -1) {
      if (errno == ETIMEDOUT) {
        *RETURN_CODE = TIMED_OUT;
        return;
      } else {
        *RETURN_CODE = NO_ACTION;
        return;
      }
    }
  }
}

void MESSAGE_QUEUE_TYPE::SEND_MESSAGE_FROM_BUFFER_READ_FROM_SOCKET(
    MESSAGE_ADDR_TYPE msg_ptr, MESSAGE_SIZE_TYPE LENGTH,
    SYSTEM_TIME_TYPE TIME_OUT, RETURN_CODE_TYPE *RETURN_CODE) {
  if (!READY_OR_NOT) {
    *RETURN_CODE = INVALID_MODE;
    return;
  }
  *RETURN_CODE = NO_ERROR;
  if (this->Q_PORT_DIRECTION == ACM_USER::PORT_SOURCE) {
    LOGGER(ACM_USER::ERROR, "Queue %s is to be used only for sending message. "
                            "Cannot write to it from a socket",
           this->Q_NAME.c_str());
    *RETURN_CODE = INVALID_MODE;
    return;
  }
  if (TIME_OUT == 0 && MESSAGE_LIMIT_EXCEEDED()) {
    *RETURN_CODE = TIMED_OUT;
    LOGGER(INFO, "TIME_OUT==0 and message limit exceeded");
    return;
  }
  if (TIME_OUT > MAX_TIME_OUT && TIME_OUT != INFINITE_TIME_VALUE) {
    *RETURN_CODE = INVALID_PARAM;
    LOGGER(ACM_USER::ERROR, "TIME_OUT>MAX_TIME_OUT");
    return;
  }
  if (LENGTH > this->Q_MAX_MESSAGE_SIZE) {
    *RETURN_CODE = INVALID_CONFIG;
    return;
  }

  // AUTO_LOCK l (MyMutex);

  if (TIME_OUT == INFINITE_TIME_VALUE) {
    int result = mq_send(this->Q_MQDES, (char *)msg_ptr, LENGTH,
                         POSIX_Q_MESSAGE_DEFAULT_PRIORITY);
    if (result == -1) {
      LOGGER(HMEVENT, "ERROR While sending to queue %s", this->Q_NAME.c_str());
      *RETURN_CODE = NO_ACTION;
      return;
    }
  } else {
    SYSTEM_TIMESPEC curr = currentTime();
    SYSTEM_TIMESPEC absolute = addTimeNano(curr, TIME_OUT);
    LOGGER(INFO, "*****Calling timed send on %s*******", this->Q_NAME.c_str());
    int result = mq_timedsend(this->Q_MQDES, (char *)msg_ptr, LENGTH,
                              POSIX_Q_MESSAGE_DEFAULT_PRIORITY, &absolute);

    LOGGER(INFO, "*****Result of timed send on %s %d %s*******",
           this->Q_NAME.c_str(), result, strerror(errno));
    if (result == -1) {
      if (errno == ETIMEDOUT) {
        *RETURN_CODE = TIMED_OUT;
        return;
      } else {
        *RETURN_CODE = NO_ACTION;
        return;
      }
    }
  }
}

void MESSAGE_QUEUE_TYPE::RECEIVE_MESSAGE(MESSAGE_ADDR_TYPE msg_ptr,
                                         MESSAGE_SIZE_TYPE *LENGTH,
                                         SYSTEM_TIME_TYPE TIME_OUT,
                                         RETURN_CODE_TYPE *RETURN_CODE) {
  if (!READY_OR_NOT) {
    *RETURN_CODE = INVALID_MODE;
    return;
  }
  *RETURN_CODE = NO_ERROR;
  if (this->Q_PORT_DIRECTION == ACM_USER::PORT_SOURCE) {
    LOGGER(
        ACM_USER::ERROR,
        "Queue %s is to be used only for sending message. Cannot read from it",
        this->Q_NAME.c_str());
    *RETURN_CODE = INVALID_MODE;
    return;
  }

  if (TIME_OUT > MAX_TIME_OUT && TIME_OUT != INFINITE_TIME_VALUE) {
    *RETURN_CODE = INVALID_PARAM;
    LOGGER(ACM_USER::ERROR, "TIME_OUT>MAX_TIME_OUT");
    return;
  }
  if (TIME_OUT == 0 && IS_EMPTY()) {
    *RETURN_CODE = NOT_AVAILABLE;
    LOGGER(ACM_USER::INFO, "TIME_OUT==0");
    return;
  }

  // AUTO_LOCK l (MyMutex);

  if (TIME_OUT == INFINITE_TIME_VALUE) {
    // BLOCK
    //*LENGTH=mq_receive(this->Q_MQDES,(char*)msg_ptr,this->Q_MAX_MESSAGE_SIZE,POSIX_Q_MESSAGE_DEFAULT_PRIORITY);
    *LENGTH =
        mq_receive(this->Q_MQDES, (char *)&(this->INTERNAL_BUFFER[0]),
                   this->Q_MAX_MESSAGE_SIZE, POSIX_Q_MESSAGE_DEFAULT_PRIORITY);
    if (*LENGTH == -1) {
      *RETURN_CODE = NO_ACTION;
      LOGGER(HMEVENT, "ERROR While reading from queue %s",
             this->Q_NAME.c_str());
      return;
    }

    memcpy(msg_ptr, &(this->INTERNAL_BUFFER[0]), *LENGTH);

  } else {
    SYSTEM_TIMESPEC curr = currentTime();
    SYSTEM_TIMESPEC absolute = addTimeNano(curr, TIME_OUT);
    // this->INTERNAL_BUFFER[0]='t';
    // std::cout<<this->INTERNAL_BUFFER[0]<<std::endl;

    *LENGTH = mq_timedreceive(
        this->Q_MQDES, (char *)&(this->INTERNAL_BUFFER[0]),
        this->Q_MAX_MESSAGE_SIZE, POSIX_Q_MESSAGE_DEFAULT_PRIORITY, &absolute);
    if (*LENGTH == -1) {
      if (errno == ETIMEDOUT) {
        *RETURN_CODE = TIMED_OUT;
        return;
      } else {
        *RETURN_CODE = NO_ACTION;
        return;
      }
    }
    memcpy(msg_ptr, &(this->INTERNAL_BUFFER[0]), *LENGTH);
  }
}

void MESSAGE_QUEUE_TYPE::GET_QUEUING_PORT_STATUS(
    QUEUING_PORT_STATUS_TYPE *QUEUING_PORT_STATUS,
    RETURN_CODE_TYPE *RETURN_CODE) {
  if (!READY_OR_NOT) {
    *RETURN_CODE = INVALID_MODE;
    return;
  }
  *RETURN_CODE = NO_ERROR;

  //	AUTO_LOCK l (MyMutex);

  struct mq_attr mq_stat;
  if (mq_getattr(this->Q_MQDES, &mq_stat) == -1) {
    *RETURN_CODE = NO_ACTION;
    LOGGER(ERROR, "ERROR in mq_getattr(this->Q_MQDES,&mqstat) for queue %s",
           this->Q_NAME.c_str());
    return;
  }
  QUEUING_PORT_STATUS->NB_MESSAGE = mq_stat.mq_curmsgs;
  QUEUING_PORT_STATUS->MAX_NB_MESSAGE = this->Q_MAX_NB_MESSAGE;
  QUEUING_PORT_STATUS->MAX_MESSAGE_SIZE = this->Q_MAX_MESSAGE_SIZE;
  if (this->Q_PORT_DIRECTION == ACM_USER::PORT_SOURCE) {
    QUEUING_PORT_STATUS->PORT_DIRECTION = SOURCE;
    QUEUING_PORT_STATUS->WAITING_PROCESSES = 0;
    // This is an implementation restrictions . POSIX does not support
    /*	long mq_sendwait
    The number of processes currently waiting to send a message. This field was
    eliminated from the POSIX standard after draft 9, but has been resurrected
    for the QNX implementation. A nonzero value in this field implies that the
    queue is full.
    long mq_recvwait
    The number of processes currently waiting to receive a message. Like
    mq_sendwait, this field was resurrected for the QNX implementation. A
    nonzero value in this field implies that the queue is empty.
    */
    return;
  }
  if (this->Q_PORT_DIRECTION == ACM_USER::PORT_DEST) {
    QUEUING_PORT_STATUS->PORT_DIRECTION = DESTINATION;
    QUEUING_PORT_STATUS->WAITING_PROCESSES = 0;
    return;
  }

  if (this->Q_PORT_DIRECTION == ACM_USER::PORT_SOURCE_DEST) {
    QUEUING_PORT_STATUS->PORT_DIRECTION = DESTINATION;
    QUEUING_PORT_STATUS->WAITING_PROCESSES = 0;
    return;
  }
}
}

#endif //_ACM_QUEUE_HELPER_CC
