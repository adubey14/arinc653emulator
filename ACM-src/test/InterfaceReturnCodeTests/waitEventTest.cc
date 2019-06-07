/*
Test of the WAIT_EVENT service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;

PROCESS_ID_TYPE PROCESS_APERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME = "PROCESS_APERIODIC";
PROCESS_ID_TYPE PROCESS_TIMEOUT_ID;
PROCESS_NAME_TYPE PROCESS_TIMEOUT_NAME = "PROCESS_TIMEOUT";
EVENT_ID_TYPE SOME_EVENT_ID;
EVENT_NAME_TYPE EVENT_NAME = "EVENT1";
EVENT_ID_TYPE SOME_OTHER_EVENT_ID;
EVENT_NAME_TYPE EVENT_NAME2 = "EVENT2";
EVENT_ID_TYPE YET_ANOTHER_EVENT_ID;
EVENT_NAME_TYPE EVENT_NAME3 = "EVENT3";
RETURN_CODE_TYPE RETURN_CODE;
// intended to track the return code while the main aperiodic process is waiting
void PROCESS_TIMEOUT(void) {
  RETURN_CODE_TYPE TIMEOUT_RETURN;
  TIMED_WAIT(convertSecsToNano(0.1), &TIMEOUT_RETURN);
  LOGGER(APP, "Initial return code is: %d", RETURN_CODE);
  TIMED_WAIT(convertSecsToNano(1), &TIMEOUT_RETURN);
}

void PROCESS_APERIODIC(void) {
  RETURN_CODE_TYPE WAIT_RETURN;
  LOGGER(APP, "Starting APERIODIC.");

  // Tests no error is returned when correctly waiting on an event in the up
  // state
  SET_EVENT(SOME_EVENT_ID, &RETURN_CODE);
  START(PROCESS_TIMEOUT_ID, &WAIT_RETURN);
  LOGGER(APP, "Waiting on up event.");
  START(PROCESS_TIMEOUT_ID, &WAIT_RETURN);
  WAIT_EVENT(SOME_EVENT_ID, convertSecsToNano(1),
             &RETURN_CODE); // Return code is checked in parallel process
  LOGGER(APP, "Done waiting.");
  TIMED_WAIT(convertSecsToNano(1.5), &WAIT_RETURN);

  // Tests that the correct code is returned when attempting to wait on an event
  // in the down state
  RESET_EVENT(SOME_EVENT_ID, &RETURN_CODE);
  START(PROCESS_TIMEOUT_ID, &WAIT_RETURN);
  LOGGER(APP, "Waiting on down event.");
  WAIT_EVENT(SOME_EVENT_ID, convertSecsToNano(1),
             &RETURN_CODE); // Return code is checked in parallel process
  LOGGER(APP, "Done waiting.");
  TIMED_WAIT(convertSecsToNano(1.5), &WAIT_RETURN);

  // Tests that the correct code is returned when the given timeout is out of
  // range
  SET_EVENT(YET_ANOTHER_EVENT_ID, &RETURN_CODE);
  WAIT_EVENT(YET_ANOTHER_EVENT_ID, convertSecsToNano(315569261), &RETURN_CODE);
  if (RETURN_CODE == NOT_AVAILABLE) {
    LOGGER(APP, "Timeout too large test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Timeout too large test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }

  // Tests that the correct code is returned when attempting to wait on an event
  // that does not exist
  EVENT_ID_TYPE NONEXISTENT_ID = 10;
  WAIT_EVENT(NONEXISTENT_ID, convertSecsToNano(5), &RETURN_CODE);
  if (RETURN_CODE == INVALID_PARAM) {
    LOGGER(APP, "No such event test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "No such event test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
}

int main() {
  // create the process APERIODIC process
  PROCESS_ATTRIBUTE_TYPE PROCESS_APERIODIC_ATTR;
  PROCESS_APERIODIC_ATTR.BASE_PRIORITY = 90;
  PROCESS_APERIODIC_ATTR.PERIOD = INFINITE_TIME_VALUE; // APeriodic
  PROCESS_APERIODIC_ATTR.TIME_CAPACITY = INFINITE_TIME_VALUE;
  PROCESS_APERIODIC_ATTR.DEADLINE = HARD;
  PROCESS_APERIODIC_ATTR.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE)PROCESS_APERIODIC;
  PROCESS_APERIODIC_ATTR.STACK_SIZE =
      0; // We currently do not support restricting stack size in this emulator
  strncpy(PROCESS_APERIODIC_ATTR.NAME, PROCESS_APERIODIC_NAME, MAX_NAME_LENGTH);
  CREATE_PROCESS(&PROCESS_APERIODIC_ATTR, &PROCESS_APERIODIC_ID, &RETURN_CODE);

  // create the process TIMEOUT process
  PROCESS_ATTRIBUTE_TYPE PROCESS_TIMEOUT_ATTR;
  PROCESS_TIMEOUT_ATTR.BASE_PRIORITY = 90;
  PROCESS_TIMEOUT_ATTR.PERIOD = INFINITE_TIME_VALUE; // TIMEOUT
  PROCESS_TIMEOUT_ATTR.TIME_CAPACITY = INFINITE_TIME_VALUE;
  PROCESS_TIMEOUT_ATTR.DEADLINE = HARD;
  PROCESS_TIMEOUT_ATTR.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE)PROCESS_TIMEOUT;
  PROCESS_TIMEOUT_ATTR.STACK_SIZE =
      0; // We currently do not support restricting stack size in this emulator
  strncpy(PROCESS_TIMEOUT_ATTR.NAME, PROCESS_TIMEOUT_NAME, MAX_NAME_LENGTH);
  CREATE_PROCESS(&PROCESS_TIMEOUT_ATTR, &PROCESS_TIMEOUT_ID, &RETURN_CODE);

  // SET_PARTITION_NAME. Required for Log Identification
  SET_PARTITION_NAME("TESTING_EVENT");

  //! A dummy process fills in the idle gap in the scheduling window.
  // It is necessary to implement suspend service properly.
  // Note: You can get rid of dummy process if you do not use suspend and resume
  // services.
  // APEX_HELPER_CREATE_AND_START_DUMMY_PROCESS(&RETURN_CODE);

  CREATE_EVENT(EVENT_NAME, &SOME_EVENT_ID, &RETURN_CODE);
  CREATE_EVENT(EVENT_NAME2, &SOME_OTHER_EVENT_ID, &RETURN_CODE);
  CREATE_EVENT(EVENT_NAME3, &YET_ANOTHER_EVENT_ID, &RETURN_CODE);

  START(PROCESS_APERIODIC_ID, &RETURN_CODE);

  // Setting partition mode. This will start the partition.
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}