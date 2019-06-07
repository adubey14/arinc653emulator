/*
This file tests the use of suspend and resume interfaces.
The periodic process suspends and resume the two aperiodic processes.
It also prints the current clock resolution.
*/

#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <iomanip>

using namespace ACM_USER;

PROCESS_ID_TYPE PERIODIC_PROCESS_ID_1 = 0;
PROCESS_ID_TYPE APERIODIC_PROCESS_ID = 0;
PROCESS_ID_TYPE APERIODIC_PROCESS_ID2 = 0;

void testmethod(void) {
  PROCESS_ID_TYPE temp_id = 0;
  RETURN_CODE_TYPE RETURN_CODE;
  int i = 0;
  while (true) {
    pthread_testcancel();
    i++;
    LOGGER(USER, "APERIODIC i %d:  ", i);
    sleep(1);
  }
  LOGGER(USER, "Finished aperiodic1");
}

void testmethod2(void) {
  PROCESS_ID_TYPE temp_id = 0;
  RETURN_CODE_TYPE RETURN_CODE;
  int i = 0;
  while (true) {
    pthread_testcancel();
    i++;
    LOGGER(USER, "APERIODIC Process 2 j %d:  ", i);
    sleep(1);
  }
  LOGGER(USER, "Finished aperiodic1");
}

void testperiodic(void) {
  static int i = 0;
  i++;
  RETURN_CODE_TYPE RETURN_CODE;
  if (i == 5) {
    LOGGER(USER, "GOING to SUSPEND APERIODIC 2 ");
    SUSPEND(APERIODIC_PROCESS_ID2, &RETURN_CODE);
    LOGGER(USER, "RETURN CODE is %s", RETURN_CODE_TO_STRING(RETURN_CODE));
  }

  if (i == 8) {
    LOGGER(USER, "GOING to SUSPEND APERIODIC Process 1 ");
    SUSPEND(APERIODIC_PROCESS_ID, &RETURN_CODE);
    LOGGER(USER, "RETURN CODE is %d", RETURN_CODE);
    LOGGER(USER, "RETURN CODE is %s", RETURN_CODE_TO_STRING(RETURN_CODE));
  }
  if (i == 10) {
    LOGGER(USER, "GOING to Resume APERIODIC ");
    RESUME(APERIODIC_PROCESS_ID, &RETURN_CODE);
    LOGGER(USER, "RETURN CODE is %s", RETURN_CODE_TO_STRING(RETURN_CODE));
  }
  LOGGER(USER, "From testperiodic: Iteration Finished");
}

int main() {

  SET_PARTITION_NAME("testsuspend");
  SYSTEM_TIMESPEC res;
  clock_getres(CLOCK_REALTIME, &res);

  LOGGER(USER, "Testapp Started. The clock_realtime resolution is %ld sec and "
               "%ld nanoseconds",
         res.tv_sec, res.tv_nsec);
  RETURN_CODE_TYPE RETURN_CODE;

  APERIODIC_PROCESS_ID = APEX_HELPER_CREATE_PROCESS(
      INFINITE_TIME_VALUE,             // SYSTEM_TIME_TYPE PERIOD,
      INFINITE_TIME_VALUE,             // SYSTEM_TIME_TYPE TIME_CAPACITY,
      (SYSTEM_ADDRESS_TYPE)testmethod, // SYSTEM_ADDRESS_TYPE ENTRY_POINT,
      0,                               // STACK_SIZE_TYPE STACK_SIZE,
      90,                              // PRIORITY_TYPE BASE_PRIORITY,
      SOFT,                            // DEADLINE_TYPE DEADLINE,
      "APERIODIC_PROCESS",             // std::string PROCESS_NAME,
      &RETURN_CODE);                   // RETURN_CODE_TYPE *RETURN_CODE)
  assert(RETURN_CODE == NO_ERROR);

  APERIODIC_PROCESS_ID2 = APEX_HELPER_CREATE_PROCESS(
      INFINITE_TIME_VALUE,              // SYSTEM_TIME_TYPE PERIOD,
      INFINITE_TIME_VALUE,              // SYSTEM_TIME_TYPE TIME_CAPACITY,
      (SYSTEM_ADDRESS_TYPE)testmethod2, // SYSTEM_ADDRESS_TYPE ENTRY_POINT,
      0,                                // STACK_SIZE_TYPE STACK_SIZE,
      90,                               // PRIORITY_TYPE BASE_PRIORITY,
      SOFT,                             // DEADLINE_TYPE DEADLINE,
      "APERIODIC_PROCESS2",             // std::string PROCESS_NAME,
      &RETURN_CODE);                    // RETURN_CODE_TYPE *RETURN_CODE)
  assert(RETURN_CODE == NO_ERROR);

  // create Event APERIODIC_PROCESS

  PERIODIC_PROCESS_ID_1 = APEX_HELPER_CREATE_PROCESS(
      convertSecsToNano(2),              // SYSTEM_TIME_TYPE PERIOD,
      convertSecsToNano(2),              // SYSTEM_TIME_TYPE TIME_CAPACITY,
      (SYSTEM_ADDRESS_TYPE)testperiodic, // SYSTEM_ADDRESS_TYPE ENTRY_POINT,
      0,                                 // STACK_SIZE_TYPE STACK_SIZE,
      90,                                // PRIORITY_TYPE BASE_PRIORITY,
      HARD,                              // DEADLINE_TYPE DEADLINE,
      "PERIODIC_PROCESS_ID_1",           // std::string PROCESS_NAME,
      &RETURN_CODE);                     // RETURN_CODE_TYPE *RETURN_CODE)
  assert(RETURN_CODE == NO_ERROR);

  START(APERIODIC_PROCESS_ID, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);

  START(APERIODIC_PROCESS_ID2, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);

  START(PERIODIC_PROCESS_ID_1, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);

  // Setting partition mode
  LOGGER(INFO, "Setting Partition State", INFO);
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}
