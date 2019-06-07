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

#ifndef _ACM_Exception_h
#define _ACM_Exception_h
#include <ACM/USER/APEX_TYPES.h>
#include <string>

namespace ACM_USER {

//! This is a base class for all ACM Exceptions
/*!
The way we will use it is that user will define their own exceptions that will
be derived from this.
Q: How to convert CORBA Exceptions to ACM_BASE_EXCEPTION
Even a CORBA USER Exception will be created such that it is derived from this
*/
class ACM_BASE_EXCEPTION_TYPE {
protected:
  std::string Message;

public:
  ACM_BASE_EXCEPTION_TYPE() : Message("No Message") {}

  ACM_BASE_EXCEPTION_TYPE(RETURN_CODE_TYPE RETURN_CODE) {
    switch (RETURN_CODE) {
    case NO_ERROR:
      Message = "NO_ERROR";
      break;
    case NO_ACTION:
      Message = "NO_ACTION";
      break;
    case NOT_AVAILABLE:
      Message = "NOT_AVAILABLE";
      break;
    case INVALID_PARAM:
      Message = "INVALID_PARAM";
      break;
    case INVALID_CONFIG:
      Message = "INVALID_CONFIG";
      break;
    case INVALID_MODE:
      Message = "INVALID_MODE";
      break;
    case TIMED_OUT:
      Message = "TIMED_OUT";
      break;
    default:
      Message = "NO_MESSAGE";
      break;
    }
  }

  ACM_BASE_EXCEPTION_TYPE(const ACM_BASE_EXCEPTION_TYPE &ex)
      : Message(ex.Message) {}

  ACM_BASE_EXCEPTION_TYPE(const std::string &_Message) : Message(_Message) {}
  virtual void setMessage(const std::string &_Message) { Message = _Message; }
  virtual std::string getMessage() const { return this->Message; }

  virtual ~ACM_BASE_EXCEPTION_TYPE() {}

  virtual ACM_BASE_EXCEPTION_TYPE *createCopy() {
    return new ACM_BASE_EXCEPTION_TYPE(this->Message);
  }
};

//! Any timing related violation
class ACM_TIMING_EXCEPTION_TYPE : public ACM_BASE_EXCEPTION_TYPE {
public:
  ACM_TIMING_EXCEPTION_TYPE() {}
  ACM_TIMING_EXCEPTION_TYPE(std::string Message)
      : ACM_BASE_EXCEPTION_TYPE(Message) {}
  virtual ~ACM_TIMING_EXCEPTION_TYPE() {}
  virtual ACM_BASE_EXCEPTION_TYPE *createCopy() {
    return new ACM_TIMING_EXCEPTION_TYPE(this->Message);
  }
};

typedef ACM_BASE_EXCEPTION_TYPE *ACM_Base_Exception_ptr;
typedef ACM_BASE_EXCEPTION_TYPE &ACM_Base_Exception_ref;

} // END ACM

#endif
