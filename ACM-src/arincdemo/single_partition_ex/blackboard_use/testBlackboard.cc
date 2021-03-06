#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <common.h>
using namespace ACM_USER;
using namespace ACM_EXAMPLE;

BLACKBOARD_NAME_TYPE BLACKBOARD_NAME = "TestBoard1";
BLACKBOARD_ID_TYPE TEST_BOARD_ID;
PROCESS_ID_TYPE PROCESS_WRITER_ID;
PROCESS_ID_TYPE PROCESS_READER_ID;
PROCESS_NAME_TYPE PROCESS_WRITER_NAME = "PROCESS_WRITER";
PROCESS_NAME_TYPE PROCESS_READER_NAME = "PROCESS_READER";

void PROCESS_WRITER(void) {

  Payload myLoad;
  RETURN_CODE_TYPE RETURN_CODE;
  LOGGER(USER, "In Writer");
  static int count = 0;

  myLoad.data = count;
  count++;
  myLoad.time = ACM_USER::currentTimeInNanoSeconds();
  DISPLAY_BLACKBOARD(TEST_BOARD_ID, (APEX_BYTE *)(&myLoad), sizeof(myLoad),
                     &RETURN_CODE);
  ACM_ASSERT(RETURN_CODE == NO_ERROR);
}

void PROCESS_READER(void) {
  LOGGER(USER, "In Reader");
  Payload readLoad;
  RETURN_CODE_TYPE RETURN_CODE;
  MESSAGE_SIZE_TYPE length;
  READ_BLACKBOARD(TEST_BOARD_ID, convertSecsToNano(1) /*Timeout*/,
                  (APEX_BYTE *)(&readLoad), &length, &RETURN_CODE);
  if (RETURN_CODE == NOT_AVAILABLE) {
    LOGGER(USER, "BLACKBOARD IS EMPTY");
    return;
  }
  if (RETURN_CODE == TIMED_OUT) {
    LOGGER(USER, "READ REQUEST TIMED OUT");
    return;
  }
  if (RETURN_CODE == NO_ERROR) {
    LOGGER(USER, "Read Data %f. Time %lld nanoseconds since Jan 1 1970",
           readLoad.data, readLoad.time);
  }
}

int main() {
  // SET_PARTITION_NAME. Required for Log Identification
  SET_PARTITION_NAME("TestingBlackboard");

  RETURN_CODE_TYPE RETURN_CODE;

  //! A dummy process fills in the idle gap in the scheduling window.
  // It is necessary to implement suspend service properly.
  // Note: You can get rid of dummy process if you do not use suspend and resume
  // services.
  // APEX_HELPER_CREATE_AND_START_DUMMY_PROCESS(&RETURN_CODE);
  // ACM_ASSERT(RETURN_CODE==NO_ERROR);

  CREATE_BLACKBOARD(BLACKBOARD_NAME, sizeof(Payload), &TEST_BOARD_ID,
                    &RETURN_CODE);
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

  // create the process writer process
  PROCESS_ATTRIBUTE_TYPE PROCESS_WRITER_ATTR;
  PROCESS_WRITER_ATTR.BASE_PRIORITY = 90;
  PROCESS_WRITER_ATTR.PERIOD = convertSecsToNano(5);
  PROCESS_WRITER_ATTR.TIME_CAPACITY = convertSecsToNano(5);
  PROCESS_WRITER_ATTR.DEADLINE = HARD;
  PROCESS_WRITER_ATTR.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE)PROCESS_WRITER;
  PROCESS_WRITER_ATTR.STACK_SIZE =
      0; // We currently do not support restricting stack size in this emulator
  strncpy(PROCESS_WRITER_ATTR.NAME, PROCESS_WRITER_NAME, MAX_NAME_LENGTH);
  CREATE_PROCESS(&PROCESS_WRITER_ATTR, &PROCESS_WRITER_ID, &RETURN_CODE);
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

  // start the process writer
  START(PROCESS_WRITER_ID, &RETURN_CODE);
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

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
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

  // start the process READER
  START(PROCESS_READER_ID, &RETURN_CODE);
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

  // Setting partition mode. This will start the partition.
  LOGGER(USER, "Setting Partition State");
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}