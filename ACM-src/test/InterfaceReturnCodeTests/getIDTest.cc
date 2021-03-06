/*
Test of the GET_PROCESS_ID service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;

PROCESS_ID_TYPE PROCESS_APERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME = "PROCESS_APERIODIC";
RETURN_CODE_TYPE RETURN_CODE;

void PROCESS_APERIODIC(void) {
  PROCESS_ID_TYPE GET_ID;

  // tests that no error is returned when correctly getting a process's ID
  GET_PROCESS_ID(PROCESS_APERIODIC_NAME, &GET_ID, &RETURN_CODE);
  if (RETURN_CODE == NO_ERROR) {
    LOGGER(APP, "Get process ID no error test passed!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Get process ID no error test failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }

  // tests that INVALID_CONFIG is returned when attmpting to get the ID of a
  // process that does not exist
  PROCESS_NAME_TYPE PROCESS_NONEXISTENT_NAME = "PROCESS_NONEXISTENT";
  GET_PROCESS_ID(PROCESS_NONEXISTENT_NAME, &GET_ID, &RETURN_CODE);
  if (RETURN_CODE == INVALID_CONFIG) {
    LOGGER(APP, "Get process ID invalid config test passed!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Get process ID invalid config test failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
}

int main() {
  // SET_PARTITION_NAME. Required for Log Identification
  SET_PARTITION_NAME("TESTING_PROCESS_SCHEDULING");

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

  // start the process PERIODIC
  START(PROCESS_APERIODIC_ID, &RETURN_CODE);

  // Setting partition mode. This will start the partition.
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}