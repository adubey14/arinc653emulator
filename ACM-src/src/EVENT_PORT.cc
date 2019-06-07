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

#ifndef ACM_EVENT_PORT_CC
#define ACM_EVENT_PORT_CC
#include <ACM/USER/EVENT_PORT.h>

#include <stdlib.h>

namespace ACM_USER {

/**********************************************************************************************************************/

ACM_EVENT_PORT::DESTINATION_URL::DESTINATION_URL() {}

ACM_EVENT_PORT::DESTINATION_URL::DESTINATION_URL(NAME_TYPE name,
                                                 const unsigned int port_num) {
  strncpy(node_name, name, MAX_NAME_LENGTH);
  port = port_num;
}
ACM_EVENT_PORT::DESTINATION_URL::DESTINATION_URL(const DESTINATION_URL &url) {
  strncpy(node_name, url.node_name, MAX_NAME_LENGTH);
  port = url.port;
}

void ACM_EVENT_PORT::DESTINATION_URL::printHere() {
  printf("Destination Name:%s, port:%u\n", node_name, port);
  printf("here\n");
}
void ACM_EVENT_PORT::SET_ONLY_INTER_MODULE(RETURN_CODE_TYPE *RETURN_CODE) {
  if (sender_mySocket == 0) {
    LOGGER(ERROR, "trying to SET_ONLY_INTER_MODULE in port %s. Ensure  "
                  "Destination URLS have been added",
           EVENT_PORT_NAME);
    *RETURN_CODE = NOT_AVAILABLE;
    return;
  }

  ONLY_INTER_MODULE = true;
}

void ACM_EVENT_PORT::ADD_INTER_MODULE_DESTINATION_URL(
    NAME_TYPE nodename, unsigned int port_number,
    RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  if (EVENT_PORT_TYPE == READ) {
    LOGGER(ERROR,
           "Trying to add INTER_MODULE_DESTINATION_URL to a read port %s",
           EVENT_PORT_NAME);
    *RETURN_CODE = INVALID_MODE;
    return;
  }

  if (EVENT_PORT_IMPL_TYPE == BUFFER || EVENT_PORT_IMPL_TYPE == BLACKBOARD) {
    LOGGER(ERROR, "Trying to add INTER_MODULE_DESTINATION_URL to an "
                  "intra-partition port %s",
           EVENT_PORT_NAME);
    *RETURN_CODE = INVALID_MODE;
    return;
  }

  printf("nodename %s, %d\n", nodename, port_number);

  // exit(0);

  DESTINATION_URL temp(nodename, port_number);
  if (sender_mySocket == 0) {
    sender_mySocket = new UDPSocket();
    sender_mySocket->make_non_blocking();
  }

  if (INTER_MODULE_DATA_BUFFER == 0) {
    INTER_MODULE_DATA_BUFFER =
        new char[sizeof(SYSTEM_TIME_TYPE) + sizeof(MESSAGE_SIZE_TYPE) +
                 EVENT_PORT_TOTAL_SIZE];
    LOGGER(INFO, "Size of INTER_MODULE_DATA_BUFFER %d",
           sizeof(SYSTEM_TIME_TYPE) + sizeof(MESSAGE_SIZE_TYPE) +
               EVENT_PORT_TOTAL_SIZE);
  }

  this->DESTINATIONS_URLS.push_back(temp);

  return;
}

void ACM_EVENT_PORT::ADD_INTER_MODULE_SOURCE_SOCKET(
    unsigned int port_number, RETURN_CODE_TYPE *RETURN_CODE) {
  *RETURN_CODE = NO_ERROR;
  if (EVENT_PORT_TYPE == WRITE) {
    LOGGER(ERROR,
           "Trying to  ADD_INTER_MODULE_SOURCE_SOCKET to a write port %s",
           EVENT_PORT_NAME);
    *RETURN_CODE = INVALID_MODE;
    return;
  }

  if (EVENT_PORT_IMPL_TYPE == BUFFER || EVENT_PORT_IMPL_TYPE == BLACKBOARD) {
    LOGGER(ERROR, "Trying to  ADD_INTER_MODULE_SOURCE_SOCKET to an "
                  "intra-partition port %s",
           EVENT_PORT_NAME);
    *RETURN_CODE = INVALID_MODE;
    return;
  }

  if (this->recv_mySocket != 0) {
    LOGGER(ERROR, "Recv Socket Already initializaed. Trying to add it again: "
                  "Event Port %s",
           EVENT_PORT_NAME);
    *RETURN_CODE = NO_ACTION;
    return;
  }

  recv_mySocket = new UDPSocket(port_number);
  if (INTER_MODULE_DATA_BUFFER == 0) {
    INTER_MODULE_DATA_BUFFER =
        new char[sizeof(SYSTEM_TIME_TYPE) + sizeof(MESSAGE_SIZE_TYPE) +
                 EVENT_PORT_TOTAL_SIZE];
  }

  // RecvSocket Will Be Blocking;
  return;
}

ACM_EVENT_PORT::ACM_EVENT_PORT(EVENT_PORT_COMM_TYPE COMM_TYPE,
                               EVENT_PORT_NAME_TYPE NAME,
                               MESSAGE_SIZE_TYPE SIZE,
                               SYSTEM_TIME_TYPE VALIDITY_PERIOD,
                               MESSAGE_RANGE_TYPE MAX_NB_MESSAGE,
                               RETURN_CODE_TYPE *RETURN_CODE, bool isCOLOCATED,
                               bool isDESTRUCTIVEREAD)
    : // member variable initialization
      EVENT_PORT_TYPE(COMM_TYPE),
      EVENT_PORT_VALIDITY_PERIOD(VALIDITY_PERIOD), EVENT_PORT_PAYLOAD(0),
      EVENT_PORT_IMPL_TYPE(ACM_EVENT_PORT::UNDEFINED),
      EVENT_PORT_DESTRUCTIVE_READ(isDESTRUCTIVEREAD), sender_mySocket(0),
      recv_mySocket(0), INTER_MODULE_DATA_BUFFER(0), ONLY_INTER_MODULE(false) {
  strncpy(EVENT_PORT_NAME, NAME, MAX_NAME_LENGTH);
  this->DESTINATIONS_URLS.clear();
  EVENT_PORT_TOTAL_SIZE = (SIZE + sizeof(SYSTEM_TIME_TYPE));
  EVENT_PORT_PAYLOAD = (MESSAGE_ADDR_TYPE)malloc(EVENT_PORT_TOTAL_SIZE);

  if (MAX_NB_MESSAGE == 1) // Non-buffered
  {
    if (isCOLOCATED) // co-located
    {
      if (isDESTRUCTIVEREAD) {
        EVENT_PORT_IMPL_TYPE = BUFFER;
      } else {
        EVENT_PORT_IMPL_TYPE = BLACKBOARD;
      }
    } else // non-co-located
    {
      if (isDESTRUCTIVEREAD) {
        EVENT_PORT_IMPL_TYPE = QUEUEINGPORT;
      } else {
        EVENT_PORT_IMPL_TYPE = SAMPLINGPORT;
      }
    }
  } else if (MAX_NB_MESSAGE > 1) // buffered
  {
    if (isCOLOCATED) // co-located
    {
      if (isDESTRUCTIVEREAD) {
        EVENT_PORT_IMPL_TYPE = BUFFER;
      } else {
        (*RETURN_CODE) = INVALID_CONFIG;
      }
    } else // non-co-located
    {
      if (isDESTRUCTIVEREAD) {
        EVENT_PORT_IMPL_TYPE = QUEUEINGPORT;
      } else {
        (*RETURN_CODE) = INVALID_CONFIG;
      }
    }
  } else                             // buffer-size = 0
    (*RETURN_CODE) = INVALID_CONFIG; // MAX_NB_MESSAGE cannot be zero.

  PORT_DIRECTION_TYPE PORT_DIRECTION =
      (COMM_TYPE == WRITE) ? SOURCE : DESTINATION;

  switch (EVENT_PORT_IMPL_TYPE) {

  case SAMPLINGPORT:

    LOGGER(TRACE, "Creating sampling port of size %d\n", EVENT_PORT_TOTAL_SIZE);
    CREATE_SAMPLING_PORT(NAME, EVENT_PORT_TOTAL_SIZE, PORT_DIRECTION,
                         VALIDITY_PERIOD, &EVENT_PORT_ID, RETURN_CODE);
    // already created, then get the id
    if (*RETURN_CODE == NO_ACTION)
      GET_SAMPLING_PORT_ID(NAME, &EVENT_PORT_ID, RETURN_CODE);
    break;

  case QUEUEINGPORT:

    CREATE_QUEUING_PORT(NAME, EVENT_PORT_TOTAL_SIZE, MAX_NB_MESSAGE,
                        PORT_DIRECTION, FIFO, &EVENT_PORT_ID, RETURN_CODE);
    // already created, then get the id
    if (*RETURN_CODE == NO_ACTION)
      GET_QUEUING_PORT_ID(NAME, &EVENT_PORT_ID, RETURN_CODE);
    break;

  case BLACKBOARD:

    CREATE_BLACKBOARD(NAME, EVENT_PORT_TOTAL_SIZE, &EVENT_PORT_ID, RETURN_CODE);
    // already created, then get the id
    if (*RETURN_CODE == NO_ACTION)
      GET_BLACKBOARD_ID(NAME, &EVENT_PORT_ID, RETURN_CODE);
    break;

  case BUFFER:

    CREATE_BUFFER(NAME, EVENT_PORT_TOTAL_SIZE, MAX_NB_MESSAGE, FIFO,
                  &EVENT_PORT_ID, RETURN_CODE);
    // already created, then get the id
    if (*RETURN_CODE == NO_ACTION) {
      GET_BUFFER_ID(NAME, &EVENT_PORT_ID, RETURN_CODE);
      LOGGER(TRACE, "GOT HANDLE TO BUFFER %s - ID - %d\n ", NAME,
             EVENT_PORT_ID);
    }
    break;

  default:
    *RETURN_CODE = INVALID_CONFIG;
    break;
  };
}

ACM_EVENT_PORT::~ACM_EVENT_PORT() {
  if (EVENT_PORT_PAYLOAD)
    free(EVENT_PORT_PAYLOAD);

  // Note :The underlying comm channel is not being deleted here.
  // Question: Will the partition on shutdown, delete all the underlying
  // comm channels.

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

  if (INTER_MODULE_DATA_BUFFER != 0)
    delete (INTER_MODULE_DATA_BUFFER);
}

void ACM_EVENT_PORT::BLOCK_AND_TRANSFER_FROM_SOCKET_READ_PORT(
    RETURN_CODE_TYPE *RETURN_CODE, SYSTEM_TIME_TYPE PORT_TIME_OUT) {
  *RETURN_CODE = NO_ERROR;
  if (EVENT_PORT_TYPE == WRITE) {
    LOGGER(ERROR, "Trying to  BLOCK_AND_TRANSFER_FROM_SOCKET_READ_PORT to a "
                  "write port %s",
           EVENT_PORT_NAME);
    *RETURN_CODE = INVALID_MODE;
    return;
  }

  if (EVENT_PORT_IMPL_TYPE == BUFFER || EVENT_PORT_IMPL_TYPE == BLACKBOARD) {
    LOGGER(ERROR, "Trying to  BLOCK_AND_TRANSFER_FROM_SOCKET_READ_PORT in an "
                  "intra-partition port %s",
           EVENT_PORT_NAME);
    *RETURN_CODE = INVALID_MODE;
    return;
  }

  if (this->recv_mySocket == 0) {
    LOGGER(ERROR, "Receive Socket Note Configured %s", EVENT_PORT_NAME);
    *RETURN_CODE = INVALID_MODE;
    return;
  }

  assert(INTER_MODULE_DATA_BUFFER != 0);

  // Now Read From the Socket....
  static unsigned int recvMsgSize;

  recvMsgSize = 0;
  LOGGER(INFO, "FROM Event Port %s Trying to Receive.", EVENT_PORT_NAME);
  recvMsgSize = recv_mySocket->recvFrom(&(INTER_MODULE_DATA_BUFFER[0]),
                                        sizeof(SYSTEM_TIME_TYPE) +
                                            sizeof(MESSAGE_SIZE_TYPE) +
                                            EVENT_PORT_TOTAL_SIZE,
                                        sourceAddress, sourcePort);
  LOGGER(INFO, "FROM Event Port %s Received %d bytes.", EVENT_PORT_NAME,
         recvMsgSize);

  switch (EVENT_PORT_IMPL_TYPE) {
  case BLACKBOARD:
    break;
  case BUFFER:
    break;
  case SAMPLINGPORT:
    WRITE_CONTENTS_FROM_DATA_BUFFER(
        EVENT_PORT_ID, (MESSAGE_ADDR_TYPE)&INTER_MODULE_DATA_BUFFER[0],
        sizeof(SYSTEM_TIME_TYPE) + sizeof(MESSAGE_SIZE_TYPE) +
            EVENT_PORT_TOTAL_SIZE,
        RETURN_CODE);
    if (*RETURN_CODE != NO_ERROR) {
      LOGGER(ERROR, "FROM  Event port socket read %s, error while writing to "
                    "destination queue ",
             this->EVENT_PORT_NAME);
    }
    break;
  case QUEUEINGPORT:
    MESSAGE_QUEUE_SEND_MESSAGE_FROM_BUFFER(
        EVENT_PORT_ID, (MESSAGE_ADDR_TYPE)&INTER_MODULE_DATA_BUFFER[0],
        recvMsgSize, PORT_TIME_OUT, RETURN_CODE);
    if (*RETURN_CODE != NO_ERROR) {
      if (*RETURN_CODE == TIMED_OUT)
        LOGGER(ERROR, "FROM Event port socket read %s, TIME OUT while writing "
                      "to destination queue ",
               EVENT_PORT_NAME);
      else
        LOGGER(ERROR, "FROM  Event port socket read %s, error while writing to "
                      "destination queue ",
               this->EVENT_PORT_NAME);
    }
    break;
  default:
    *RETURN_CODE = NO_ACTION;
    break;
  };
}

void ACM_EVENT_PORT::WRITE_TO_EVENT_PORT(MESSAGE_ADDR_TYPE MESSAGE_ADDR,
                                         MESSAGE_SIZE_TYPE LENGTH,
                                         SYSTEM_TIME_TYPE TIME_OUT,
                                         RETURN_CODE_TYPE *RETURN_CODE) {

  MESSAGE_SIZE_TYPE NEW_LENGTH = 0;

  if (!ASSEMBLE_DATA(MESSAGE_ADDR, LENGTH, &NEW_LENGTH)) {

    *RETURN_CODE = INVALID_CONFIG; // size is larger than inital config
    return;
  }

  LOGGER(INFO, "size of new length %ld", NEW_LENGTH);

  switch (EVENT_PORT_IMPL_TYPE) {
  case BLACKBOARD:

    DISPLAY_BLACKBOARD(EVENT_PORT_ID, EVENT_PORT_PAYLOAD, NEW_LENGTH,
                       RETURN_CODE);
    break;

  case BUFFER:

    SEND_BUFFER(EVENT_PORT_ID, EVENT_PORT_PAYLOAD, NEW_LENGTH, TIME_OUT,
                RETURN_CODE);
    break;

  case SAMPLINGPORT:

    WRITE_SAMPLING_MESSAGE(EVENT_PORT_ID, EVENT_PORT_PAYLOAD, NEW_LENGTH,
                           RETURN_CODE);

    if (sender_mySocket != 0) {
      assert(INTER_MODULE_DATA_BUFFER != 0);
      MESSAGE_SIZE_TYPE READ_LENGTH;
      READ_CONTENTS_TO_DATA_BUFFER(
          EVENT_PORT_ID, (MESSAGE_ADDR_TYPE)&INTER_MODULE_DATA_BUFFER[0],
          sizeof(SYSTEM_TIME_TYPE) + sizeof(MESSAGE_SIZE_TYPE) +
              EVENT_PORT_TOTAL_SIZE,
          &READ_LENGTH, RETURN_CODE);
      assert(*RETURN_CODE == NO_ERROR);
      LOGGER(INFO, "size of written message to Socket %ld", READ_LENGTH);
      //	LOGGER(INFO,"Read Contents from buffer. Trying to send to
      //destination");

      for (unsigned int counter = 0; counter < this->DESTINATIONS_URLS.size();
           counter++) {
        this->DESTINATIONS_URLS[counter].printHere();

        this->sender_mySocket->sendTo(INTER_MODULE_DATA_BUFFER, READ_LENGTH,
                                      DESTINATIONS_URLS[counter].node_name,
                                      DESTINATIONS_URLS[counter].port);
      }
    }
    LOGGER(TRACE, "FINISHED WRITING SAMPLING MESSAGE");
    break;

  case QUEUEINGPORT:

    if (!ONLY_INTER_MODULE) {
      SEND_QUEUING_MESSAGE(EVENT_PORT_ID, EVENT_PORT_PAYLOAD, NEW_LENGTH,
                           TIME_OUT, RETURN_CODE);
    }

    if (sender_mySocket != 0) {
      assert(INTER_MODULE_DATA_BUFFER != 0);
      LOGGER(INFO, "size of written message to Socket %d", EVENT_PORT_PAYLOAD);
      for (unsigned int counter = 0; counter < this->DESTINATIONS_URLS.size();
           counter++) {
        this->sender_mySocket->sendTo(EVENT_PORT_PAYLOAD, NEW_LENGTH,
                                      DESTINATIONS_URLS[counter].node_name,
                                      DESTINATIONS_URLS[counter].port);
      }
    }
    break;

  default:

    *RETURN_CODE = NO_ACTION;
    break;
  }
}

void ACM_EVENT_PORT::READ_FROM_EVENT_PORT(MESSAGE_ADDR_TYPE MESSAGE_ADDR,
                                          MESSAGE_SIZE_TYPE *LENGTH,
                                          bool *IS_VALID,
                                          SYSTEM_TIME_TYPE TIME_OUT,
                                          RETURN_CODE_TYPE *RETURN_CODE) {

  VALIDITY_TYPE VALIDITY = VALID;

  switch (EVENT_PORT_IMPL_TYPE) {
  case BLACKBOARD:

    READ_BLACKBOARD(EVENT_PORT_ID, TIME_OUT, EVENT_PORT_PAYLOAD, LENGTH,
                    RETURN_CODE);
    // if (EVENT_PORT_DESTRUCTIVE_READ && (*RETURN_CODE == NO_ERROR))
    //	CLEAR_BLACKBOARD(EVENT_PORT_ID,RETURN_CODE);
    break;

  case BUFFER:

    RECEIVE_BUFFER(EVENT_PORT_ID, TIME_OUT, EVENT_PORT_PAYLOAD, LENGTH,
                   RETURN_CODE);
    LOGGER(TRACE, "FINISHED READING BUFFER");
    break;

  case SAMPLINGPORT:

    READ_SAMPLING_MESSAGE(EVENT_PORT_ID, EVENT_PORT_PAYLOAD, LENGTH, &VALIDITY,
                          RETURN_CODE);
    LOGGER(TRACE, "FINISHED READING SAMPLING MESSAGE");
    break;

  case QUEUEINGPORT:

    RECEIVE_QUEUING_MESSAGE(EVENT_PORT_ID, TIME_OUT, EVENT_PORT_PAYLOAD, LENGTH,
                            RETURN_CODE);
    break;

  default:

    *RETURN_CODE = NO_ACTION;
    break;
  }

  LOGGER(TRACE, "STARTED PARSE_AND_VALIDATE_DATA");
  PARSE_AND_VALIDATE_DATA(MESSAGE_ADDR, LENGTH, VALIDITY, IS_VALID,
                          RETURN_CODE);

  LOGGER(TRACE, "FINISHED READ");
}

// Possible Problem
void ACM_EVENT_PORT::PARSE_AND_VALIDATE_DATA(MESSAGE_ADDR_TYPE MESSAGE_ADDR,
                                             MESSAGE_SIZE_TYPE *LENGTH,
                                             VALIDITY_TYPE VALIDITY,
                                             bool *IS_VALID,
                                             RETURN_CODE_TYPE *RETURN_CODE) {

  (*IS_VALID) = false;

  // check the return code -> if error return immediately
  if (*RETURN_CODE != NO_ERROR) {
    LOGGER(ERROR, "ERROR %d\n", *RETURN_CODE);
    return;
  }

  SYSTEM_TIME_TYPE WRITE_TIME;
  MESSAGE_SIZE_TYPE MIN_SIZE = sizeof(SYSTEM_TIME_TYPE);
  if (*LENGTH < MIN_SIZE) {
    // Invalid data
    *RETURN_CODE = NOT_AVAILABLE;
    LOGGER(ERROR, "LENGTH <MINSIZE %d\n", *RETURN_CODE);
    return;
  }

  *LENGTH = *LENGTH - MIN_SIZE;
  memcpy(MESSAGE_ADDR, EVENT_PORT_PAYLOAD + MIN_SIZE, *LENGTH);
  LOGGER(TRACE, "COPIED MESSAGE");

  if (VALIDITY == VALID) {
    LOGGER(TRACE, "CHECKING VALIDITY");
    memcpy((void *)(&WRITE_TIME), EVENT_PORT_PAYLOAD, MIN_SIZE);
    SYSTEM_TIME_TYPE AGE = currentTimeInNanoSeconds() - WRITE_TIME;
    if (AGE <= EVENT_PORT_VALIDITY_PERIOD)
      (*IS_VALID) = true;
  } else
    LOGGER(TRACE, "NOT CHECKING VALIDITY...Validity is already set to invalid");
}

bool ACM_EVENT_PORT::ASSEMBLE_DATA(MESSAGE_ADDR_TYPE MESSAGE_ADDR,
                                   MESSAGE_SIZE_TYPE LENGTH,
                                   MESSAGE_SIZE_TYPE *NEW_LENGTH) {
  bool ret = false;

  MESSAGE_SIZE_TYPE TIME_SIZE = sizeof(SYSTEM_TIME_TYPE);
  SYSTEM_TIME_TYPE WRITE_TIME = currentTimeInNanoSeconds();

  *NEW_LENGTH = LENGTH + TIME_SIZE;

  if (*NEW_LENGTH <= EVENT_PORT_TOTAL_SIZE) {
    memcpy(EVENT_PORT_PAYLOAD, &WRITE_TIME, TIME_SIZE);
    memcpy(EVENT_PORT_PAYLOAD + TIME_SIZE, MESSAGE_ADDR, LENGTH);
    ret = true;
  }

  return ret;
}

/**************************************************************************************************************************/
}
#endif // ACM_EVENT_PORT_CC
