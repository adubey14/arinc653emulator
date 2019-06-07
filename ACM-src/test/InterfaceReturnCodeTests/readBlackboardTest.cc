/*
Test of the READ_BLACKBOARD service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <common.h>
using namespace ACM_USER;
using namespace ACM_EXAMPLE;

PROCESS_ID_TYPE PROCESS_APERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME = "PROCESS_APERIODIC";
PROCESS_ID_TYPE PROCESS_TIMEOUT_ID;
PROCESS_NAME_TYPE PROCESS_TIMEOUT_NAME = "PROCESS_TIMEOUT";
BLACKBOARD_ID_TYPE SOME_BLACKBOARD_ID;
BLACKBOARD_NAME_TYPE BLACKBOARD_NAME = "BLACKBOARD";
RETURN_CODE_TYPE RETURN_CODE;
// intended to track the return code while the main aperiodic process is blocked
void PROCESS_TIMEOUT(void) {
  RETURN_CODE_TYPE TIMEOUT_RETURN;
  TIMED_WAIT(convertSecsToNano(0.01), &TIMEOUT_RETURN);
  LOGGER(APP, "Initial return code is: %d", RETURN_CODE);
  return;
}

void PROCESS_APERIODIC(void) {
  RETURN_CODE_TYPE WAIT_RETURN;
  MESSAGE_SIZE_TYPE length;
  Payload readLoad, myLoad;
  myLoad.data = 5;
  myLoad.time = ACM_USER::currentTimeInNanoSeconds();
  LOGGER(APP, "Starting APERIODIC.");
  DISPLAY_BLACKBOARD(SOME_BLACKBOARD_ID, (APEX_BYTE *)(&myLoad),
                     sizeof(Payload), &RETURN_CODE);
  // Tests no error is returned when correctly when reading from a blackboard
  READ_BLACKBOARD(SOME_BLACKBOARD_ID, convertSecsToNano(1),
                  (APEX_BYTE *)(&readLoad), &length, &RETURN_CODE);
  if (RETURN_CODE == NO_ERROR) {
    LOGGER(APP, "Read blackboard test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Read blackboard test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
  CLEAR_BLACKBOARD(SOME_BLACKBOARD_ID, &WAIT_RETURN);
  // Tests no error is returned when blocking on an empty blackboard
  LOGGER(APP, "Blocking APERIODIC.");
  START(PROCESS_TIMEOUT_ID, &WAIT_RETURN);
  READ_BLACKBOARD(SOME_BLACKBOARD_ID, convertSecsToNano(1),
                  (APEX_BYTE *)&readLoad, &length, &RETURN_CODE);
  LOGGER(APP, "Timed out");
  if (RETURN_CODE == TIMED_OUT) {
    LOGGER(APP, "Block on empty blackboard test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Block on empty blackboard test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
  TIMED_WAIT(convertSecsToNano(1.5), &WAIT_RETURN);

  // Tests the correct code is returned when not blocking on an empty blackboard
  READ_BLACKBOARD(SOME_BLACKBOARD_ID, convertSecsToNano(0),
                  (APEX_BYTE *)&readLoad, &length, &RETURN_CODE);
  if (RETURN_CODE == NOT_AVAILABLE) {
    LOGGER(APP, "Don't block on empty blackboard test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Don't block on empty blackboard test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }

  // Tests that the correct code is returned when the given timeout is out of
  // range
  DISPLAY_BLACKBOARD(SOME_BLACKBOARD_ID, (APEX_BYTE *)(&myLoad), sizeof(myLoad),
                     &RETURN_CODE);
  READ_BLACKBOARD(SOME_BLACKBOARD_ID, convertSecsToNano(315569261),
                  (APEX_BYTE *)&readLoad, &length, &RETURN_CODE);
  if (RETURN_CODE == INVALID_PARAM) {
    LOGGER(APP, "Timeout too large test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Timeout too large test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }

  // Tests that the correct code is returned when attempting to read from a
  // blackboard that does not exist
  BLACKBOARD_ID_TYPE NONEXISTENT_ID = 10;
  READ_BLACKBOARD(SOME_BLACKBOARD_ID, convertSecsToNano(315569261),
                  (APEX_BYTE *)&readLoad, &length, &RETURN_CODE);
  if (RETURN_CODE == INVALID_PARAM) {
    LOGGER(APP, "No such blackboard test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "No such blackboard test Failed.");
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

  CREATE_BLACKBOARD(
      /*!in */ BLACKBOARD_NAME,
      /*!in */ sizeof(Payload),
      /*!out*/ &SOME_BLACKBOARD_ID,
      /*!out*/ &RETURN_CODE);

  START(PROCESS_APERIODIC_ID, &RETURN_CODE);

  // Setting partition mode. This will start the partition.
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}