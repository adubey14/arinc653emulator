/* Example to show the use of buffers */

#include <ACM/USER/ACM_USER_INCLUDE.h>

namespace ACM_EXAMPLE {
//! This is the payload that is used to demonstrate interprocess communication
//! in the examples
struct Payload {

  ACM_USER::SYSTEM_TIME_TYPE time;
  float data;
};
}

using namespace ACM_USER;
using namespace ACM_EXAMPLE;

BUFFER_NAME_TYPE BUFFER_NAME = "TestBuffer1";
BUFFER_ID_TYPE TEST_BUFFER_ID;
PROCESS_ID_TYPE PROCESS_WRITER_ID;
PROCESS_ID_TYPE PROCESS_READER_ID;
PROCESS_NAME_TYPE PROCESS_WRITER_NAME = "PROCESS_WRITER";
PROCESS_NAME_TYPE PROCESS_READER_NAME = "PROCESS_READER";

PROCESS_ID_TYPE PROCESS_WRITER_ID1;
PROCESS_ID_TYPE PROCESS_READER_ID1;
PROCESS_NAME_TYPE PROCESS_WRITER_NAME1 = "PROCESS_WRITER1";
PROCESS_NAME_TYPE PROCESS_READER_NAME1 = "PROCESS_READER1";

void PROCESS_WRITER1(void) {

  Payload myLoad;
  RETURN_CODE_TYPE RETURN_CODE;
  LOGGER(APP, "In Writer 1");
  static int count = 0;

  myLoad.data = count;
  count++;

  myLoad.time = ACM_USER::currentTimeInNanoSeconds();

  SEND_BUFFER(
      /*in */ TEST_BUFFER_ID,
      /*in */ MESSAGE_ADDR_TYPE(&myLoad), /* by reference */
      /*in */ sizeof(myLoad),
      /*in */ INFINITE_TIME_VALUE, /* Block if Buffer is Full */
      /*out*/ &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);
}

void PROCESS_WRITER(void) {

  Payload myLoad;
  RETURN_CODE_TYPE RETURN_CODE;
  LOGGER(APP, "In Writer 0");
  static int count = 0;

  myLoad.data = count;
  count++;

  myLoad.time = ACM_USER::currentTimeInNanoSeconds();

  SEND_BUFFER(
      /*in */ TEST_BUFFER_ID,
      /*in */ MESSAGE_ADDR_TYPE(&myLoad), /* by reference */
      /*in */ sizeof(myLoad),
      /*in */ INFINITE_TIME_VALUE, /* Block if Buffer is Full */
      /*out*/ &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);
}

void PROCESS_READER1(void) {
  LOGGER(APP, "In Reader 1");
  Payload readLoad;
  RETURN_CODE_TYPE RETURN_CODE;
  MESSAGE_SIZE_TYPE length;

  while (true) {
    pthread_testcancel();

    RECEIVE_BUFFER(
        /*in */ TEST_BUFFER_ID,
        /*in */ INFINITE_TIME_VALUE,
        /*out*/ MESSAGE_ADDR_TYPE(&readLoad),
        /*out*/ &length,
        /*out*/ &RETURN_CODE);

    if (RETURN_CODE == NOT_AVAILABLE) {
      LOGGER(APP, "BUFFER IS EMPTY");
      return;
    }
    if (RETURN_CODE == TIMED_OUT) {
      LOGGER(APP, "READ REQUEST TIMED OUT");
      return;
    }
    if (RETURN_CODE == NO_ERROR) {
      LOGGER(APP, "Read Data %f. Time %lld nanoseconds since Jan 1 1970",
             readLoad.data, readLoad.time);
    }
  }
}

void PROCESS_READER(void) {
  LOGGER(APP, "In Reader 0");
  Payload readLoad;
  RETURN_CODE_TYPE RETURN_CODE;
  MESSAGE_SIZE_TYPE length;

  RECEIVE_BUFFER(
      /*in */ TEST_BUFFER_ID,
      /*in */ 0,
      /*out*/ MESSAGE_ADDR_TYPE(&readLoad),
      /*out*/ &length,
      /*out*/ &RETURN_CODE);

  if (RETURN_CODE == NOT_AVAILABLE) {
    LOGGER(APP, "BUFFER IS EMPTY");
    return;
  }
  if (RETURN_CODE == TIMED_OUT) {
    LOGGER(APP, "READ REQUEST TIMED OUT");
    return;
  }
  if (RETURN_CODE == NO_ERROR) {
    LOGGER(APP, "Read Data %f. Time %lld nanoseconds since Jan 1 1970",
           readLoad.data, readLoad.time);
  }
}

