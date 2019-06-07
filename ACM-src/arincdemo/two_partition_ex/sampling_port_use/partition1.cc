#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <common.h>
using namespace ACM_USER;
using namespace ACM_EXAMPLE;

PROCESS_ID_TYPE PROCESS_PERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_PERIODIC_NAME = "PROCESS_PERIODIC";
SAMPLING_PORT_NAME_TYPE SAMPLING_PORT_NAME = "T_SAMPLE_PORT1";
SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID;

void PROCESS_PERIODIC(void) {

  RETURN_CODE_TYPE RETURN_CODE;
  LOGGER(USER, "In PERIODIC. Sending Message to Sampling PORT");
  static Payload myLoad;
  myLoad.data = 0.2;
  myLoad.time = currentTimeInNanoSeconds();
  WRITE_SAMPLING_MESSAGE(
      /*in */ SAMPLING_PORT_ID,
      /*in */ (MESSAGE_ADDR_TYPE)&myLoad, /* by reference */
      /*in */ sizeof(myLoad),
      /*out*/ &RETURN_CODE);
}

int main() {

  // SET_PARTITION_NAME. Required for Log Identification
  SET_PARTITION_NAME("Partition1");

  RETURN_CODE_TYPE RETURN_CODE;

  //! Create Port.
  CREATE_SAMPLING_PORT(
      /*in */ SAMPLING_PORT_NAME,
      /*in */ sizeof(Payload),
      /*in */ SOURCE,
      /*in */ convertSecsToNano(4),
      /*out*/ &SAMPLING_PORT_ID,
      /*out*/ &RETURN_CODE);

  ACM_ASSERT(RETURN_CODE == NO_ERROR);

  //! A dummy process fills in the idle gap in the scheduling window.
  // It is necessary to implement suspend service properly.
  // Note: You can get rid of dummy process if you do not use suspend and resume
  // services.
  ////APEX_HELPER_CREATE_AND_START_DUMMY_PROCESS(&RETURN_CODE);
  // ACM_ASSERT(RETURN_CODE==NO_ERROR);

  // create the process PERIODIC process
  PROCESS_ATTRIBUTE_TYPE PROCESS_PERIODIC_ATTR;
  PROCESS_PERIODIC_ATTR.BASE_PRIORITY = 90;
  PROCESS_PERIODIC_ATTR.PERIOD = convertSecsToNano(4);
  PROCESS_PERIODIC_ATTR.TIME_CAPACITY = convertSecsToNano(4);
  PROCESS_PERIODIC_ATTR.DEADLINE = HARD;
  PROCESS_PERIODIC_ATTR.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE)PROCESS_PERIODIC;
  PROCESS_PERIODIC_ATTR.STACK_SIZE =
      0; // We currently do not support restricting stack size in this emulator
  strncpy(PROCESS_PERIODIC_ATTR.NAME, PROCESS_PERIODIC_NAME, MAX_NAME_LENGTH);
  CREATE_PROCESS(&PROCESS_PERIODIC_ATTR, &PROCESS_PERIODIC_ID, &RETURN_CODE);
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

  // start the process PERIODIC
  START(PROCESS_PERIODIC_ID, &RETURN_CODE);
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

  // Setting partition mode. This will start the partition.
  LOGGER(USER, "Setting Partition State");
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}
