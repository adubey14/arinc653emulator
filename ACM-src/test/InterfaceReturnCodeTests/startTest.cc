/*
Test of the START service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;
PROCESS_ID_TYPE PROCESS_APERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME = "PROCESS_APERIODIC";
PROCESS_ID_TYPE PROCESS_APERIODIC_ID2;
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME2 = "PROCESS_APERIODIC2";
RETURN_CODE_TYPE RETURN_CODE;

void PROCESS_APERIODIC(void) {
  LOGGER(APP, "APERIODIC STARTED");
  while (true) {
    USE_INFINITE_LOOP_PROTECTION; // Always put this statement in infinite
                                  // loops. It creates a thread cancellation
                                  // point which is required in order to stop
                                  // the process if necessary.
    TIMED_WAIT(convertSecsToNano(2.0), &RETURN_CODE);

    LOGGER(APP, "APERIODIC Triggered");
  }
}

void PROCESS_APERIODIC2(void) {
  LOGGER(APP, "APERIODIC STARTED");
  // Tests that no error is returned when correctly starting a process
  START(PROCESS_APERIODIC_ID, &RETURN_CODE);
  if (RETURN_CODE == NO_ERROR) {
    LOGGER(APP, "Correct start test passed!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Correct start test failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }

  // Tests that the corect code is returned if the specified process is not
  // dormant
  START(PROCESS_APERIODIC_ID, &RETURN_CODE);
  if (RETURN_CODE == NO_ACTION) {
    LOGGER(APP, "Already started test passed!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Already started test failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }

  PROCESS_ID_TYPE PROCESS_NONEXISTENT_ID = 10;
  // Tests that the correct code is returned if the specified process does not
  // exist
  START(PROCESS_NONEXISTENT_ID, &RETURN_CODE);
  if (RETURN_CODE == INVALID_PARAM) {
    LOGGER(APP, "No such process test passed!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "No such process test failed.");
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

  // create the process APERIODIC2 process
  PROCESS_ATTRIBUTE_TYPE PROCESS_APERIODIC_ATTR2;
  PROCESS_APERIODIC_ATTR2.BASE_PRIORITY = 90;
  PROCESS_APERIODIC_ATTR2.PERIOD = INFINITE_TIME_VALUE; // APeriodic
  PROCESS_APERIODIC_ATTR2.TIME_CAPACITY = INFINITE_TIME_VALUE;
  PROCESS_APERIODIC_ATTR2.DEADLINE = HARD;
  PROCESS_APERIODIC_ATTR2.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE)PROCESS_APERIODIC2;
  PROCESS_APERIODIC_ATTR2.STACK_SIZE =
      0; // We currently do not support restricting stack size in this emulator
  strncpy(PROCESS_APERIODIC_ATTR2.NAME, PROCESS_APERIODIC_NAME2,
          MAX_NAME_LENGTH);
  CREATE_PROCESS(&PROCESS_APERIODIC_ATTR2, &PROCESS_APERIODIC_ID2,
                 &RETURN_CODE);
  START(PROCESS_APERIODIC_ID2, &RETURN_CODE);

  // Setting partition mode. This will start the partition.
  LOGGER(APP, "Setting Partition State", APP);
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}