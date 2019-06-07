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

#ifndef _ACM_SAMPLING_PORT_CHANNEL_CPP
#define _ACM_SAMPLING_PORT_CHANNEL_CPP

#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>

using namespace ACM_USER;

namespace ACM_INTERNAL {

SAMPLING_CHANNEL::SAMPLING_CHANNEL(
    std::string NAME, COMMUNICATION_TYPE_ENUM COMMUNICATION_TYPE_VAL,
    unsigned int srcportNumber)
    : CHANNEL_NAME(NAME), SOURCE_PORT(0), INITIALIZED(false),
      MAX_SIZE_OF_MESSAGE(0), sender_mySocket(0), recv_mySocket(0),
      MAX_SOCKET_MESSAGE(1), COMMUNICATION_TYPE(COMMUNICATION_TYPE_VAL) {
  switch (COMMUNICATION_TYPE) {
  case INTRA_MODULE:
    break;
  case INTER_MODULE_SRC:
    sender_mySocket = new UDPSocket();
    assert(sender_mySocket != 0);
    sender_mySocket->make_non_blocking();
    break;
  case INTER_MODULE_DST:
    recv_mySocket = new UDPSocket(srcportNumber);
    recv_mySocket->make_non_blocking();
    assert(recv_mySocket != 0);
    break;
  };
  LOGGER(INFO, "Channel %s Created", CHANNEL_NAME.c_str());
}

SAMPLING_CHANNEL::SAMPLING_CHANNEL(
    COMMUNICATION_TYPE_ENUM COMMUNICATION_TYPE_VAL, unsigned int srcportNumber)
    : CHANNEL_NAME(""), SOURCE_PORT(0), INITIALIZED(false),
      MAX_SIZE_OF_MESSAGE(0), sender_mySocket(0), recv_mySocket(0),
      MAX_SOCKET_MESSAGE(1), COMMUNICATION_TYPE(COMMUNICATION_TYPE_VAL) {
  switch (COMMUNICATION_TYPE) {
  case INTRA_MODULE:
    break;
  case INTER_MODULE_SRC:
    sender_mySocket = new UDPSocket();
    assert(sender_mySocket != 0);
    sender_mySocket->make_non_blocking();
    break;
  case INTER_MODULE_DST:
    recv_mySocket = new UDPSocket(srcportNumber);
    recv_mySocket->make_non_blocking();
    assert(recv_mySocket != 0);
    break;
  };
  LOGGER(INFO, "Channel %s Created", CHANNEL_NAME.c_str());
}

SAMPLING_CHANNEL::SAMPLING_CHANNEL(const SAMPLING_CHANNEL &my)
    : CHANNEL_NAME(my.CHANNEL_NAME), INITIALIZED(my.INITIALIZED),
      MAX_SIZE_OF_MESSAGE(my.MAX_SIZE_OF_MESSAGE),
      sender_mySocket(my.sender_mySocket), recv_mySocket(my.recv_mySocket),
      MAX_SOCKET_MESSAGE(my.MAX_SOCKET_MESSAGE),
      COMMUNICATION_TYPE(my.COMMUNICATION_TYPE) {

  if (my.sender_mySocket != 0)
    my.sender_mySocket->ref = my.sender_mySocket->ref + 1;
  if (my.recv_mySocket != 0)
    my.recv_mySocket->ref = my.recv_mySocket->ref + 1;

  LOGGER(INFO, "Channel %s Copied", CHANNEL_NAME.c_str());
}
void SAMPLING_CHANNEL::SET_MAX_SOCKET_MESSAGE(unsigned int data,
                                              RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  if (INITIALIZED) {
    LOGGER(ERROR, "from SET_MAX_SOCKET_MESSAGE: CHANNEL %s ALREADY INITIALIZED",
           CHANNEL_NAME.c_str());
    *RETURN_CODE = NO_ACTION;
    return;
  }

  MAX_SOCKET_MESSAGE = data;
}

void SAMPLING_CHANNEL::SET_COMMUNICATION_TYPE_ENUM(
    COMMUNICATION_TYPE_ENUM data, RETURN_CODE_TYPE *RETURN_CODE,
    unsigned int srcportNum) {
  *RETURN_CODE = NO_ERROR;
  if (INITIALIZED) {
    LOGGER(ERROR,
           "from SET_COMMUNICATION_TYPE_ENUM: CHANNEL %s ALREADY INITIALIZED",
           CHANNEL_NAME.c_str());
    *RETURN_CODE = NO_ACTION;
    return;
  }

  if (this->COMMUNICATION_TYPE != INTRA_MODULE) {
    LOGGER(ERROR, "from SET_COMMUNICATION_TYPE_ENUM: communication type "
                  "already set to %d, CHANNEL %s ",
           COMMUNICATION_TYPE, CHANNEL_NAME.c_str());
    *RETURN_CODE = NO_ACTION;
    return;
  }
  this->COMMUNICATION_TYPE = data;

  switch (COMMUNICATION_TYPE) {
  case INTRA_MODULE:
    break;
  case INTER_MODULE_SRC:
    sender_mySocket = new UDPSocket();
    assert(sender_mySocket != 0);
    sender_mySocket->make_non_blocking();
    break;
  case INTER_MODULE_DST:
    recv_mySocket = new UDPSocket(srcportNum);
    recv_mySocket->make_non_blocking();
    assert(recv_mySocket != 0);
    break;
  };
}

void SAMPLING_CHANNEL::SET_NAME(std::string data) { CHANNEL_NAME = data; }
std::string SAMPLING_CHANNEL::GET_NAME() { return this->CHANNEL_NAME; }
std::string SAMPLING_CHANNEL::GET_SOURCE_NAME() {
  if (this->SOURCE_PORT == 0)
    return ("");
  return this->SOURCE_PORT->GET_NAME();
}

std::string SAMPLING_CHANNEL::GET_DESTINATION_NAMES() {
  std::string temp;

  for (unsigned int i = 0; i < DESTINATION_PORT_VECTOR.size(); i++)
    if (DESTINATION_PORT_VECTOR[i] != 0)
      temp.append(DESTINATION_PORT_VECTOR[i]->GET_NAME() + ":");

  return temp;
}

SAMPLING_CHANNEL::~SAMPLING_CHANNEL() {
  if (SOURCE_PORT != 0)
    delete (SOURCE_PORT);
  for (unsigned int i = 0; i < this->DESTINATION_PORT_VECTOR.size(); i++) {
    if (DESTINATION_PORT_VECTOR[i] != 0)
      delete (DESTINATION_PORT_VECTOR[i]);
  }
  DESTINATION_PORT_VECTOR.clear();
  LOGGER(INFO, "Channel %s Destroyed", CHANNEL_NAME.c_str());

  if (sender_mySocket != 0) {
    sender_mySocket->ref = sender_mySocket->ref - 1;
    if (sender_mySocket->ref == 0)
      delete (sender_mySocket);
  }

  if (recv_mySocket != 0) {
    recv_mySocket->ref = recv_mySocket->ref - 1;
    if (recv_mySocket->ref == 0)
      delete (recv_mySocket);
  }
}

void SAMPLING_CHANNEL::ADD_DESTINATION_URL(const std::string &nodename,
                                           const unsigned short &port_number,
                                           RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  if (INITIALIZED) {
    LOGGER(ERROR, "CHANNEL %s ALREADY INITIALIZED", CHANNEL_NAME.c_str());
    *RETURN_CODE = NO_ACTION;
    return;
  }
  if (this->sender_mySocket == 0) {
    LOGGER(ERROR,
           "CHANNEL %s was not constructed to allow inter-module communication",
           CHANNEL_NAME.c_str());
    *RETURN_CODE = NOT_AVAILABLE;
    return;
  }
  DESTINATION_URL temp(nodename, port_number);
  this->destination_url_vector.push_back(temp);
  return;
}

void SAMPLING_CHANNEL::SET_MAX_SIZE_OF_MESSAGE(MESSAGE_SIZE_TYPE data,
                                               RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  if (INITIALIZED) {
    LOGGER(ERROR, "CHANNEL %s ALREADY INITIALIZED", CHANNEL_NAME.c_str());
    *RETURN_CODE = NO_ACTION;
    return;
  }

  if (COMMUNICATION_TYPE != INTER_MODULE_DST) {
    LOGGER(ERROR, "CHANNEL %s is configured for INTRA_MODULE or "
                  "INTER_MODULE_SRC. Add source_port for setting message size",
           CHANNEL_NAME.c_str());
    *RETURN_CODE = NOT_AVAILABLE;
    return;
  }
  this->MAX_SIZE_OF_MESSAGE = data;
  return;
}
MESSAGE_SIZE_TYPE
SAMPLING_CHANNEL::GET_MAX_SIZE_OF_MESSAGE(RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  return this->MAX_SIZE_OF_MESSAGE;
}

void SAMPLING_CHANNEL::SET_INITIALIZED() { this->INITIALIZED = true; };
bool SAMPLING_CHANNEL::GET_INITIALIZED() { return this->INITIALIZED; }

void SAMPLING_CHANNEL::ADD_SOURCE(std::string Name,
                                  MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
                                  SYSTEM_TIME_TYPE REFRESH_PERIOD,
                                  RETURN_CODE_TYPE *RETURN_CODE) {
  if (INITIALIZED) {
    LOGGER(ERROR, "CHANNEL %s ALREADY INITIALIZED", CHANNEL_NAME.c_str());
    *RETURN_CODE = NO_ACTION;
    return;
  }
  if (SOURCE_PORT != 0) {
    LOGGER(ERROR, "source port already defined for CHANNEL %s ",
           CHANNEL_NAME.c_str());
    *RETURN_CODE = NO_ACTION;
    return;
  }
  if (recv_mySocket != 0) {
    LOGGER(ERROR, "This is a multi module receive channel. source port already "
                  "defined for CHANNEL %s ",
           CHANNEL_NAME.c_str());
    *RETURN_CODE = NO_ACTION;
    return;
  }

  SOURCE_PORT =
      new SAMPLING_PORT_HELPER(Name, MAX_MESSAGE_SIZE, PORT_SOURCE_DEST,
                               REFRESH_PERIOD, RETURN_CODE, O_CREAT);
  LOGGER(INFO, "VALUE OF RETURN CODE IS %d", *RETURN_CODE);
  if (*RETURN_CODE == NO_ERROR) {
    SOURCE_PORT->shared_memory->ABSOLUTE_TIME_WRITTEN = 0;
    SOURCE_PORT->shared_memory->MESSAGE_SIZE = 0;
    this->MAX_SIZE_OF_MESSAGE = MAX_MESSAGE_SIZE;
    LOGGER(INFO, "CHANNEL %s MAX_SIZE_OF_MESSAGE=%d",
           this->CHANNEL_NAME.c_str(), MAX_SIZE_OF_MESSAGE);
  }

  return;
}
void SAMPLING_CHANNEL::ADD_DESTINATION(std::string Name,
                                       MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
                                       SYSTEM_TIME_TYPE REFRESH_PERIOD,
                                       RETURN_CODE_TYPE *RETURN_CODE) {
  if (INITIALIZED) {
    LOGGER(ERROR, "CHANNEL %s ALREADY INITIALIZED", CHANNEL_NAME.c_str());
    *RETURN_CODE = NO_ACTION;
    return;
  }
  if (this->MAX_SIZE_OF_MESSAGE != MAX_MESSAGE_SIZE) {
    LOGGER(ERROR, "CHANNEL %s MAX_SIZE_OF_MESSAGE for source not equal to "
                  "MAX_SIZE_OF_MESSAGE of destination",
           CHANNEL_NAME.c_str());
    *RETURN_CODE = INVALID_CONFIG;
    return;
  }

  SAMPLING_PORT_HELPER *temp =
      new SAMPLING_PORT_HELPER(Name, MAX_MESSAGE_SIZE, PORT_SOURCE_DEST,
                               REFRESH_PERIOD, RETURN_CODE, O_CREAT);
  if (*RETURN_CODE == NO_ERROR) {
    temp->shared_memory->ABSOLUTE_TIME_WRITTEN = 0;
    temp->shared_memory->MESSAGE_SIZE = 0;
    this->DESTINATION_PORT_VECTOR.push_back(temp);
  }
}
void SAMPLING_CHANNEL::MOVE_MESSAGES(RETURN_CODE_TYPE *RETURN_CODE) {

  *RETURN_CODE = NO_ERROR;

  LOGGER(INFO, "Sampling Channel %s Fired", CHANNEL_NAME.c_str());

  if (!INITIALIZED) {
    LOGGER(ERROR, "CHANNEL %s NOT INITIALIZED. SET_INITIALIZED First",
           CHANNEL_NAME.c_str());
    *RETURN_CODE = NO_ACTION;
    return;
  }

  LOGGER(TRACE, "FIRING  Sampling Channel %s", this->CHANNEL_NAME.c_str());

  if (this->recv_mySocket ==
      0) // This is an intra module channel as far as receive is concerned
  {

    // FIRST get the number of messages in Source
    if (this->SOURCE_PORT == 0 ||
        (this->DESTINATION_PORT_VECTOR.size() == 0 && sender_mySocket == 0)) {
      *RETURN_CODE = NO_ACTION;
      LOGGER(INFO, "SOURCE_PORT ==0 ||(DESTINATION_PORT_VECTOR.size()==0 && "
                   "sender_mySocket==0 )");
      return;
    }

    for (unsigned int j = 0; j < DESTINATION_PORT_VECTOR.size(); j++) {
      SOURCE_PORT->COPY_CONTENTS_VERBATIM(DESTINATION_PORT_VECTOR[j],
                                          RETURN_CODE);
      if (*RETURN_CODE != NO_ERROR) {
        LOGGER(ERROR, "FROM CHANNEL %s, error while writing to destination "
                      "sampling port %s",
               this->CHANNEL_NAME.c_str(),
               DESTINATION_PORT_VECTOR[j]->S_NAME.c_str());
        continue;
      }
    }

    if (this->sender_mySocket != 0) {
      // prepapre the packet
      // unsigned int
      // size=sizeof(SYSTEM_TIME_TYPE)+sizeof(MESSAGE_SIZE_TYPE)+SOURCE_PORT->shared_memory->MESSAGE_SIZE;
      unsigned int written = 0;
      memcpy(&(DataBuffer[0]),
             &(SOURCE_PORT->shared_memory->ABSOLUTE_TIME_WRITTEN),
             sizeof(SYSTEM_TIME_TYPE));
      std::cout << SOURCE_PORT->shared_memory->ABSOLUTE_TIME_WRITTEN
                << std::endl;
      written += sizeof(SYSTEM_TIME_TYPE);
      memcpy(&(DataBuffer[0]) + written,
             &(SOURCE_PORT->shared_memory->MESSAGE_SIZE),
             sizeof(MESSAGE_SIZE_TYPE));
      written += sizeof(MESSAGE_SIZE_TYPE);
      memcpy(&(DataBuffer[0]) + written, &(SOURCE_PORT->shared_memory->data[0]),
             SOURCE_PORT->shared_memory->MESSAGE_SIZE);
      written += SOURCE_PORT->shared_memory->MESSAGE_SIZE;
      for (unsigned int counter = 0;
           counter < this->destination_url_vector.size(); counter++) {
        this->sender_mySocket->sendTo(&(DataBuffer[0]), written,
                                      destination_url_vector[counter].node_name,
                                      destination_url_vector[counter].port);
        LOGGER(INFO, "size of written message to Socket %d", written);
      }
    }
  } else {
    // This port has a source socket port
    if (this->DESTINATION_PORT_VECTOR.size() == 0) {
      *RETURN_CODE = NO_ACTION;
      LOGGER(INFO, "FROM CHANNEL %s DESTINATION_QUEUE_VECTOR.size()=0",
             CHANNEL_NAME.c_str());
      return;
    }

    // read the queue and copy contents to
    // Try and Repeat this for maximum number of socket messages

    for (unsigned int times = 0; times < MAX_SOCKET_MESSAGE; times++) {
      unsigned int written = 0;
      // read from the socket
      int recvMsgSize = 0;
      string sourceAddress;      // Address of datagram source
      unsigned short sourcePort; // Port of datagram source
      LOGGER(INFO, "FROM CHANNEL %sTrying to Receive. Step 0",
             CHANNEL_NAME.c_str());
      recvMsgSize = recv_mySocket->recvFrom(
          &(DataBuffer[0]),
          sizeof(SYSTEM_TIME_TYPE) + sizeof(MESSAGE_SIZE_TYPE) +
              SYSTEM_LIMIT_MAX_SAMPLING_MESSAGE_SIZE,
          sourceAddress, sourcePort);
      LOGGER(INFO, " From sockedreceived message size %d", recvMsgSize);
      if (recvMsgSize == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        LOGGER(INFO, "FROM CHANNEL %s Nothing to Receive.error in 1 step of "
                     "inter-module receive",
               CHANNEL_NAME.c_str());
        return;
      }

      SYSTEM_TIME_TYPE TIME_OF_MESSAGE;
      MESSAGE_SIZE_TYPE message_real_size;
      memcpy(&TIME_OF_MESSAGE, &(DataBuffer[0]), sizeof(SYSTEM_TIME_TYPE));
      std::cout << TIME_OF_MESSAGE << endl;
      written += sizeof(SYSTEM_TIME_TYPE);
      memcpy(&message_real_size, &(DataBuffer[0]) + written,
             sizeof(MESSAGE_SIZE_TYPE));
      assert(message_real_size >= 0);
      written += sizeof(MESSAGE_SIZE_TYPE);

      // we have all the data;

      for (unsigned int j = 0; j < DESTINATION_PORT_VECTOR.size(); j++) {
        // SOURCE_PORT->COPY_CONTENTS_VERBATIM(DESTINATION_PORT_VECTOR[j],RETURN_CODE);
        LOGGER(INFO, "Writing to dest port %s",
               DESTINATION_PORT_VECTOR[j]->GET_NAME().c_str());
        DESTINATION_PORT_VECTOR[j]->shared_memory->ABSOLUTE_TIME_WRITTEN =
            TIME_OF_MESSAGE;
        DESTINATION_PORT_VECTOR[j]->shared_memory->MESSAGE_SIZE =
            message_real_size;
        memcpy(&(DESTINATION_PORT_VECTOR[j]->shared_memory->data[0]),
               &(DataBuffer[0]) + written, message_real_size);
      }
    }
  }
}
}
#endif //_ACM_SAMPLING_PORT_HELPER_H
