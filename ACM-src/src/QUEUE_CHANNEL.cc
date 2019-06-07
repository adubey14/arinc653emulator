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

#ifndef _ACM_QUEUE_CHANNEL_CC
#define _ACM_QUEUE_CHANNEL_CC
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

namespace ACM_INTERNAL {

/////////////////////////////////////
QUEUE_CHANNEL::~QUEUE_CHANNEL() {
  // printf("Myname is %s\n", CHANNEL_NAME.c_str());

  if (SOURCE_QUEUE != 0) {
    SOURCE_QUEUE->ref = SOURCE_QUEUE->ref - 1;
    if (SOURCE_QUEUE->ref == 0) {
      delete (SOURCE_QUEUE);
    }
  }

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
  for (unsigned int i = 0; i < this->DESTINATION_QUEUE_VECTOR.size(); i++) {
    if (DESTINATION_QUEUE_VECTOR[i] != 0)
      delete (DESTINATION_QUEUE_VECTOR[i]);
  }
  DESTINATION_QUEUE_VECTOR.clear();
  if (INTERNAL_BUFFER != 0)
    delete (this->INTERNAL_BUFFER);
  LOGGER(INFO, "Channel %s Destroyed", CHANNEL_NAME.c_str());
}

QUEUE_CHANNEL::QUEUE_CHANNEL(const QUEUE_CHANNEL &q)
    : CHANNEL_NAME(q.CHANNEL_NAME), SOURCE_QUEUE(q.SOURCE_QUEUE),
      INITIALIZED(q.INITIALIZED), INTERNAL_BUFFER(0),
      MAX_SIZEOFMESSAGE(q.MAX_SIZEOFMESSAGE),
      sender_mySocket(q.sender_mySocket), recv_mySocket(q.recv_mySocket),
      MAX_SOCKET_MESSAGE(q.MAX_SOCKET_MESSAGE),
      COMMUNICATION_TYPE(q.COMMUNICATION_TYPE)

{
  // memset ( &buffer[0], 0, SYSTEM_LIMIT_MAX_SAMPLING_MESSAGE_SIZE);

  if (q.SOURCE_QUEUE != 0)
    q.SOURCE_QUEUE->ref = q.SOURCE_QUEUE->ref + 1;
  if (q.sender_mySocket != 0)
    q.sender_mySocket->ref = q.sender_mySocket->ref + 1;
  if (q.recv_mySocket != 0)
    q.recv_mySocket->ref = q.recv_mySocket->ref + 1;

  //	assert(q.SOURCE_QUEUE==0);
  if (q.INTERNAL_BUFFER != 0) {
    this->INTERNAL_BUFFER = new char[q.MAX_SIZEOFMESSAGE];
    memcpy(q.INTERNAL_BUFFER, this->INTERNAL_BUFFER, q.MAX_SIZEOFMESSAGE);
  }

  LOGGER(INFO, "Channel %s Copied", CHANNEL_NAME.c_str());
}

void QUEUE_CHANNEL::SET_MAX_SIZE_OF_MESSAGE(MESSAGE_SIZE_TYPE data,
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
  this->MAX_SIZEOFMESSAGE = data;
  this->INTERNAL_BUFFER = new char[MAX_SIZEOFMESSAGE];
  return;
}

void QUEUE_CHANNEL::SET_COMMUNICATION_TYPE_ENUM(COMMUNICATION_TYPE_ENUM data,
                                                RETURN_CODE_TYPE *RETURN_CODE,
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

MESSAGE_SIZE_TYPE
QUEUE_CHANNEL::GET_MAX_SIZE_OF_MESSAGE(RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  return this->MAX_SIZEOFMESSAGE;
}
void QUEUE_CHANNEL::SET_MAX_SOCKET_MESSAGE(unsigned int data,
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

QUEUE_CHANNEL::QUEUE_CHANNEL(std::string NAME,
                             COMMUNICATION_TYPE_ENUM COMMUNICATION_TYPE_VAL,
                             unsigned int srcportNumber)
    : CHANNEL_NAME(NAME), SOURCE_QUEUE(0), INITIALIZED(false),
      INTERNAL_BUFFER(0), MAX_SIZEOFMESSAGE(0), sender_mySocket(0),
      recv_mySocket(0), MAX_SOCKET_MESSAGE(1),
      COMMUNICATION_TYPE(COMMUNICATION_TYPE_VAL) {
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

QUEUE_CHANNEL::QUEUE_CHANNEL(COMMUNICATION_TYPE_ENUM COMMUNICATION_TYPE_VAL,
                             unsigned int srcportNumber)
    : CHANNEL_NAME(""), SOURCE_QUEUE(0), INITIALIZED(false), INTERNAL_BUFFER(0),
      MAX_SIZEOFMESSAGE(0), sender_mySocket(0), recv_mySocket(0),
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

void QUEUE_CHANNEL::ADD_SOURCE_QUEUE(std::string Name,
                                     MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
                                     MESSAGE_RANGE_TYPE MAX_NB_MESSAGE,
                                     RETURN_CODE_TYPE *RETURN_CODE) {
  if (INITIALIZED) {
    LOGGER(ERROR, "CHANNEL %s ALREADY INITIALIZED", CHANNEL_NAME.c_str());
    *RETURN_CODE = NO_ACTION;
    return;
  }
  if (SOURCE_QUEUE != 0) {
    LOGGER(ERROR,
           "source port already defined for CHANNEL %s ALREADY INITIALIZED",
           CHANNEL_NAME.c_str());
    *RETURN_CODE = NO_ACTION;
    return;
  }

  SOURCE_QUEUE = new MESSAGE_QUEUE_TYPE(Name, MAX_MESSAGE_SIZE, MAX_NB_MESSAGE,
                                        PORT_SOURCE_DEST, RETURN_CODE, O_CREAT);

  assert(*RETURN_CODE == ACM_USER::NO_ERROR);

  this->MAX_SIZEOFMESSAGE = MAX_MESSAGE_SIZE;
  this->INTERNAL_BUFFER = new char[MAX_MESSAGE_SIZE];
}
void QUEUE_CHANNEL::ADD_DESTINATION_QUEUE(std::string Name,
                                          MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
                                          MESSAGE_RANGE_TYPE MAX_NB_MESSAGE,
                                          RETURN_CODE_TYPE *RETURN_CODE) {
  if (INITIALIZED) {
    LOGGER(ERROR, "CHANNEL %s ALREADY INITIALIZED", CHANNEL_NAME.c_str());
    *RETURN_CODE = NO_ACTION;
    return;
  }

  if (this->MAX_SIZEOFMESSAGE > MAX_MESSAGE_SIZE) {
    *RETURN_CODE = INVALID_CONFIG;
    LOGGER(ERROR, "this->MAX_SIZEOFMESSAGE>MAX_MESSAGE_SIZE");
    return;
  }
  MESSAGE_QUEUE_TYPE *DESTINATION_Q =
      new MESSAGE_QUEUE_TYPE(Name, MAX_MESSAGE_SIZE, MAX_NB_MESSAGE,
                             PORT_SOURCE_DEST, RETURN_CODE, O_CREAT);
  this->DESTINATION_QUEUE_VECTOR.push_back(DESTINATION_Q);
}

void QUEUE_CHANNEL::ADD_DESTINATION_URL(const std::string &nodename,
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

void QUEUE_CHANNEL::MOVE_MESSAGES(RETURN_CODE_TYPE *RETURN_CODE) {

  *RETURN_CODE = NO_ERROR;
  LOGGER(INFO, "Queue Channel %s Fired", CHANNEL_NAME.c_str());
  if (!INITIALIZED) {
    LOGGER(ERROR, "CHANNEL %s NOT INITIALIZED. SET_INITIALIZED First",
           CHANNEL_NAME.c_str());
    *RETURN_CODE = NO_ACTION;
    return;
  }
  // FIRST get the number of messages in Source
  MESSAGE_SIZE_TYPE LENGTH = 0;
  if (this->recv_mySocket ==
      0) // This is an intra module channel as far as receive is concerned
  {
    if (this->SOURCE_QUEUE == 0) {
      *RETURN_CODE = NO_ACTION;
      LOGGER(ERROR, "SOURCE_QUEUE ==0 ");
      return;
    }
    if (this->DESTINATION_QUEUE_VECTOR.size() == 0 && sender_mySocket == 0) {
      *RETURN_CODE = NO_ACTION;
      LOGGER(ERROR, "DESTINATION_QUEUE_VECTOR.size()=0 and sender_mySocket==0");
      return;
    }

    int COUNTER = SOURCE_QUEUE->NUMBER_OF_MESSAGES();
    QUEUING_PORT_STATUS_TYPE status;
    SOURCE_QUEUE->GET_QUEUING_PORT_STATUS(&status, RETURN_CODE);

    LOGGER(INFO, "queuestatus:%s:%d,%d,%d,%d", SOURCE_QUEUE->GET_NAME().c_str(),
           status.MAX_MESSAGE_SIZE, status.NB_MESSAGE, status.PORT_DIRECTION,
           status.WAITING_PROCESSES);

    if (COUNTER == 0) {
      LOGGER(INFO, "CHANNEL %s: no data to move: Source queue %s",
             this->CHANNEL_NAME.c_str(), SOURCE_QUEUE->GET_NAME().c_str());
    }

    for (int i = 0; i < COUNTER; i++) {

      SOURCE_QUEUE->RECEIVE_MESSAGE((MESSAGE_ADDR_TYPE)&INTERNAL_BUFFER[0],
                                    &LENGTH, (SYSTEM_TIME_TYPE)0, RETURN_CODE);
      if (*RETURN_CODE != NO_ERROR)
        return;

      for (unsigned int j = 0; j < DESTINATION_QUEUE_VECTOR.size(); j++) {
        DESTINATION_QUEUE_VECTOR[j]->SEND_MESSAGE(
            (MESSAGE_ADDR_TYPE)&INTERNAL_BUFFER[0], LENGTH, (SYSTEM_TIME_TYPE)0,
            RETURN_CODE);
        if (*RETURN_CODE != NO_ERROR) {
          if (*RETURN_CODE == TIMED_OUT)
            LOGGER(ERROR, "FROM CHANNEL %s, TIME OUT while writing to "
                          "destination queue %s",
                   this->CHANNEL_NAME.c_str(),
                   DESTINATION_QUEUE_VECTOR[j]->GET_NAME().c_str());
          else
            LOGGER(
                ERROR,
                "FROM CHANNEL %s, error while writing to destination queue %s",
                this->CHANNEL_NAME.c_str(),
                DESTINATION_QUEUE_VECTOR[j]->GET_NAME().c_str());
          continue;
        }
      }
      if (this->sender_mySocket != 0) {
        // prepapre the packet
        // unsigned int
        // size=sizeof(SYSTEM_TIME_TYPE)+sizeof(MESSAGE_SIZE_TYPE)+SOURCE_PORT->shared_memory->MESSAGE_SIZE;
        for (unsigned int counter = 0;
             counter < this->destination_url_vector.size(); counter++) {
          this->sender_mySocket->sendTo(
              &(INTERNAL_BUFFER[0]), LENGTH,
              destination_url_vector[counter].node_name,
              destination_url_vector[counter].port);
          std::cout << "size of written message " << LENGTH << endl;
        }
      }
    }
  } else {
    // Do something else....
    // This port has a source socket port
    if (this->DESTINATION_QUEUE_VECTOR.size() == 0) {
      *RETURN_CODE = NO_ACTION;
      LOGGER(INFO, "FROM CHANNEL %s DESTINATION_QUEUE_VECTOR.size()=0",
             CHANNEL_NAME.c_str());
      return;
    }

    //
    // read the queue and copy contents to
    // Try and Repeat this for maximum number of socket messages

    for (unsigned int times = 0; times < MAX_SOCKET_MESSAGE; times++) {
      // unsigned int written=0;
      int recvMsgSize = 0;
      string sourceAddress;      // Address of datagram source
      unsigned short sourcePort; // Port of datagram source
      LOGGER(INFO, "FROM CHANNEL %s Trying to Receive. Step 0",
             CHANNEL_NAME.c_str());
      recvMsgSize = recv_mySocket->recvFrom(&(INTERNAL_BUFFER[0]),
                                            this->MAX_SIZEOFMESSAGE,
                                            sourceAddress, sourcePort);

      std::cout << "received message size " << recvMsgSize << " "
                << sourceAddress << ":" << sourcePort << endl;
      if (recvMsgSize == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        LOGGER(INFO, "FROM CHANNEL %s Nothing to Receive.error in 1 step of "
                     "inter-module receive",
               CHANNEL_NAME.c_str());
        return;
      }
      LENGTH = recvMsgSize;
      // we have all the data;

      for (unsigned int j = 0; j < DESTINATION_QUEUE_VECTOR.size(); j++) {
        DESTINATION_QUEUE_VECTOR[j]->SEND_MESSAGE(
            (MESSAGE_ADDR_TYPE)&INTERNAL_BUFFER[0], LENGTH, (SYSTEM_TIME_TYPE)0,
            RETURN_CODE);
        if (*RETURN_CODE != NO_ERROR) {
          if (*RETURN_CODE == TIMED_OUT)
            LOGGER(ERROR, "FROM CHANNEL %s, TIME OUT while writing to "
                          "destination queue %s",
                   this->CHANNEL_NAME.c_str(),
                   DESTINATION_QUEUE_VECTOR[j]->GET_NAME().c_str());
          else
            LOGGER(
                ERROR,
                "FROM CHANNEL %s, error while writing to destination queue %s",
                this->CHANNEL_NAME.c_str(),
                DESTINATION_QUEUE_VECTOR[j]->GET_NAME().c_str());
          continue;
        }
      }
    }
  }
}
}

#endif //_ACM_QUEUE_HELPER_CC
