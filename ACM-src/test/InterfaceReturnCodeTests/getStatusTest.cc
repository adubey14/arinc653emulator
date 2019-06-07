/*
Test of the GET_PROCESS_STATUS service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;

PROCESS_ID_TYPE PROCESS_PERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_PERIODIC_NAME = "PROCESS_PERIODIC";

void PROCESS_PERIODIC(void) {
  RETURN_CODE_TYPE RETURN_CODE;
  PROCESS_STATUS_TYPE GET_STATUS;

  LOGGER(APP, "Begin PERIODIC.");
  GET_PROCESS_STATUS(PROCESS_PERIODIC_ID, &GET_STATUS, &RETURN_CODE);
  // Tests that no error is returned when correctly getting a process's status
  if (RETURN_CODE == NO_ERROR) {
    LOGGER(APP, "Get process STATUS no error test passed!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Get process STATUS no error test failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }

  PROCESS_ID_TYPE PROCESS_NONEXISTENT_ID = 10;
  GET_PROCESS_STATUS(PROCESS_NONEXISTENT_ID, &GET_STATUS, &RETURN_CODE);
  // Tests that INVALID_PARAM is returned when trying to get the status of a
  // process that does not exist
  if (RETURN_CODE == INVALID_PARAM) {
    LOGGER(APP, "Get process STATUS invalid config test passed!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Get process STATUS invalid config test failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
}

int main() {
  // SET_PARTITION_NAME. Required for Log Identification
  SET_PARTITION_NAME("TESTING_PROCESS_SCHEDULING");
  RETURN_CODE_TYPE RETURN_CODE;
  //! A dummy process fills in the idle gap in the scheduling window.
  // It is necessary to implement suspend service properly.
  // Note: You can get rid of dummy process if you do not use suspend and resume
  // services.
  // APEX_HELPER_CREATE_AND_START_DUMMY_PROCESS(&RETURN_CODE);

  // create the process PERIODIC process
  PROCESS_ATTRIBUTE_TYPE PROCESS_PERIODIC_ATTR;
  PROCESS_PERIODIC_ATTR.BASE_PRIORITY =
      91; // This prioority should be more than the aperiodic priority. Since we
          // are starting the aperiodic process, it is possible that it will
          // take the resources away from the periodic process
  PROCESS_PERIODIC_ATTR.PERIOD = convertSecsToNano(5);
  PROCESS_PERIODIC_ATTR.TIME_CAPACITY = convertSecsToNano(5);
  PROCESS_PERIODIC_ATTR.DEADLINE = HARD;
  PROCESS_PERIODIC_ATTR.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE)PROCESS_PERIODIC;
  PROCESS_PERIODIC_ATTR.STACK_SIZE =
      0; // We currently do not support restricting stack size in this emulator
  strncpy(PROCESS_PERIODIC_ATTR.NAME, PROCESS_PERIODIC_NAME, MAX_NAME_LENGTH);
  CREATE_PROCESS(&PROCESS_PERIODIC_ATTR, &PROCESS_PERIODIC_ID, &RETURN_CODE);

  // start the process PERIODIC
  START(PROCESS_PERIODIC_ID, &RETURN_CODE);

  // Setting partition mode. This will start the partition.
  LOGGER(APP, "Setting Partition State", APP);
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}