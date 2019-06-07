#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <common.h>
using namespace ACM_USER;
using namespace ACM_EXAMPLE;

PROCESS_ID_TYPE PROCESS_APERIODIC_ID;

PROCESS_NAME_TYPE PROCESS_APERIODIC_NAME = "PROCESS_APERIODIC";
QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME =
    "T_QUEUE_PORT_DEST"; // This port will be visible if you list message queues
                         // on your system
// Note you need to create a different port with different name for partition 2.
QUEUING_PORT_ID_TYPE QUEUING_PORT_ID;

void PROCESS_APERIODIC(void) {
  static Payload data; // Just used to prevent multiple instantiations. You can
                       // make this non static.
  RETURN_CODE_TYPE RETURN_CODE;

  while (true) {
    USE_INFINITE_LOOP_PROTECTION; // Always put this statement in infinite
                                  // loops. It creates a thread cancellation
                                  // point which is required in order to stop
                                  // the process if necessary.

    MESSAGE_SIZE_TYPE LENGTH;
    RECEIVE_QUEUING_MESSAGE(
        /*in */ QUEUING_PORT_ID,
        /*in */ INFINITE_TIME_VALUE, // block till data arrives
        /*out*/ (MESSAGE_ADDR_TYPE)&data,
        /*out*/ &LENGTH,
        /*out*/ &RETURN_CODE);
    LOGGER(USER, "APERIODIC Triggered");
    if (LENGTH == sizeof(Payload)) {
      LOGGER(USER, "Read message %f at %lld", data.data, data.time);
    }
  }
}

int main() {
  // SET_PARTITION_NAME. Required for Log Identification
  SET_PARTITION_NAME("Partition 2");

  RETURN_CODE_TYPE RETURN_CODE;

  CREATE_QUEUING_PORT(
      /*in */ QUEUING_PORT_NAME,
      /*in */ sizeof(Payload),
      /*in */ 20, // maximum 30 . Should match the source port's parameters
      /*in */ DESTINATION,
      /*in */ FIFO,
      /*out*/ &QUEUING_PORT_ID,
      /*out*/ &RETURN_CODE);
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

  //! A dummy process fills in the idle gap in the scheduling window.
  // It is necessary to implement suspend service properly.
  // Note: You can get rid of dummy process if you do not use suspend and resume
  // services.
  // APEX_HELPER_CREATE_AND_START_DUMMY_PROCESS(&RETURN_CODE);
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

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
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

  // start the process APERIODIC
  START(PROCESS_APERIODIC_ID, &RETURN_CODE);
  ACM_ASSERT(RETURN_CODE == NO_ERROR);

  // Setting partition mode. This will start the partition.
  LOGGER(USER, "Setting Partition State");
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}