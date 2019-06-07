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

#ifndef _ACM_QUEUE_CHANNEL_H
#define _ACM_QUEUE_CHANNEL_H
#include <ACM/INTERNAL/QUEUE_PORT.h>
#include <ACM/INTERNAL/SAMPLING_CHANNEL.h>
#include <ACM/USER/ACM_EXCEPTION.h>
#include <ACM/USER/ACM_LOGGER.h>
#include <ACM/USER/ACM_UTILITIES.h>
#include <ACM/USER/APEX_INTERFACE.h>
#include <ACM/USER/APEX_TYPES.h>

using namespace ACM_USER;
namespace ACM_INTERNAL {

class QUEUE_CHANNEL {
private:
  std::string CHANNEL_NAME;
  MESSAGE_QUEUE_TYPE *SOURCE_QUEUE;
  bool INITIALIZED;
  char *INTERNAL_BUFFER;
  // char buffer[SYSTEM_LIMIT_MAX_SAMPLING_MESSAGE_SIZE];
  MESSAGE_SIZE_TYPE MAX_SIZEOFMESSAGE;
  std::vector<MESSAGE_QUEUE_TYPE *> DESTINATION_QUEUE_VECTOR;
  // socket descriptions
  UDPSocket *sender_mySocket;
  UDPSocket *recv_mySocket;
  unsigned int MAX_SOCKET_MESSAGE;
  std::vector<DESTINATION_URL> destination_url_vector;

  COMMUNICATION_TYPE_ENUM COMMUNICATION_TYPE;

public:
  void SET_MAX_SIZE_OF_MESSAGE(MESSAGE_SIZE_TYPE data,
                               RETURN_CODE_TYPE *RETURN_CODE);
  MESSAGE_SIZE_TYPE GET_MAX_SIZE_OF_MESSAGE(RETURN_CODE_TYPE *RETURN_CODE);
  void SET_MAX_SOCKET_MESSAGE(unsigned int data, RETURN_CODE_TYPE *RETURN_CODE);
  COMMUNICATION_TYPE_ENUM GET_COMMUNICATION_TYPE() {
    return this->COMMUNICATION_TYPE;
  }
  void SET_COMMUNICATION_TYPE_ENUM(COMMUNICATION_TYPE_ENUM data,
                                   RETURN_CODE_TYPE *RETURN_CODE,
                                   unsigned int srcportNum = 0);
  void ADD_DESTINATION_URL(const std::string &nodename,
                           const unsigned short &port_number,
                           RETURN_CODE_TYPE *RETURN_CODE);

  QUEUE_CHANNEL(std::string NAME,
                COMMUNICATION_TYPE_ENUM COMMUNICATION_TYPE_VAL = INTRA_MODULE,
                unsigned int srcportNumber = 0);
  QUEUE_CHANNEL(COMMUNICATION_TYPE_ENUM COMMUNICATION_TYPE_VAL = INTRA_MODULE,
                unsigned int srcportNumber = 0);
  QUEUE_CHANNEL(const QUEUE_CHANNEL &q);

  void SET_NAME(std::string data) { CHANNEL_NAME = data; }
  std::string GET_NAME() { return this->CHANNEL_NAME; }

  std::string GET_SOURCE_NAME() {
    if (this->SOURCE_QUEUE != 0)
      return this->SOURCE_QUEUE->GET_NAME();
    else if (this->recv_mySocket != 0) {
      char buffer[30];
      snprintf(buffer, 30, "InterModulePort:%d",
               this->recv_mySocket->getLocalPort());
      return buffer;
    } else
      return ("");
  }

  std::string GET_DESTINATION_NAMES() {
    std::string temp;

    for (unsigned int i = 0; i < DESTINATION_QUEUE_VECTOR.size(); i++)
      if (DESTINATION_QUEUE_VECTOR[i] != 0)
        temp.append(DESTINATION_QUEUE_VECTOR[i]->GET_NAME() + ":");

    char buffer[30];
    for (unsigned int i = 0; i < destination_url_vector.size(); i++) {
      //	char buffer[30];
      temp.append("interModule:");
      temp.append(destination_url_vector[i].node_name);
      snprintf(buffer, 30, ":%d:", destination_url_vector[i].port);
      temp.append(buffer);
    }

    return temp;
  }

  ~QUEUE_CHANNEL();
  void SET_INITIALIZED() { this->INITIALIZED = true; };

  bool GET_INITIALIZED() { return this->INITIALIZED; }
  void ADD_SOURCE_QUEUE(std::string Name, MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
                        MESSAGE_RANGE_TYPE MAX_NB_MESSAGE,
                        RETURN_CODE_TYPE *RETURN_CODE);
  void ADD_DESTINATION_QUEUE(std::string Name,
                             MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
                             MESSAGE_RANGE_TYPE MAX_NB_MESSAGE,
                             RETURN_CODE_TYPE *RETURN_CODE);
  void MOVE_MESSAGES(RETURN_CODE_TYPE *RETURN_CODE);

  void SET_SOCKET_BLOCKING() {
    if (sender_mySocket != 0)
      sender_mySocket->make_blocking();

    if (recv_mySocket != 0)
      recv_mySocket->make_blocking();
  }
  void SET_SOCKET_NON_BLOCKING() {
    if (sender_mySocket != 0)
      sender_mySocket->make_non_blocking();

    if (recv_mySocket != 0)
      recv_mySocket->make_non_blocking();
  }
};
}
#endif //_ACM_QUEUE_HELPER_HS
