/** \section{Name}
This  file describes the use of a buffer and use of a health manager process.
*/

#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <iomanip>
#include <string.h>
using namespace ACM_USER;

PROCESS_ID_TYPE PERIODIC_PROCESS_ID_1 = 0;
PROCESS_ID_TYPE APERIODIC_PROCESS_ID = 0;
BUFFER_ID_TYPE BUFFER_ID = -1;
char BLACKBOARD_NAME[30] = "testboard";
BLACKBOARD_ID_TYPE BLACKBOARD_ID;

void testmethod(void) {
  PROCESS_ID_TYPE temp_id = 0;
  RETURN_CODE_TYPE RETURN_CODE;
  GET_MY_ID(&temp_id, &RETURN_CODE);

  while (1) {
    LOGGER(APP, "From testmethod: Get my Id is %ld", temp_id);
    LOGGER(INFO, "From testmethod: GOING TO WAIT ON aperiodic EVENT");
    EVENT_ID_TYPE APERIODIC_EVENT_ID;
    char EVENT_NAME[30] = "APERIODIC_PROCESS";
    GET_EVENT_ID(EVENT_NAME, &APERIODIC_EVENT_ID, &RETURN_CODE);
    WAIT_EVENT(APERIODIC_EVENT_ID, convertSecsToNano(0.5), &RETURN_CODE);

    int j = 0;
    MESSAGE_SIZE_TYPE LENGTH;
    RECEIVE_BUFFER(
        /*in */ BUFFER_ID,
        /*in */ INFINITE_TIME_VALUE,
        /*out*/ MESSAGE_ADDR_TYPE(&j),
        /*out*/ &LENGTH,
        /*out*/ &RETURN_CODE);

    assert(RETURN_CODE == NO_ERROR);
    LOGGER(APP, "Received length of message %d. Received message %d", LENGTH,
           j);

    LOGGER(INFO, "From testmethod: OUT OF EVENT. The return code is %d",
           RETURN_CODE);
    // sleep(10);
  }
  LOGGER(INFO, "*********Finished Work in test method************************");
}
void HMprocess(void) {
  while (1) {

    LOGGER(APP, "HM start ");
    BUFFER_ID_TYPE BUFFER_ID;
    RETURN_CODE_TYPE RETURN_CODE;
    ERROR_STATUS_TYPE ERROR_STATUS;
    BLACKBOARD_ID_TYPE TARGET_BBOARD_ID;
    HM_GET_PROCESS_ERROR_MESSAGE(&ERROR_STATUS, &TARGET_BBOARD_ID,
                                 &RETURN_CODE);
    std::cout << ERROR_STATUS.LENGTH << " bboard id " << TARGET_BBOARD_ID
              << std::endl;

    char MESSGAE[10];
    memcpy(MESSGAE, &ERROR_STATUS.MESSAGE, ERROR_STATUS.LENGTH);
    std::cout << " " << MESSGAE << std::endl;
    char display_message[50] = "go ahead. from HM";

    HM_RESPONSE_TYPE HM_RESPONSE;
    HM_RESPONSE.ERROR_CODE = ERROR_STATUS.ERROR_CODE;
    HM_RESPONSE.TIMEOFMESSAGE = currentTimeInNanoSeconds();
    HM_RESPONSE.HM_MESSAGE_DISCRIMINATOR = STRING;
    strcpy((char *)HM_RESPONSE.HM_MESSAGE, display_message);
    HM_RESPONSE.HM_MESSAGE_LENGTH =
        (APEX_UNSIGNED)strlen((char *)HM_RESPONSE.HM_MESSAGE);
    HM_NOTIFY_PROCESS(/*!in*/ ERROR_STATUS.FAILED_PROCESS_ID,
                      /*!in*/ &HM_RESPONSE, /*!out*/ &RETURN_CODE);

    // DISPLAY_BLACKBOARD
    // (TARGET_BBOARD_ID,(APEX_BYTE*)display_message,sizeof(display_message),&RETURN_CODE);
    LOGGER(APP, "HM finish ");
  }
}

