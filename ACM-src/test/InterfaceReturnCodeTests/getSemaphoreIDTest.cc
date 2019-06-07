/*
Test of the GET_SEMAPHORE_ID service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;

SEMAPHORE_NAME_TYPE semName = "mysem";
SEMAPHORE_ID_TYPE mysemid;
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME = "PROCESS_APERIODIC";
PROCESS_ID_TYPE PROCESS_APERIODIC_ID;

void PROCESS_APERIODIC(void) {
  RETURN_CODE_TYPE RETURN_CODE;
  SEMAPHORE_ID_TYPE GET_ID;
  // Tests that no error is returned when correctly getting a semaphore's ID
  GET_SEMAPHORE_ID(
      /*!in */ semName,
      /*!out*/ &GET_ID,
      /*!out*/ &RETURN_CODE);
  if (RETURN_CODE == NO_ERROR) {
    LOGGER(APP, "Get semaphore ID test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Get semaphore ID test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
  // Tests that the correct code is returned when attempting to get the ID of a
  // nonexistent semaphore
  SEMAPHORE_NAME_TYPE nonexistentsem = "NonexistentSemaphore";
  GET_SEMAPHORE_ID(
      /*!in */ nonexistentsem,
      /*!out*/ &GET_ID,
      /*!out*/ &RETURN_CODE);
  if (RETURN_CODE == INVALID_PARAM) {
    LOGGER(APP, "Nonexistent semaphore ID test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Nonexistent semaphore ID test Failed.");
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

  START(PROCESS_APERIODIC_ID, &RETURN_CODE);

  CREATE_SEMAPHORE(
      /*!in */ semName,
      /*!in */ SEMAPHORE_VALUE_TYPE(0),
      /*!in */ SEMAPHORE_VALUE_TYPE(1),
      /*!in */ FIFO,
      /*!out*/ &mysemid,
      /*!out*/ &RETURN_CODE);

  // Setting partition mode. This will start the partition.
  LOGGER(APP, "Setting Partition State", APP);
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}