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

#ifndef _ACM_SAMPLING_PORT_HELPER_H
#define _ACM_SAMPLING_PORT_HELPER_H

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

using namespace ACM_USER;

namespace ACM_INTERNAL {
class SAMPLING_CHANNEL;

class SAMPLING_PORT_HELPER {
  friend class SAMPLING_CHANNEL;

private:
  struct SAMPLING_DATA {
    SYSTEM_TIME_TYPE ABSOLUTE_TIME_WRITTEN;
    MESSAGE_SIZE_TYPE MESSAGE_SIZE;
    char data[SYSTEM_LIMIT_MAX_SAMPLING_MESSAGE_SIZE];
  };

  MUTEX myMutex;

public:
  std::string S_NAME;
  MESSAGE_SIZE_TYPE S_MAX_MESSAGE_SIZE;
  DIRECTION_TYPE S_PORT_DIRECTION;
  SYSTEM_TIME_TYPE S_REFRESH_PERIOD;
  int SAMPLING_DESCRIPTOR; //!<descriptor
  MESSAGE_SIZE_TYPE S_REAL_MAX_MESSAGE_SIZE;
  MESSAGE_SIZE_TYPE S_EXTRA_MEMORY_REQUIRED;

private:
  bool READY_OR_NOT;

public:
  VALIDITY_TYPE LAST_VALIDITY;

  std::string GET_NAME() { return this->S_NAME; }

public:
  ~SAMPLING_PORT_HELPER();
  SAMPLING_PORT_HELPER(std::string Name, MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
                       DIRECTION_TYPE PORT_DIRECTION,
                       SYSTEM_TIME_TYPE REFRESH_PERIOD,
                       RETURN_CODE_TYPE *RETURN_CODE, int FLAGS = O_CREAT);

  void WRITE_SAMPLING_MESSAGE(MESSAGE_ADDR_TYPE msg_ptr,
                              MESSAGE_SIZE_TYPE LENGTH,
                              RETURN_CODE_TYPE *RETURN_CODE);

  void READ_CONTENTS_TO_DATA_BUFFER(MESSAGE_ADDR_TYPE msg_ptr,
                                    MESSAGE_SIZE_TYPE MAX_LENGTH,
                                    MESSAGE_SIZE_TYPE *READ_LENGTH,
                                    RETURN_CODE_TYPE *RETURN_CODE);

  void READ_SAMPLING_MESSAGE(MESSAGE_ADDR_TYPE msg_ptr,
                             MESSAGE_SIZE_TYPE *LENGTH, VALIDITY_TYPE *VALIDITY,
                             RETURN_CODE_TYPE *RETURN_CODE);
  void GET_SAMPLING_PORT_STATUS(SAMPLING_PORT_STATUS_TYPE *SAMPLING_PORT_STATUS,
                                RETURN_CODE_TYPE *RETURN_CODE);
  void COPY_CONTENTS_VERBATIM(SAMPLING_PORT_HELPER *DESTINATION,
                              RETURN_CODE_TYPE *RETURN_CODE) {
    if (this->S_REAL_MAX_MESSAGE_SIZE == DESTINATION->S_REAL_MAX_MESSAGE_SIZE) {
      DESTINATION->shared_memory->ABSOLUTE_TIME_WRITTEN =
          this->shared_memory->ABSOLUTE_TIME_WRITTEN;
      DESTINATION->shared_memory->MESSAGE_SIZE =
          this->shared_memory->MESSAGE_SIZE;
      memcpy(&(DESTINATION->shared_memory->data[0]), &(shared_memory->data[0]),
             this->shared_memory->MESSAGE_SIZE);
      *RETURN_CODE = NO_ERROR;
      return;
    } else {
      *RETURN_CODE = INVALID_CONFIG;
      LOGGER(ERROR, "CANNOT COPY MAX MESSAGE SIZE of source: %s  does not "
                    "match destination %s",
             this->S_NAME.c_str(), DESTINATION->S_NAME.c_str());
      return;
    }
  }

  void WRITE_CONTENTS_FROM_DATA_BUFFER(MESSAGE_ADDR_TYPE msg_ptr,
                                       MESSAGE_SIZE_TYPE LENGTH,
                                       RETURN_CODE_TYPE *RETURN_CODE);

private:
  SAMPLING_DATA *shared_memory;
};
///////////////////////////////////////
}
#endif //_ACM_SAMPLING_PORT_HELPER_H