void testperiodic(void) {
  RETURN_CODE_TYPE RETURN_CODE;

  LOGGER(INFO, "From testperiodic: GOING TO WAIT ON Aperiodic EVENT");
  EVENT_ID_TYPE APERIODIC_EVENT_ID;
  char EVENT_NAME[30] = "APERIODIC_PROCESS";
  APEX_HELPER_WAIT_EVENT(EVENT_NAME, convertSecsToNano(1), &RETURN_CODE);
  LOGGER(INFO, "From testperiodic:  Out of event:The return code is %d",
         RETURN_CODE);
  if (RETURN_CODE != 6)
    APEX_HELPER_RESET_EVENT(EVENT_NAME, &RETURN_CODE);
  char aperiodiname[30];
  // LOGGER(INFO,"From testperiodic: launching APERIODIC_PROCESS");
  strcpy(aperiodiname, "APERIODIC_PROCESS");
  // DELAYED_START(APERIODIC_PROCESS_ID,convertSecsToNano(2),&RETURN_CODE);
  // APEX_HELPER_START_PROCESS(aperiodiname,&RETURN_CODE);

  LOGGER(INFO, "From testperiodic: Finished Work in test Periodic method");
}
void anotherperiodic(void) {

  RETURN_CODE_TYPE RETURN_CODE;
  // EVENT_ID_TYPE APERIODIC_EVENT_ID;
  sleep(1);
  // LOGGER(INFO,"From anotherperiodic: GOING TO WAIT ON Aperiodic EVENT");
  char EVENT_NAME[30] = "APERIODIC_PROCESS";
  static int i = 0;
  i++;
  LOGGER(APP, "PERIODIC WRITING MESSAGE");

  SEND_BUFFER(
      /*in */ BUFFER_ID,
      /*in */ MESSAGE_ADDR_TYPE(&i), /* by reference */
      /*in */ sizeof(i),
      /*in */ INFINITE_TIME_VALUE,
      /*out*/ &RETURN_CODE);

  assert(RETURN_CODE == NO_ERROR);
  if (i == 2 || i == 4 || i == 6 || i == 8 || i == 10) {
    char MESSGAE[10] = "TEST";
    LOGGER(HMEVENT, "RAISING ERROR");
    char readData[MAX_HM_RESPONSE_MESSAGE_SIZE];

    MESSAGE_SIZE_TYPE readlength;

    HM_RESPONSE_TYPE HM_RESPONSE;

    RAISE_APPLICATION_ERROR_AND_GET_RESPONSE(
        /*!in */ APPLICATION_ERROR,
        /*!in */ MESSAGE_ADDR_TYPE(MESSGAE),
        /*!in */ sizeof(MESSGAE),
        /*!in */ INFINITE_TIME_VALUE, // Added Timeout in response to the
                                      // discussion with Gabor
        /*!out*/ &HM_RESPONSE,
        /*!out*/ &RETURN_CODE);

    // RAISE_APPLICATION_ERROR_AND_GET_RESPONSE (	/*!in
    // */APPLICATION_ERROR,
    ///*!in */ MESSAGE_ADDR_TYPE (MESSGAE),
    ///*!in */ sizeof(MESSGAE),
    ///*!out*/ (APEX_BYTE*)readData,
    ///*!in */ MAX_HM_RESPONSE_MESSAGE_SIZE,
    // INFINITE_TIME_VALUE,
    ///*!out*/ &readlength,
    ///*!out*/ &RETURN_CODE );

    // RAISE_APPLICATION_ERROR (/*!in */ APPLICATION_ERROR,/*!in */
    // MESSAGE_ADDR_TYPE (MESSGAE),/*!in */ sizeof(MESSGAE),/*!out*/
    // &RETURN_CODE );
    /*
    MESSAGE_SIZE_TYPE length;
    READ_BLACKBOARD(BLACKBOARD_ID,INFINITE_TIME_VALUE,(APEX_BYTE*)readData,&length,&RETURN_CODE);
    CLEAR_BLACKBOARD(BLACKBOARD_ID,&RETURN_CODE);
    */
    if (HM_RESPONSE.HM_MESSAGE_DISCRIMINATOR == STRING)
      LOGGER(APP, "FROM another periodic. reply from HM %s",
             (char *)HM_RESPONSE.HM_MESSAGE);
  }
  // LOGGER(INFO,"From anotherperiodic: GOING TO Wait on Aperiodic EVENT");
  // APEX_HELPER_WAIT_EVENT(EVENT_NAME,convertSecsToNano(3),&RETURN_CODE);
  LOGGER(INFO, "From anotherperiodic:  Finished Work in anotherperiodic:The "
               "return code is %d",
         RETURN_CODE);
}

