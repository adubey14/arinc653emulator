#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;
PROCESS_ID_TYPE PROCESS_PERIODIC_ID;
PROCESS_ID_TYPE PROCESS_APERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_PERIODIC_NAME = "PROCESS_PERIODIC";
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME = "PROCESS_APERIODIC";

void PROCESS_PERIODIC2(void) {
  long counter = 0;

  RETURN_CODE_TYPE RETURN_CODE = NO_ERROR;
  while (1) {

    counter++;

    LOGGER(USER, "In PERIODIC2.");
    PERIODIC_WAIT(&RETURN_CODE);
    assert(RETURN_CODE == NO_ERROR);
  }
}

void PROCESS_PERIODIC(void) {
  static long counter = 0;
  counter++;
  RETURN_CODE_TYPE RETURN_CODE;
  LOGGER(USER, "In PERIODIC. Triggering APeriodic");
  if (counter == 1) {
    START(PROCESS_APERIODIC_ID, &RETURN_CODE);
    LOGGER(USER, "START(PROCESS_APERIODIC_ID,&RETURN_CODE);");
    ACM_ASSERT(RETURN_CODE == NO_ERROR);
  }
  if (counter == 2) {
    LOGGER(USER, "SUSPEND(PROCESS_APERIODIC_ID,&RETURN_CODE);");
    SUSPEND(PROCESS_APERIODIC_ID, &RETURN_CODE);
    LOGGER(USER, "OUT OF SUSPEND(PROCESS_APERIODIC_ID,&RETURN_CODE);");
    ACM_ASSERT(RETURN_CODE == NO_ERROR);
  }
  if (counter == 3) {
    RESUME(PROCESS_APERIODIC_ID, &RETURN_CODE);
    LOGGER(USER, "RESUME(PROCESS_APERIODIC_ID,&RETURN_CODE);");
    ACM_ASSERT(RETURN_CODE == NO_ERROR);
  }
  if (counter == 4) {
    STOP(PROCESS_APERIODIC_ID, &RETURN_CODE);
    LOGGER(USER, "STOP(PROCESS_APERIODIC_ID,&RETURN_CODE);");
    ACM_ASSERT(RETURN_CODE == NO_ERROR);
  }
  if (counter == 5) {
    DELAYED_START(PROCESS_APERIODIC_ID, convertSecsToNano(1.0), &RETURN_CODE);
    LOGGER(USER, "DELAYED_START(PROCESS_APERIODIC_ID,convertSecsToNano(1.0),&"
                 "RETURN_CODE);");
    ACM_ASSERT(RETURN_CODE == NO_ERROR);
  }
}

void PROCESS_APERIODIC(void) {
  LOGGER(USER, "APERIODIC STARTED");
  while (true) {
    USE_INFINITE_LOOP_PROTECTION; // Always put this statement in infinite
                                  // loops. It creates a thread cancellation
                                  // point which is required in order to stop
                                  // the process if necessary.
    RETURN_CODE_TYPE RETURN_CODE;
    TIMED_WAIT(convertSecsToNano(2.0), &RETURN_CODE);

    LOGGER(USER, "APERIODIC Triggered");
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
  // ACM_ASSERT(RETURN_CODE==NO_ERROR);

  // create the process PERIODIC process
  PROCESS_ATTRIBUTE_TYPE PROCESS_PERIODIC_ATTR;
  PROCESS_PERIODIC_ATTR.BASE_PRIORITY =
      91; // This prioority should be more than the aperiodic priority. Since we
          // are starting the aperiodic process, it is possible that it will
          // take the resources away from the periodic process
  PROCESS_PERIODIC_ATTR.PERIOD = convertSecsToNano(5);
  PROCESS_PERIODIC_ATTR.TIME_CAPACITY = convertSecsToNano(5);
  PROCESS_PERIODIC_ATTR.DEADLINE = HARD;
  PROCESS_PERIODIC_ATTR.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE)PROCESS_PERIODIC2;
  PROCESS_PERIODIC_ATTR.STACK_SIZE =
      0; // We currently do not support restricting stack size in this emulator
  strncpy(PROCESS_PERIODIC_ATTR.NAME, PROCESS_PERIODIC_NAME, MAX_NAME_LENGTH);
  CREATE_PROCESS(&PROCESS_PERIODIC_ATTR, &PROCESS_PERIODIC_ID, &RETURN_CODE);
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

  // start the process PERIODIC
  START(PROCESS_PERIODIC_ID, &RETURN_CODE);
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

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
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

  // Setting partition mode. This will start the partition.
  LOGGER(USER, "Setting Partition State");
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}
