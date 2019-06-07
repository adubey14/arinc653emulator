/*
Test of the Get_MY_ID service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;

PROCESS_ID_TYPE PROCESS_APERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME = "PROCESS_APERIODIC";

void PROCESS_APERIODIC(void) {
  RETURN_CODE_TYPE RETURN_CODE;
  PROCESS_ID_TYPE GET_ID;

  LOGGER(APP, "Starting APERIODIC.");
  GET_MY_ID(&GET_ID, &RETURN_CODE);
  // Tests that no error is returned when correctly getting a processses own ID
  if (GET_ID == PROCESS_APERIODIC_ID) {
    LOGGER(APP, "Get process ID no error test passed!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Get process ID no error test failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
}

int main() {
  RETURN_CODE_TYPE RETURN_CODE;
  // SET_PARTITION_NAME. Required for Log Identification
  SET_PARTITION_NAME("TESTING_PROCESS_SCHEDULING");

  PROCESS_ID_TYPE GET_ID;

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

  START(PROCESS_APERIODIC_ID, &RETURN_CODE);

  GET_MY_ID(&GET_ID, &RETURN_CODE);
  // Tests that INVALID_MODE is returned if the process calling GET_MY_ID has no
  // ID
  if (RETURN_CODE == INVALID_MODE) {
    LOGGER(APP, "Has no ID test passed!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Has no ID test failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }

  // Setting partition mode. This will start the partition.
  LOGGER(APP, "Setting Partition State", APP);
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}