/*
Test of the CREATE_SEMAPHORE service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;
SEMAPHORE_NAME_TYPE semName = "mysem";
SEMAPHORE_ID_TYPE mysemid;
PROCESS_ID_TYPE PROCESS_APERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME = "PROCESS_APERIODIC";

void PROCESS_APERIODIC(void) {
  RETURN_CODE_TYPE RETURN_CODE;
  SEMAPHORE_ID_TYPE mysemid5;
  SEMAPHORE_NAME_TYPE semName5 = "mysem5";
  CREATE_SEMAPHORE(
      /*!in */ semName5,
      /*!in */ SEMAPHORE_VALUE_TYPE(1),
      /*!in */ SEMAPHORE_VALUE_TYPE(1),
      /*!in */ FIFO,
      /*!out*/ &mysemid5,
      /*!out*/ &RETURN_CODE);
  if (RETURN_CODE == INVALID_MODE) {
    LOGGER(APP, "Invalid partition mode semaphore test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Invalid partition mode semaphore test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
}

int main() {
  // SET_PARTITION_NAME. Required for Log Identification
  SET_PARTITION_NAME("TESTING_PROCESS_SCHEDULING");

  RETURN_CODE_TYPE RETURN_CODE;
  // Tests that no error is returned when correctly creating a semaphore
  CREATE_SEMAPHORE(
      /*!in */ semName,
      /*!in */ SEMAPHORE_VALUE_TYPE(0),
      /*!in */ SEMAPHORE_VALUE_TYPE(1),
      /*!in */ FIFO,
      /*!out*/ &mysemid,
      /*!out*/ &RETURN_CODE);
  if (RETURN_CODE == NO_ERROR) {
    LOGGER(APP, "Create semaphore test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Create semaphore test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
  // Tests that the correct code is returned when attempting to create a
  // semaphore with  the name of an existing semaphore
  SEMAPHORE_ID_TYPE secondSemID;
  CREATE_SEMAPHORE(
      /*!in */ semName,
      /*!in */ SEMAPHORE_VALUE_TYPE(0),
      /*!in */ SEMAPHORE_VALUE_TYPE(1),
      /*!in */ FIFO,
      /*!out*/ &secondSemID,
      /*!out*/ &RETURN_CODE);
  if (RETURN_CODE == NO_ACTION) {
    LOGGER(APP, "Create redundant semaphore test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Create redundant semaphore test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
  // Tests that the correct code is returned when the given current value is
  // greater than the given max value
  SEMAPHORE_ID_TYPE mysemid2;
  SEMAPHORE_NAME_TYPE semName2 = "mysem2";
  CREATE_SEMAPHORE(
      /*!in */ semName2,
      /*!in */ SEMAPHORE_VALUE_TYPE(5),
      /*!in */ SEMAPHORE_VALUE_TYPE(1),
      /*!in */ FIFO,
      /*!out*/ &mysemid2,
      /*!out*/ &RETURN_CODE);
  if (RETURN_CODE == INVALID_PARAM) {
    LOGGER(APP, "Current > max semaphore test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Current > max semaphore test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
  // Tests that the correct code is returned when the given maximum value is out
  // of range
  SEMAPHORE_ID_TYPE mysemid3;
  SEMAPHORE_NAME_TYPE semName3 = "mysem3";
  CREATE_SEMAPHORE(
      /*!in */ semName3,
      /*!in */ SEMAPHORE_VALUE_TYPE(5),
      /*!in */ SEMAPHORE_VALUE_TYPE(32768),
      /*!in */ FIFO,
      /*!out*/ &mysemid3,
      /*!out*/ &RETURN_CODE);
  if (RETURN_CODE == INVALID_PARAM) {
    LOGGER(APP, "Max out of range semaphore test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Max out of range semaphore test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
  // Tests that the correct code is returned when an invalid queuing discipline
  // is given
  SEMAPHORE_ID_TYPE mysemid4;
  SEMAPHORE_NAME_TYPE semName4 = "mysem4";
  QUEUING_DISCIPLINE_TYPE INVALID_DISCIPLINE;
  CREATE_SEMAPHORE(
      /*!in */ semName4,
      /*!in */ SEMAPHORE_VALUE_TYPE(1),
      /*!in */ SEMAPHORE_VALUE_TYPE(1),
      /*!in */ INVALID_DISCIPLINE,
      /*!out*/ &mysemid4,
      /*!out*/ &RETURN_CODE);
  if (RETURN_CODE == INVALID_PARAM) {
    LOGGER(APP, "Invalid queuing discipline semaphore test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Invalid queuing discipline semaphore test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }

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
