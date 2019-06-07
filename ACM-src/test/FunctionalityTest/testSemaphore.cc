/*
This file tests the use of semaphore interfaces and shows the use of print
created object list.
*/

#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <iomanip>
#include <string.h>

using namespace ACM_USER;

SEMAPHORE_NAME_TYPE semName = "mysem";
SEMAPHORE_ID_TYPE mysemid;
PROCESS_ID_TYPE PERIODIC_ID, PERIODIC_ID2;

void periodic(void) {
  RETURN_CODE_TYPE RETURN_CODE;

  LOGGER(APP, " PERIODIC: GOING TO WAIT");
  WAIT_SEMAPHORE(
      /*!in */ mysemid,
      /*!in */ convertSecsToNano(1),
      /*!out*/ &RETURN_CODE);
  LOGGER(APP, " PERIODIC: Out of WAIT");
}

void periodic2(void) {
  RETURN_CODE_TYPE RETURN_CODE;

  LOGGER(APP, " PERIODIC2: SIGNALLING");
  SIGNAL_SEMAPHORE(
      /*!in */ mysemid,
      /*!out*/ &RETURN_CODE);
  LOGGER(APP, " PERIODIC2: FINISHED SIGNALLING");
}

int main() {
  RETURN_CODE_TYPE RETURN_CODE;

  PERIODIC_ID = APEX_HELPER_CREATE_PROCESS(
      convertSecsToNano(2),          // SYSTEM_TIME_TYPE PERIOD,
      convertSecsToNano(2),          // SYSTEM_TIME_TYPE TIME_CAPACITY,
      (SYSTEM_ADDRESS_TYPE)periodic, // SYSTEM_ADDRESS_TYPE ENTRY_POINT,
      0,                             // STACK_SIZE_TYPE STACK_SIZE,
      90,                            // PRIORITY_TYPE BASE_PRIORITY,
      HARD,                          // DEADLINE_TYPE DEADLINE,
      "PERIODIC_PROCESS_ID_1",       // std::string PROCESS_NAME,
      &RETURN_CODE);                 // RETURN_CODE_TYPE *RETURN_CODE)

  PERIODIC_ID2 = APEX_HELPER_CREATE_PROCESS(
      convertSecsToNano(4),           // SYSTEM_TIME_TYPE PERIOD,
      convertSecsToNano(4),           // SYSTEM_TIME_TYPE TIME_CAPACITY,
      (SYSTEM_ADDRESS_TYPE)periodic2, // SYSTEM_ADDRESS_TYPE ENTRY_POINT,
      0,                              // STACK_SIZE_TYPE STACK_SIZE,
      90,                             // PRIORITY_TYPE BASE_PRIORITY,
      HARD,                           // DEADLINE_TYPE DEADLINE,
      "PERIODIC_PROCESS_ID_2",        // std::string PROCESS_NAME,
      &RETURN_CODE);                  // RETURN_CODE_TYPE *RETURN_CODE)

  CREATE_SEMAPHORE(
      /*!in */ semName,
      /*!in */ SEMAPHORE_VALUE_TYPE(0),
      /*!in */ SEMAPHORE_VALUE_TYPE(32),
      /*!in */ FIFO,
      /*!out*/ &mysemid,
      /*!out*/ &RETURN_CODE);

  START(PERIODIC_ID, &RETURN_CODE);
  START(PERIODIC_ID2, &RETURN_CODE);

  LOGGER(APP, "GOING TO WAIT. Result should be timed out");
  WAIT_SEMAPHORE(
      /*!in */ mysemid,
      /*!in */ convertSecsToNano(3),
      /*!out*/ &RETURN_CODE);

  LOGGER(APP, "OUT OF WAIT. Return Code is %s ",
         RETURN_CODE_TO_STRING(RETURN_CODE));

  /* shows the use of print created object list */

  PRINT_CREATED_OBJECT_LIST(&RETURN_CODE);
  LOGGER(APP, "Setting Partition State", APP);
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}
