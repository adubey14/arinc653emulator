/*
This file tests the use of ACM timer interface, which implements a discrete
event scheduler.
testaperiodic adds the time values.
*/

#include <ACM/USER/ACM_TIMER.h>
#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <iomanip>
#include <string.h>
using namespace ACM_USER;
using namespace ACM_HELPERS;
ACM_TIMER *mytimer;
PROCESS_ID_TYPE APERIODIC_PROCESS_ID_1 = 0;
PROCESS_ID_TYPE APERIODIC_PROCESS_ID = 0;
void mymethod(void) { LOGGER(APP, "Timer Callback"); }
void testaperiodic(void) {

  LOGGER(USER, "From testperiodic: Finished Work in test Periodic method");
  sleep(2);
  SYSTEM_TIMESPEC timeout;
  SYSTEM_TIMESPEC now = currentTime();
  timeout = addTimeNano(now, convertSecsToNano(5));
  RETURN_CODE_TYPE return_code;
  mytimer->addTimeValue(timeout, &return_code);
  assert(return_code == NO_ERROR);
  timeout = addTimeNano(now, convertSecsToNano(2));
  mytimer->addTimeValue(timeout, &return_code);
  assert(return_code == NO_ERROR);
  sleep(2);
  LOGGER(APP, "Starting timer");
  // sleep(2);
  mytimer->startTimer(&return_code);
  assert(return_code == NO_ERROR);
  sleep(10);
  LOGGER(APP, "Stopping timer");
  mytimer->stopTimer(&return_code);
  assert(return_code == NO_ERROR);
}
void anotherperiodic(void) {

  LOGGER(APP, "From anotherperiodic:  Finished Work in anotherperiodic:");
}

int main() {

  try {

    RETURN_CODE_TYPE RETURN_CODE;
    mytimer = new ACM_TIMER("name", mymethod);

    APERIODIC_PROCESS_ID_1 = APEX_HELPER_CREATE_PROCESS(
        INFINITE_TIME_VALUE,                // SYSTEM_TIME_TYPE PERIOD,
        INFINITE_TIME_VALUE,                // SYSTEM_TIME_TYPE TIME_CAPACITY,
        (SYSTEM_ADDRESS_TYPE)testaperiodic, // SYSTEM_ADDRESS_TYPE ENTRY_POINT,
        0,                                  // STACK_SIZE_TYPE STACK_SIZE,
        90,                                 // PRIORITY_TYPE BASE_PRIORITY,
        HARD,                               // DEADLINE_TYPE DEADLINE,
        "APERIODIC_PROCESS_ID_1",           // std::string PROCESS_NAME,
        &RETURN_CODE);                      // RETURN_CODE_TYPE *RETURN_CODE)
    assert(RETURN_CODE == NO_ERROR);

    PROCESS_ID_TYPE PERIODIC_PROCESS_ID_2 = APEX_HELPER_CREATE_PROCESS(
        convertSecsToNano(10), // SYSTEM_TIME_TYPE PERIOD,
        convertSecsToNano(10), // SYSTEM_TIME_TYPE TIME_CAPACITY,
        (SYSTEM_ADDRESS_TYPE)
            anotherperiodic,     // SYSTEM_ADDRESS_TYPE ENTRY_POINT,
        0,                       // STACK_SIZE_TYPE STACK_SIZE,
        90,                      // PRIORITY_TYPE BASE_PRIORITY,
        SOFT,                    // DEADLINE_TYPE DEADLINE,
        "PERIODIC_PROCESS_ID_2", // std::string PROCESS_NAME,
        &RETURN_CODE);           // RETURN_CODE_TYPE *RETURN_CODE)
    assert(RETURN_CODE == NO_ERROR);

    START(APERIODIC_PROCESS_ID_1, &RETURN_CODE);
    assert(RETURN_CODE == NO_ERROR);
    START(PERIODIC_PROCESS_ID_2, &RETURN_CODE);
    assert(RETURN_CODE == NO_ERROR);
    // Setting partition mode
    LOGGER(APP, "Setting Partition State", APP);
    SET_PARTITION_MODE(NORMAL, &RETURN_CODE);
    delete (mytimer);
  } // End of Try
  catch (ACM_BASE_EXCEPTION_TYPE &ex) {

    LOGGER(ERROR, "ACM_BASE_EXCEPTION: %s", ex.getMessage().c_str());

    return -1;
  } catch (...) {
    LOGGER(ERROR, "reached here. Exception Received");
    return -1;
  }
  return 0;
}