int main(int argc, char *argv[]) {

  SET_PARTITION_NAME("testHM");

  RETURN_CODE_TYPE RETURN_CODE;

  APERIODIC_PROCESS_ID = APEX_HELPER_CREATE_PROCESS(
      INFINITE_TIME_VALUE,             // SYSTEM_TIME_TYPE PERIOD,
      INFINITE_TIME_VALUE,             // SYSTEM_TIME_TYPE TIME_CAPACITY,
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
  SEMAPHORE_ID_TYPE SEMAPHORE_ID;
  CREATE_SEMAPHORE(EVENT_NAME, 1, 1, FIFO, &SEMAPHORE_ID, &RETURN_CODE);

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
      convertSecsToNano(2),                 // SYSTEM_TIME_TYPE PERIOD,
      convertSecsToNano(2),                 // SYSTEM_TIME_TYPE TIME_CAPACITY,
      (SYSTEM_ADDRESS_TYPE)anotherperiodic, // SYSTEM_ADDRESS_TYPE ENTRY_POINT,
      0,                                    // STACK_SIZE_TYPE STACK_SIZE,
      90,                                   // PRIORITY_TYPE BASE_PRIORITY,
      SOFT,                                 // DEADLINE_TYPE DEADLINE,
      "PERIODIC_PROCESS_ID_2",              // std::string PROCESS_NAME,
      &RETURN_CODE);                        // RETURN_CODE_TYPE *RETURN_CODE)
  assert(RETURN_CODE == NO_ERROR);

  BUFFER_NAME_TYPE BUFFER_NAME = "TESTBUFFER";
  CREATE_BUFFER(BUFFER_NAME, sizeof(int), 10, FIFO, &BUFFER_ID, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);

  START(APERIODIC_PROCESS_ID, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);
  START(PERIODIC_PROCESS_ID_1, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);
  START(PERIODIC_PROCESS_ID_2, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);

  HM_DESCRIPTOR_ID_TYPE HM_DESCRIPTOR_ID;
  PROCESS_NAME_TYPE HM_PROCESS_NAME = "HMprocess";

  CREATE_HEALTH_MANAGER_PROCESS(/*in*/ (SYSTEM_ADDRESS_TYPE)HMprocess,
                                /*in*/ HM_PROCESS_NAME,
                                /*in*/ (STACK_SIZE_TYPE)0,
                                /*out*/ &HM_DESCRIPTOR_ID,
                                /*out*/ &RETURN_CODE);

  // CREATE_BLACKBOARD(BLACKBOARD_NAME,200,&BLACKBOARD_ID,&RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);

  REGISTER_PROCESS_WITH_HEALTH_MANAGER(PERIODIC_PROCESS_ID_2, HM_DESCRIPTOR_ID,
                                       &BLACKBOARD_ID, /*out*/ &RETURN_CODE);
  std::cout << BLACKBOARD_ID << " created\n";

  assert(RETURN_CODE == NO_ERROR);
  START_HM(HM_DESCRIPTOR_ID, &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);

  PRINT_CREATED_OBJECT_LIST(&RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);
  // Setting partition mode
  LOGGER(INFO, "Setting Partition State", INFO);
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);
}
