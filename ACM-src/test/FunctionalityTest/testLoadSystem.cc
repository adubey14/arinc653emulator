#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <iomanip>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace ACM_USER;

PROCESS_ID_TYPE PERIODIC_PROCESS_ID_1 = 0;
PROCESS_ID_TYPE APERIODIC_ID = 0;
PROCESS_NAME_TYPE APERIODIC_NAME = "APERIODIC_PROCESS_ID_1";
bool finishExec = false;
void catcher(int sig) { finishExec = true; }

void aperiodic(void) {
  // LOGGER(APP,"From anotherperiodic: GOING TO WAIT ON Aperiodic EVENT");
  finishExec = false;
  struct itimerval value;
  int which = ITIMER_REAL;
  value.it_interval.tv_sec = 0;  /* Zero seconds */
  value.it_interval.tv_usec = 0; /* Two hundred milliseconds */
  value.it_value.tv_sec = 1;     /* Zero seconds */
  value.it_value.tv_usec = 0;    /* 1 second */
  setitimer(which, &value, NULL);
  double i = 33333.333;
  while (!finishExec) {
    double j = i;
    double k = sqrt(j);
    double c = sqrt(k);
    pthread_testcancel();
  }
  value.it_interval.tv_sec = 0;   /* Zero seconds */
  value.it_interval.tv_usec = 0;  /* Two hundred milliseconds */
  value.it_value.tv_sec = 0;      /* Zero seconds */
  value.it_value.tv_usec = 0;     /* 1 second */
  setitimer(which, &value, NULL); // disable the timer

  LOGGER(APP, "From anotherperiodic:  Finished Work in anotherperiodic");
}

void anotherperiodic(void) {
  PROCESS_RESPONSE_TYPE response;
  MESSAGE_SIZE_TYPE LENGTH;
  RETURN_CODE_TYPE RETURN_CODE;
  // start aperiodic
  APEX_HELPER_START_PROCESS_AND_WAIT_FOR_RESPONSE(
      /*!in */ APERIODIC_NAME,
      /*!out*/ (MESSAGE_ADDR_TYPE)&response,
      /*!in */ (MESSAGE_SIZE_TYPE)sizeof(PROCESS_RESPONSE_TYPE),
      /*!out*/ &LENGTH,
      /*!out*/ &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);
  if (LENGTH > 0) {
    // std::cout<<"Received Response. Processing
    // Done="<<response.PROCESSING_COMPLETED<<std::endl;
    std::cout << "RESPONDING ENTITY" << response.RESPONDING_ENTITY << std::endl;
    std::cout << "TIME OF MESSAGE " << response.TIMEOFMESSAGE << std::endl;
  } else {
    std::cout << "LENGTH is 0\n";
  }
}
int main(int argc, char **argv) {
  CONFIGLOG(INFO);
  signal(SIGALRM, &catcher);
  if (argc != 2) {
    std::cout << "Usage " << argv[0]
              << " [cpuNumber in integer e.g. 0 1 or 2 ...]" << std::endl;
    return -1;
  }
  int cpunumber;
  std::string inputchoice(argv[1]);
  if (!from_string(cpunumber, inputchoice, std::dec)) {
    std::cout << "You gave wrong choice:" << argv[1]
              << ":It should be an integer" << std::endl;
    return -1;
  }

  RETURN_CODE_TYPE RETURN_CODE;
  char EVENT_NAME[30] = "APERIODIC_PROCESS";
  EVENT_ID_TYPE APERIODIC_EVENT_ID;
  CREATE_EVENT(EVENT_NAME, &APERIODIC_EVENT_ID, &RETURN_CODE);
  cpu_set_t mask;
  CPU_ZERO(&mask);           /* clear mask */
  CPU_SET(cpunumber, &mask); /* processors 0 */
  unsigned int len = sizeof(mask);
  if (sched_setaffinity(0, len, &mask) < 0) {
    LOGGER(FATAL, "Error when trying to set CPU affinity: %s", strerror(errno));
  }

  // create Event APERIODIC_PROCESS

  APERIODIC_ID = APEX_HELPER_CREATE_PROCESS(
      INFINITE_TIME_VALUE,            // SYSTEM_TIME_TYPE PERIOD,
      convertSecsToNano(0.25),        // SYSTEM_TIME_TYPE TIME_CAPACITY,
      (SYSTEM_ADDRESS_TYPE)aperiodic, // SYSTEM_ADDRESS_TYPE ENTRY_POINT,
      0,                              // STACK_SIZE_TYPE STACK_SIZE,
      90,                             // PRIORITY_TYPE BASE_PRIORITY,
      HARD,                           // DEADLINE_TYPE DEADLINE,
      "APERIODIC_PROCESS_ID_1",       // std::string PROCESS_NAME,
      &RETURN_CODE);                  // RETURN_CODE_TYPE *RETURN_CODE)
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
  START(PERIODIC_PROCESS_ID_2, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);
  // Setting partition mode
  PRINT_CREATED_OBJECT_LIST(&RETURN_CODE);
  LOGGER(APP, "Setting Partition State", APP);
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}
