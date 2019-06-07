/*
Test of the SUSPEND_SELF service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;
PROCESS_ID_TYPE PROCESS_PERIODIC_ID;
PROCESS_ID_TYPE PROCESS_APERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_PERIODIC_NAME = "PROCESS_PERIODIC";
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME = "PROCESS_APERIODIC";
RETURN_CODE_TYPE RETURN_CODE;

void PROCESS_APERIODIC(void) {
  LOGGER(APP, "APERIODIC STARTED");
  LOGGER(APP, "Suspending Myself");
  SUSPEND_SELF(convertSecsToNano(2.0), &RETURN_CODE);
}

void PROCESS_PERIODIC(void) {
  RETURN_CODE_TYPE PERIODIC_RETURN;
  LOGGER(APP, "PERIODIC STARTED");

  // start the process PERIODIC
  START(PROCESS_APERIODIC_ID, &RETURN_CODE);

  TIMED_WAIT(convertSecsToNano(0.1), &PERIODIC_RETURN);
  if (RETURN_CODE == NO_ERROR) {
    LOGGER(APP, "Suspend self test succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
    TIMED_WAIT(convertSecsToNano(2.1), &PERIODIC_RETURN);
    // Waits for the interval to time out and tests if the right return code is
    // returned
    if (RETURN_CODE == TIMED_OUT) {
      LOGGER(APP, "Timeout test succeeded!");
      LOGGER(APP, "Return code is: %d", RETURN_CODE);
    } else {
      LOGGER(APP, "Timeout test failed.");
      LOGGER(APP, "Return code is: %d", RETURN_CODE);
    }
  } else {
    LOGGER(APP, "Suspend self test failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }

  LOGGER(APP, "Periodic attempting to suspend myself");
  SUSPEND_SELF(convertSecsToNano(2.0), &RETURN_CODE);

  // Tests if the periodic process correctly failed to suspend itself
  if (RETURN_CODE == INVALID_MODE) {
    LOGGER(APP, "Don't suspend periodic test succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Don't suspend periodic test failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
}

int main() {
  // SET_PARTITION_NAME. Required for Log Identification
  SET_PARTITION_NAME("TESTING_PROCESS_SCHEDULING");

  //! A dummy process fills in the idle gap in the scheduling window.
  // It is necessary to implement suspend service properly.
  // Note: You can get rid of dummy process if you do not use suspend and resume
  // services.
  // APEX_HELPER_CREATE_AND_START_DUMMY_PROCESS(&RETURN_CODE);

  // create the process APERIODIC process
  PROCESS_ATTRIBUTE_TYPE PROCESS_APERIODIC_ATTR;
  PROCESS_APERIODIC_ATTR.BASE_PRIORITY = 91;
  PROCESS_APERIODIC_ATTR.PERIOD = INFINITE_TIME_VALUE; // APeriodic
  PROCESS_APERIODIC_ATTR.TIME_CAPACITY = INFINITE_TIME_VALUE;
  PROCESS_APERIODIC_ATTR.DEADLINE = HARD;
  PROCESS_APERIODIC_ATTR.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE)PROCESS_APERIODIC;
  PROCESS_APERIODIC_ATTR.STACK_SIZE =
      0; // We currently do not support restricting stack size in this emulator
  strncpy(PROCESS_APERIODIC_ATTR.NAME, PROCESS_APERIODIC_NAME, MAX_NAME_LENGTH);
  CREATE_PROCESS(&PROCESS_APERIODIC_ATTR, &PROCESS_APERIODIC_ID, &RETURN_CODE);

  // create the process PERIODIC process
  PROCESS_ATTRIBUTE_TYPE PROCESS_PERIODIC_ATTR;
  PROCESS_PERIODIC_ATTR.BASE_PRIORITY =
      90; // This prioority should be more than the aperiodic priority. Since we
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

  // start the process APERIODIC
  START(PROCESS_PERIODIC_ID, &RETURN_CODE);

  // Setting partition mode. This will start the partition.
  LOGGER(APP, "Setting Partition State", APP);
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}