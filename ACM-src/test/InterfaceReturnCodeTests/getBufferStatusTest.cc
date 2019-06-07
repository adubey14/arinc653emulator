/*
Test of the GET_BLACKBOARD_STATUS service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <common.h>
using namespace ACM_USER;
using namespace ACM_EXAMPLE;

BUFFER_NAME_TYPE BUFFER_NAME = "MY_BUFFER";
BUFFER_ID_TYPE BUFFER_ID;
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME = "PROCESS_APERIODIC";
PROCESS_ID_TYPE PROCESS_APERIODIC_ID;

void PROCESS_APERIODIC(void) {
  RETURN_CODE_TYPE RETURN_CODE;
  BUFFER_STATUS_TYPE GET_STATUS;
  // Tests that no error is returned when correctly getting a buffer's status
  GET_BUFFER_STATUS(BUFFER_ID, &GET_STATUS, &RETURN_CODE);
  if (RETURN_CODE == NO_ERROR) {
    LOGGER(APP, "Get buffer status test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Get buffer status test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }

  // Tests that the correct code is returned when attempting to get the ID of a
  // nonexistent blackboard
  BUFFER_ID_TYPE NONEXISTENT_BUFFER_ID = 10;
  GET_BUFFER_STATUS(NONEXISTENT_BUFFER_ID, &GET_STATUS, &RETURN_CODE);
  if (RETURN_CODE == INVALID_PARAM) {
    LOGGER(APP, "Buffer does not exist test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Buffer does not exist test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
  return;
}

int main() {
  RETURN_CODE_TYPE RETURN_CODE;

  // SET_PARTITION_NAME. Required for Log Identification
  SET_PARTITION_NAME("TESTING_PROCESS_SCHEDULING");

  //! A dummy process fills in the idle gap in the scheduling window.
  // It is necessary to implement suspend service properly.
  // Note: You can get rid of dummy process if you do not use suspend and resume
  // services.
  // APEX_HELPER_CREATE_AND_START_DUMMY_PROCESS(&RETURN_CODE);

  // create the process APERIODIC process with no invalid parameters
  PROCESS_ATTRIBUTE_TYPE PROCESS_APERIODIC_ATTR;
  PROCESS_APERIODIC_ATTR.BASE_PRIORITY = 91;
  PROCESS_APERIODIC_ATTR.PERIOD = INFINITE_TIME_VALUE;
  PROCESS_APERIODIC_ATTR.TIME_CAPACITY = INFINITE_TIME_VALUE;
  PROCESS_APERIODIC_ATTR.DEADLINE = HARD;
  PROCESS_APERIODIC_ATTR.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE)PROCESS_APERIODIC;
  PROCESS_APERIODIC_ATTR.STACK_SIZE =
      0; // We currently do not support restricting stack size in this emulator
  strncpy(PROCESS_APERIODIC_ATTR.NAME, PROCESS_APERIODIC_NAME, MAX_NAME_LENGTH);
  CREATE_PROCESS(&PROCESS_APERIODIC_ATTR, &PROCESS_APERIODIC_ID, &RETURN_CODE);

  START(PROCESS_APERIODIC_ID, &RETURN_CODE);

  CREATE_BUFFER(
      /*!in */ BUFFER_NAME,
      /*!in */ sizeof(Payload),
      /*!in */ 10,
      /*!in */ FIFO,
      /*!out*/ &BUFFER_ID,
      /*!out*/ &RETURN_CODE);

  // Setting partition mode. This will start the partition.
  LOGGER(APP, "Setting Partition State", APP);
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}