int main() {
  // SET_PARTITION_NAME. Required for Log Identification
  SET_PARTITION_NAME("TestingBUFFER");

  RETURN_CODE_TYPE RETURN_CODE;

  CREATE_BUFFER(BUFFER_NAME, sizeof(Payload), 10 /*number of messages*/, FIFO,
                &TEST_BUFFER_ID, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);

  // create the process writer process
  PROCESS_ATTRIBUTE_TYPE PROCESS_WRITER_ATTR;
  PROCESS_WRITER_ATTR.BASE_PRIORITY = 90;
  PROCESS_WRITER_ATTR.PERIOD = INFINITE_TIME_VALUE;
  PROCESS_WRITER_ATTR.TIME_CAPACITY = convertSecsToNano(5);
  PROCESS_WRITER_ATTR.DEADLINE = HARD;
  PROCESS_WRITER_ATTR.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE)PROCESS_WRITER;
  PROCESS_WRITER_ATTR.STACK_SIZE =
      0; // We currently do not support restricting stack size in this emulator
  strncpy(PROCESS_WRITER_ATTR.NAME, PROCESS_WRITER_NAME, MAX_NAME_LENGTH);
  CREATE_PROCESS(&PROCESS_WRITER_ATTR, &PROCESS_WRITER_ID, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);

  // start the process writer
  START(PROCESS_WRITER_ID, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);

  // Create Writer 2

  PROCESS_WRITER_ATTR.BASE_PRIORITY = 90;
  PROCESS_WRITER_ATTR.PERIOD = convertSecsToNano(10);
  PROCESS_WRITER_ATTR.TIME_CAPACITY = convertSecsToNano(10);
  PROCESS_WRITER_ATTR.DEADLINE = HARD;
  PROCESS_WRITER_ATTR.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE)PROCESS_WRITER1;
  PROCESS_WRITER_ATTR.STACK_SIZE =
      0; // We currently do not support restricting stack size in this emulator
  strncpy(PROCESS_WRITER_ATTR.NAME, PROCESS_WRITER_NAME1, MAX_NAME_LENGTH);
  CREATE_PROCESS(&PROCESS_WRITER_ATTR, &PROCESS_WRITER_ID1, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);

  // start the process writer
  START(PROCESS_WRITER_ID1, &RETURN_CODE);
  // assert(RETURN_CODE==NO_ERROR);

  // create the process reader process
  PROCESS_ATTRIBUTE_TYPE PROCESS_READER_ATTR;
  PROCESS_READER_ATTR.BASE_PRIORITY = 90;
  PROCESS_READER_ATTR.PERIOD = convertSecsToNano(5);
  PROCESS_READER_ATTR.TIME_CAPACITY = convertSecsToNano(5);
  PROCESS_READER_ATTR.DEADLINE = HARD;
  PROCESS_READER_ATTR.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE)PROCESS_READER;
  PROCESS_READER_ATTR.STACK_SIZE =
      0; // We currently do not support restricting stack size in this emulator
  strncpy(PROCESS_READER_ATTR.NAME, PROCESS_READER_NAME, MAX_NAME_LENGTH);
  CREATE_PROCESS(&PROCESS_READER_ATTR, &PROCESS_READER_ID, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);

  // start the process READER
  START(PROCESS_READER_ID, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);

  // create the process reader 1 process
  PROCESS_READER_ATTR.BASE_PRIORITY = 90;
  PROCESS_READER_ATTR.PERIOD = INFINITE_TIME_VALUE;
  PROCESS_READER_ATTR.TIME_CAPACITY = INFINITE_TIME_VALUE;
  PROCESS_READER_ATTR.DEADLINE = HARD;
  PROCESS_READER_ATTR.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE)PROCESS_READER1;
  PROCESS_READER_ATTR.STACK_SIZE =
      0; // We currently do not support restricting stack size in this emulator
  strncpy(PROCESS_READER_ATTR.NAME, PROCESS_READER_NAME1, MAX_NAME_LENGTH);
  CREATE_PROCESS(&PROCESS_READER_ATTR, &PROCESS_READER_ID1, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);

  // start the process READER
  START(PROCESS_READER_ID1, &RETURN_CODE);
  // assert(RETURN_CODE==NO_ERROR);

  // Setting partition mode. This will start the partition.
  LOGGER(APP, "Setting Partition State", APP);
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}