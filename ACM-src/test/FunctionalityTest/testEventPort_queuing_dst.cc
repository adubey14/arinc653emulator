/*
This code shows the usage of ACM Event port which creates
a seamless abstraction around sampling ports and queueing ports.
It also allows to add an inter module destination via sockets.

This testExample Should be Used with testEventPort_queuing_src

*/

#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <iomanip>
#include <iostream>
using namespace std;
using namespace ACM_USER;
QUEUING_PORT_NAME_TYPE PORT_NAME_S = "sampling1";
QUEUING_PORT_ID_TYPE PORT_ID_SOURCE;

QUEUING_PORT_NAME_TYPE PORT_NAME_D = "sampling2";
QUEUING_PORT_ID_TYPE PORT_ID_DEST;
ACM_EVENT_PORT *READ_EV_PORT;
#define portNumber 3314

void MY_FUNC() {
  RETURN_CODE_TYPE RETURN_CODE;
  while (true) {
    pthread_testcancel();
    READ_EV_PORT->BLOCK_AND_TRANSFER_FROM_SOCKET_READ_PORT(&RETURN_CODE);
    assert(RETURN_CODE == NO_ERROR);
    LOGGER(APP, "Transferred data from socket");
  }
}

ACM_EVENT_PORT *createReadEventPort(RETURN_CODE_TYPE *RETURN_CODE) {
  ACM_EVENT_PORT *ep =
      new ACM_EVENT_PORT(READ, PORT_NAME_S, 200,
                         convertSecsToNano(5), // VALIDITY_PERIOD
                         5, RETURN_CODE, false, true);
  assert(*RETURN_CODE == NO_ERROR);
  READ_EV_PORT = ep;
  ep->ADD_INTER_MODULE_SOURCE_SOCKET(portNumber, RETURN_CODE);
  assert(*RETURN_CODE == NO_ERROR);

  return ep;
}

void runTest() {
  std::cout << "starting runTest\n";
  // char message[200]="This is the sampling-eventport message";
  RETURN_CODE_TYPE RETURN_CODE;

  // MESSAGE_SIZE_TYPE mlength = strlen(message) +1;
  char readData[200];
  bool isValid = true;
  MESSAGE_SIZE_TYPE length;
  // LOGGER(APP,"READ_EV_PORTING FROM EMPTY BBOARD");
  /*
  ( MESSAGE_ADDR_TYPE MESSAGE_ADDR,
                                                     MESSAGE_SIZE_TYPE* LENGTH,
                                                     bool& IS_VALID,
                                                     SYSTEM_TIME_TYPE TIME_OUT,
                                                     RETURN_CODE_TYPE
  *RETURN_CODE);
                                                     */

  READ_EV_PORT->READ_FROM_EVENT_PORT((APEX_BYTE *)readData, &length, &isValid,
                                     INFINITE_TIME_VALUE, &RETURN_CODE);
  LOGGER(APP,
         "Return code from READ_EV_PORTING FROM EMPTY EVENTPORT - BBOARD is %d",
         RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);
  cout << " length is " << length << endl;

  printf("*************************************************************\n");
  printf("\n");
  printf("\n");
  printf("\n");
  printf("%s", readData);
  printf("\n");
  printf("\n");
  printf("\n");
  printf("*************************************************************\n");
}

void runTest2() {
  std::cout << "starting\n";

  while (true) {

    runTest();
  }
  //		delete read;
}

int main(const int argc, char *argv[]) {
  RETURN_CODE_TYPE RETURN_CODE;
  createReadEventPort(&RETURN_CODE);
  PROCESS_ID_TYPE APERIODIC_PROCESS_ID = APEX_HELPER_CREATE_PROCESS(
      INFINITE_TIME_VALUE,          // SYSTEM_TIME_TYPE PERIOD,
      INFINITE_TIME_VALUE,          // SYSTEM_TIME_TYPE TIME_CAPACITY,
      (SYSTEM_ADDRESS_TYPE)MY_FUNC, // SYSTEM_ADDRESS_TYPE ENTRY_POINT,
      0,                            // STACK_SIZE_TYPE STACK_SIZE,
      90,                           // PRIORITY_TYPE BASE_PRIORITY,
      HARD,                         // DEADLINE_TYPE DEADLINE,
      "PROCESS1",                   // std::string PROCESS_NAME,
      &RETURN_CODE);                // RETURN_CODE_TYPE *RETURN_CODE)
  assert(RETURN_CODE == NO_ERROR);

  START(APERIODIC_PROCESS_ID, &RETURN_CODE);

  PROCESS_ID_TYPE APERIODIC_PROCESS_ID2 = APEX_HELPER_CREATE_PROCESS(
      INFINITE_TIME_VALUE,           // SYSTEM_TIME_TYPE PERIOD,
      INFINITE_TIME_VALUE,           // SYSTEM_TIME_TYPE TIME_CAPACITY,
      (SYSTEM_ADDRESS_TYPE)runTest2, // SYSTEM_ADDRESS_TYPE ENTRY_POINT,
      0,                             // STACK_SIZE_TYPE STACK_SIZE,
      90,                            // PRIORITY_TYPE BASE_PRIORITY,
      HARD,                          // DEADLINE_TYPE DEADLINE,
      "PROCESSi2",                   // std::string PROCESS_NAME,
      &RETURN_CODE);                 // RETURN_CODE_TYPE *RETURN_CODE)
  assert(RETURN_CODE == NO_ERROR);
  START(APERIODIC_PROCESS_ID2, &RETURN_CODE);

  // Setting partition mode
  LOGGER(APP, "Setting Partition State", APP);
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  delete (READ_EV_PORT);

  LOGGER(APP, "Delete Succeeded");
  return 0;
}
