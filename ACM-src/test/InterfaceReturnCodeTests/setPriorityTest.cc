/*
Test of the SET_PRIORITY service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;
PROCESS_ID_TYPE PROCESS_PERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_PERIODIC_NAME = "PROCESS_PERIODIC";
PROCESS_ID_TYPE PROCESS_APERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME = "PROCESS_APERIODIC";

void PROCESS_PERIODIC(void) { LOGGER(APP, "In PERIODIC."); }

void PROCESS_APERIODIC(void) {
  RETURN_CODE_TYPE RETURN_CODE;

  // Correct set priority
  SET_PRIORITY(PROCESS_PERIODIC_ID, 90, &RETURN_CODE);
  if (RETURN_CODE == NO_ERROR) {
    LOGGER(APP, "Correct set priority test succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Correct set priority test failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }

  PROCESS_ID_TYPE PROCESS_NONEXISTENT_ID = 10;
  // Attempt to set priority for a process that doesn't exist
  SET_PRIORITY(PROCESS_NONEXISTENT_ID, 90, &RETURN_CODE);
  if (RETURN_CODE == INVALID_PARAM) {
    LOGGER(APP, "Nonexistent process set priority test succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Nonexistent prosses set priority test failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }

  // Priority level is out of bounds
  SET_PRIORITY(PROCESS_PERIODIC_ID, 150, &RETURN_CODE);
  if (RETURN_CODE == INVALID_PARAM) {
    LOGGER(APP, "Invalid prioroity set priority test succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Invalid priority set priority test failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }

  STOP(PROCESS_PERIODIC_ID, &RETURN_CODE);
  // Attempt to set priority level for a dormant process
  SET_PRIORITY(PROCESS_PERIODIC_ID, 90, &RETURN_CODE);
  if (RETURN_CODE == INVALID_MODE) {
    LOGGER(APP, "Invalid mode set priority test succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Invalid mode set priority test failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
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

  // create the process PERIODIC process
  PROCESS_ATTRIBUTE_TYPE PROCESS_APERIODIC_ATTR;
  PROCESS_APERIODIC_ATTR.BASE_PRIORITY =
      91; // This prioority should be more than the aperiodic priority. Since we
          // are starting the aperiodic process, it is possible that it will
          // take the resources away from the periodic process
  PROCESS_APERIODIC_ATTR.PERIOD = INFINITE_TIME_VALUE; // Aperiodic
  PROCESS_APERIODIC_ATTR.TIME_CAPACITY = convertSecsToNano(5);
  PROCESS_APERIODIC_ATTR.DEADLINE = HARD;
  PROCESS_APERIODIC_ATTR.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE)PROCESS_APERIODIC;
  PROCESS_APERIODIC_ATTR.STACK_SIZE =
      0; // We currently do not support restricting stack size in this emulator
  strncpy(PROCESS_APERIODIC_ATTR.NAME, PROCESS_APERIODIC_NAME, MAX_NAME_LENGTH);
  CREATE_PROCESS(&PROCESS_APERIODIC_ATTR, &PROCESS_APERIODIC_ID, &RETURN_CODE);

  // start the process PERIODIC
  START(PROCESS_PERIODIC_ID, &RETURN_CODE);

  // start the process APERIODIC
  START(PROCESS_APERIODIC_ID, &RETURN_CODE);

  // Setting partition mode. This will start the partition.
  LOGGER(APP, "Setting Partition State", APP);
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);
  return 0;
}