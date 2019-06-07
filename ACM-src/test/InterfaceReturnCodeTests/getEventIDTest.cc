/*
Test of the GET_EVENT_ID service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;

EVENT_ID_TYPE SOME_EVENT_ID;
EVENT_NAME_TYPE EVENT_NAME = "EVENT1";
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME = "PROCESS_APERIODIC";
PROCESS_ID_TYPE PROCESS_APERIODIC_ID;

void PROCESS_APERIODIC(void) {
  RETURN_CODE_TYPE RETURN_CODE;
  EVENT_ID_TYPE GET_ID;
  // Tests that no error is returned when correctly getting an event's ID
  GET_EVENT_ID(EVENT_NAME, &GET_ID, &RETURN_CODE);
  if (GET_ID == SOME_EVENT_ID) {
    LOGGER(APP, "Get event ID test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Get event ID test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }

  EVENT_NAME_TYPE NONEXISTENT_NAME = "ANOTHER_EVENT";
  // Tests that the correct code is returned when attempting to get the ID of an
  // event that does not exist
  GET_EVENT_ID(NONEXISTENT_NAME, &GET_ID, &RETURN_CODE);
  if (RETURN_CODE == INVALID_CONFIG) {
    LOGGER(APP, "No such event test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "No such event test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
}

int main() {
  RETURN_CODE_TYPE RETURN_CODE;

  // SET_PARTITION_NAME. Required for Log Identification
  SET_PARTITION_NAME("TESTING_EVENT");

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

  CREATE_EVENT(EVENT_NAME, &SOME_EVENT_ID, &RETURN_CODE);
  START(PROCESS_APERIODIC_ID, &RETURN_CODE);

  // Setting partition mode. This will start the partition.
  LOGGER(APP, "Setting Partition State", APP);
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}