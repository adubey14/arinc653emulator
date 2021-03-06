/*
This file tests the use of create process and scheduling them with events.
Also shows the use of delayed start. All wait events will timeout in this code.
*/

#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <iomanip>
#include <string.h>
using namespace ACM_USER;

PROCESS_ID_TYPE PERIODIC_PROCESS_ID_1 = 0;
PROCESS_ID_TYPE APERIODIC_PROCESS_ID = 0;
void testmethod(void) {
  PROCESS_ID_TYPE temp_id = 0;
  RETURN_CODE_TYPE RETURN_CODE;
  GET_MY_ID(&temp_id, &RETURN_CODE);

  LOGGER(USER, "From testmethod: Get my Id is %ld", temp_id);
  LOGGER(USER, "From testmethod: GOING TO WAIT ON aperiodic EVENT");

  EVENT_ID_TYPE APERIODIC_EVENT_ID;
  char EVENT_NAME[30] = "APERIODIC_PROCESS";
  GET_EVENT_ID(EVENT_NAME, &APERIODIC_EVENT_ID, &RETURN_CODE);
  WAIT_EVENT(APERIODIC_EVENT_ID, convertSecsToNano(2), &RETURN_CODE);

  LOGGER(USER, "From testmethod: OUT OF EVENT. The return code is %s",
         RETURN_CODE_TO_STRING(RETURN_CODE));

  LOGGER(USER, "*********Finished Work in test method************************");
}

void testperiodic(void) {
  RETURN_CODE_TYPE RETURN_CODE;
  LOGGER(USER, "From testperiodic: GOING TO WAIT ON Aperiodic EVENT");
  char EVENT_NAME[30] = "APERIODIC_PROCESS";
  // APEX_HELPER_WAIT_EVENT is a combination of get_event_id and wait_event
  APEX_HELPER_WAIT_EVENT(EVENT_NAME, convertSecsToNano(5), &RETURN_CODE);
  LOGGER(USER, "From testperiodic:  Out of event:The return code is %s",
         RETURN_CODE_TO_STRING(RETURN_CODE));
  if (RETURN_CODE != TIMED_OUT)
    APEX_HELPER_RESET_EVENT(EVENT_NAME, &RETURN_CODE);
  char aperiodiname[30];
  LOGGER(USER, "From testperiodic: launching APERIODIC_PROCESS");
  strcpy(aperiodiname, "APERIODIC_PROCESS");
  DELAYED_START(APERIODIC_PROCESS_ID, convertSecsToNano(2), &RETURN_CODE);
  LOGGER(USER, "From testperiodic: Finished Work in test Periodic method");
}
void anotherperiodic(void) {

  RETURN_CODE_TYPE RETURN_CODE;

  sleep(1);
  // LOGGER(USER,"From anotherperiodic: GOING TO WAIT ON Aperiodic EVENT");
  char EVENT_NAME[30] = "APERIODIC_PROCESS";
  LOGGER(USER, "From anotherperiodic: GOING TO Wait on Aperiodic EVENT");
  APEX_HELPER_WAIT_EVENT(EVENT_NAME, convertSecsToNano(3), &RETURN_CODE);
  LOGGER(USER, "From anotherperiodic:  Finished Work in anotherperiodic:The "
               "return code is %d",
         RETURN_CODE);
}

int main() {

  RETURN_CODE_TYPE RETURN_CODE;

  APERIODIC_PROCESS_ID = APEX_HELPER_CREATE_PROCESS(
      INFINITE_TIME_VALUE,             // SYSTEM_TIME_TYPE PERIOD,
      convertSecsToNano(10),           // SYSTEM_TIME_TYPE TIME_CAPACITY,
      (SYSTEM_ADDRESS_TYPE)testmethod, // SYSTEM_ADDRESS_TYPE ENTRY_POINT,
      0,                               // STACK_SIZE_TYPE STACK_SIZE,
      90,                              // PRIORITY_TYPE BASE_PRIORITY,
      HARD,                            // DEADLINE_TYPE DEADLINE,
      "APERIODIC_PROCESS",             // std::string PROCESS_NAME,
      &RETURN_CODE);                   // RETURN_CODE_TYPE *RETURN_CODE)
  assert(RETURN_CODE == NO_ERROR);

  char EVENT_NAME[30] = "APERIODIC_PROCESS";
  EVENT_ID_TYPE APERIODIC_EVENT_ID;
  CREATE_EVENT(EVENT_NAME, &APERIODIC_EVENT_ID, &RETURN_CODE);
  // create Event APERIODIC_PROCESS

  PERIODIC_PROCESS_ID_1 = APEX_HELPER_CREATE_PROCESS(
      convertSecsToNano(10),             // SYSTEM_TIME_TYPE PERIOD,
      convertSecsToNano(10),             // SYSTEM_TIME_TYPE TIME_CAPACITY,
      (SYSTEM_ADDRESS_TYPE)testperiodic, // SYSTEM_ADDRESS_TYPE ENTRY_POINT,
      0,                                 // STACK_SIZE_TYPE STACK_SIZE,
      90,                                // PRIORITY_TYPE BASE_PRIORITY,
      HARD,                              // DEADLINE_TYPE DEADLINE,
      "PERIODIC_PROCESS_ID_1",           // std::string PROCESS_NAME,
      &RETURN_CODE);                     // RETURN_CODE_TYPE *RETURN_CODE)
  assert(RETURN_CODE == NO_ERROR);

  PROCESS_ID_TYPE PERIODIC_PROCESS_ID_2 = APEX_HELPER_CREATE_PROCESS(
      convertSecsToNano(10),                // SYSTEM_TIME_TYPE PERIOD,
      convertSecsToNano(10),                // SYSTEM_TIME_TYPE TIME_CAPACITY,
      (SYSTEM_ADDRESS_TYPE)anotherperiodic, // SYSTEM_ADDRESS_TYPE ENTRY_POINT,
      0,                                    // STACK_SIZE_TYPE STACK_SIZE,
      90,                                   // PRIORITY_TYPE BASE_PRIORITY,
      SOFT,                                 // DEADLINE_TYPE DEADLINE,
      "PERIODIC_PROCESS_ID_2",              // std::string PROCESS_NAME,
      &RETURN_CODE);                        // RETURN_CODE_TYPE *RETURN_CODE)
  assert(RETURN_CODE == NO_ERROR);

  // START(APERIODIC_PROCESS_ID,&RETURN_CODE);
  // assert(RETURN_CODE==NO_ERROR);
  START(PERIODIC_PROCESS_ID_1, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);
  START(PERIODIC_PROCESS_ID_2, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);

  // Setting partition mode
  LOGGER(USER, "Setting Partition State", USER);
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);
}
