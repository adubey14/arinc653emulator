/*
Test of the CREATE_BLACKBOARD service
*/
#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;
BLACKBOARD_NAME_TYPE bbName = "mybb";
BLACKBOARD_ID_TYPE mybbid;
PROCESS_ID_TYPE PROCESS_APERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME = "PROCESS_APERIODIC";

void PROCESS_APERIODIC(void) {
  RETURN_CODE_TYPE RETURN_CODE;
  BLACKBOARD_ID_TYPE mybbid3;
  BLACKBOARD_NAME_TYPE bbName3 = "mybb3";
  CREATE_BLACKBOARD(
      /*!in */ bbName3,
      /*!in */ MESSAGE_SIZE_TYPE(1),
      /*!out*/ &mybbid3,
      /*!out*/ &RETURN_CODE);
  if (RETURN_CODE == INVALID_MODE) {
    LOGGER(APP, "Invalid partition mode blackboard test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Invalid partition mode blackboard test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
}

int main() {
  // SET_PARTITION_NAME. Required for Log Identification
  SET_PARTITION_NAME("TESTING_PROCESS_SCHEDULING");

  RETURN_CODE_TYPE RETURN_CODE;
  // Tests that no error is returned when correctly creating a blackboard
  CREATE_BLACKBOARD(
      /*!in */ bbName,
      /*!in */ MESSAGE_SIZE_TYPE(1),
      /*!out*/ &mybbid,
      /*!out*/ &RETURN_CODE);
  if (RETURN_CODE == NO_ERROR) {
    LOGGER(APP, "Create blackboard test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Create blackboard test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }
  // Tests that the correct code is returned when attempting to create a
  // blackboard with  the name of an existing blackboard
  BLACKBOARD_ID_TYPE secondbbID;
  CREATE_BLACKBOARD(
      /*!in */ bbName,
      /*!in */ MESSAGE_SIZE_TYPE(1),
      /*!out*/ &secondbbID,
      /*!out*/ &RETURN_CODE);
  if (RETURN_CODE == NO_ACTION) {
    LOGGER(APP, "Create redundant blackboard test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Create redundant blackboard test Failed.");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  }

  // Tests that the correct code is returned when the given maximum message size
  // is out of range
  BLACKBOARD_ID_TYPE mybbid2;
  BLACKBOARD_NAME_TYPE bbName2 = "mybb2";
  CREATE_BLACKBOARD(
      /*!in */ bbName2,
      /*!in */ MESSAGE_SIZE_TYPE(2147483648),
      /*!out*/ &mybbid2,
      /*!out*/ &RETURN_CODE);
  if (RETURN_CODE == INVALID_PARAM) {
    LOGGER(APP, "Max out of range blackboard test Succeeded!");
    LOGGER(APP, "Return code is: %d", RETURN_CODE);
  } else {
    LOGGER(APP, "Max out of range blackboard test Failed.");
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
