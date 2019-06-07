#include <ACM/USER/ACM_USER_INCLUDE.h>
using namespace ACM_USER;
PROCESS_ID_TYPE PROCESS_PERIODIC_ID_1;
PROCESS_ID_TYPE PROCESS_PERIODIC_ID_2;
PROCESS_NAME_TYPE PROCESS_PERIODIC_NAME_1 = "PROCESS_PERIODIC_1";
PROCESS_NAME_TYPE PROCESS_PERIODIC_NAME_2 = "PROCESS_PERIODIC_2";
SEMAPHORE_NAME_TYPE semName = "mysem";
SEMAPHORE_ID_TYPE mysemid;

void PROCESS_PERIODIC_1(void) {
  RETURN_CODE_TYPE RETURN_CODE;
  LOGGER(USER, " PROCESS_PERIODIC_1: GOING TO WAIT");
  SEMAPHORE_STATUS_TYPE sem_status;
  GET_SEMAPHORE_STATUS(mysemid, &sem_status, &RETURN_CODE);

  LOGGER(USER,
         "PROCESS_PERIODIC_1: SEM current value %ld, waiting processes: %ld",
         sem_status.CURRENT_VALUE, sem_status.WAITING_PROCESSES);

  WAIT_SEMAPHORE(
      /*!in */ mysemid,
      /*!in */ convertSecsToNano(0.5),
      /*!out*/ &RETURN_CODE);
  LOGGER(USER, " PROCESS_PERIODIC_1: Out of WAIT");

  if (RETURN_CODE == TIMED_OUT) {
    LOGGER(USER, "PROCESS_PERIODIC_1: WAIT SEM REQUEST TIMED OUT");
    return;
  }
  if (RETURN_CODE == NO_ERROR) {

    GET_SEMAPHORE_STATUS(mysemid, &sem_status, &RETURN_CODE);

    LOGGER(USER,
           "PROCESS_PERIODIC_1: SEM current value %ld, waiting processes: %ld",
           sem_status.CURRENT_VALUE, sem_status.WAITING_PROCESSES);

    LOGGER(USER, "PROCESS_PERIODIC_1: Releasing Semaphore");
    // SIGNAL_SEMAPHORE(mysemid,&RETURN_CODE);
  }
}

void PROCESS_PERIODIC_2(void) {
  RETURN_CODE_TYPE RETURN_CODE;
  LOGGER(USER, " PROCESS_PERIODIC_2: GOING TO WAIT");
  WAIT_SEMAPHORE(
      /*!in */ mysemid,
      /*!in */ convertSecsToNano(3),
      /*!out*/ &RETURN_CODE);
  LOGGER(USER, " PROCESS_PERIODIC_2: Out of WAIT");

  if (RETURN_CODE == TIMED_OUT) {
    LOGGER(USER, "PROCESS_PERIODIC_2: WAIT SEM REQUEST TIMED OUT");
    return;
  }
  if (RETURN_CODE == NO_ERROR) {
    LOGGER(USER, "PROCESS_PERIODIC_2: Releasing Semaphore");
    SIGNAL_SEMAPHORE(mysemid, &RETURN_CODE);
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
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

  CREATE_SEMAPHORE(
      /*!in */ semName,
      /*!in */ SEMAPHORE_VALUE_TYPE(1),
      /*!in */ SEMAPHORE_VALUE_TYPE(1),
      /*!in */ FIFO,
      /*!out*/ &mysemid,
      /*!out*/ &RETURN_CODE);

  // create the process PERIODIC process
  PROCESS_ATTRIBUTE_TYPE PROCESS_PERIODIC_ATTR;
  PROCESS_PERIODIC_ATTR.BASE_PRIORITY = 90;
  PROCESS_PERIODIC_ATTR.PERIOD = convertSecsToNano(1);
  PROCESS_PERIODIC_ATTR.TIME_CAPACITY = convertSecsToNano(1);
  PROCESS_PERIODIC_ATTR.DEADLINE = HARD;
  PROCESS_PERIODIC_ATTR.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE)PROCESS_PERIODIC_1;
  PROCESS_PERIODIC_ATTR.STACK_SIZE =
      0; // We currently do not support restricting stack size in this emulator
  strncpy(PROCESS_PERIODIC_ATTR.NAME, PROCESS_PERIODIC_NAME_1, MAX_NAME_LENGTH);
  CREATE_PROCESS(&PROCESS_PERIODIC_ATTR, &PROCESS_PERIODIC_ID_1, &RETURN_CODE);
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

  // start the process PERIODIC
  START(PROCESS_PERIODIC_ID_1, &RETURN_CODE);
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

  // create the process APERIODIC process
  PROCESS_ATTRIBUTE_TYPE PROCESS_PERIODIC_ATTR_2;
  PROCESS_PERIODIC_ATTR_2.BASE_PRIORITY = 90;
  PROCESS_PERIODIC_ATTR_2.PERIOD = convertSecsToNano(4);
  PROCESS_PERIODIC_ATTR_2.TIME_CAPACITY = convertSecsToNano(4);
  PROCESS_PERIODIC_ATTR_2.DEADLINE = HARD;
  PROCESS_PERIODIC_ATTR_2.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE)PROCESS_PERIODIC_2;
  PROCESS_PERIODIC_ATTR_2.STACK_SIZE =
      0; // We currently do not support restricting stack size in this emulator
  strncpy(PROCESS_PERIODIC_ATTR_2.NAME, PROCESS_PERIODIC_NAME_2,
          MAX_NAME_LENGTH);
  CREATE_PROCESS(&PROCESS_PERIODIC_ATTR_2, &PROCESS_PERIODIC_ID_2,
                 &RETURN_CODE);
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

  // start the process PERIODIC
  START(PROCESS_PERIODIC_ID_2, &RETURN_CODE);
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

  // Setting partition mode. This will start the partition.
  LOGGER(USER, "Setting Partition State");
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}