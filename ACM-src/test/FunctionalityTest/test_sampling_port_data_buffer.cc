/* This example tests the use of an API used only by the Module Manager and
 * Network Partition */

#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
#include <iomanip>
#include <string.h>
using namespace ACM_USER;
using namespace ACM_INTERNAL;
SAMPLING_PORT_NAME_TYPE SAMPLING_PORT_NAME_S = "/sampling1";
SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID_SOURCE;

SAMPLING_PORT_NAME_TYPE SAMPLING_PORT_NAME_D = "/sampling2";
SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID_DEST;

PROCESS_ID_TYPE PERIODIC_ID;
PROCESS_NAME_TYPE PROCESS_NAME = "Test";

void doSomething() {
  char buf[11] = "4256767890";
  char buf2[11];

  char DataBuffer[sizeof(SYSTEM_TIME_TYPE) + sizeof(MESSAGE_SIZE_TYPE) + 11];

  RETURN_CODE_TYPE RETURN_CODE = NO_ERROR;
  WRITE_SAMPLING_MESSAGE(
      /*in */ SAMPLING_PORT_ID_SOURCE,
      /*in */ (MESSAGE_ADDR_TYPE)&buf[0], /* by reference */
      /*in */ 11,
      /*out*/ &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);

  sleep(2);
  MESSAGE_SIZE_TYPE LENGTH;

  READ_CONTENTS_TO_DATA_BUFFER(
      SAMPLING_PORT_ID_SOURCE, (MESSAGE_ADDR_TYPE)&DataBuffer[0],
      sizeof(SYSTEM_TIME_TYPE) + sizeof(MESSAGE_SIZE_TYPE) + 11, &LENGTH,
      &RETURN_CODE);
  assert(RETURN_CODE == NO_ERROR);

  WRITE_CONTENTS_FROM_DATA_BUFFER(
      SAMPLING_PORT_ID_DEST, (MESSAGE_ADDR_TYPE)&DataBuffer[0],
      sizeof(SYSTEM_TIME_TYPE) + sizeof(MESSAGE_SIZE_TYPE) + 11, &RETURN_CODE);

  assert(RETURN_CODE == NO_ERROR);
  VALIDITY_TYPE VALIDITY;
  READ_SAMPLING_MESSAGE(
      /*in */ SAMPLING_PORT_ID_DEST,
      /*out*/ (MESSAGE_ADDR_TYPE)&buf2[0],
      /*out*/ &LENGTH,
      /*out*/ &VALIDITY,
      /*out*/ &RETURN_CODE);

  assert(RETURN_CODE == NO_ERROR);
  std::cout << "Read size" << LENGTH << " number " << std::string(buf2)
            << std::endl;
  std::cout << "Validity is " << VALIDITY << std::endl;
  assert(RETURN_CODE == NO_ERROR);
}

int main() {

  RETURN_CODE_TYPE RETURN_CODE;

  CREATE_SAMPLING_PORT(
      /*in */ SAMPLING_PORT_NAME_S,
      /*in */ 11,
      /*in */ SOURCE,
      /*in */ convertSecsToNano(5),
      /*out*/ &SAMPLING_PORT_ID_SOURCE,
      /*out*/ &RETURN_CODE);

  CREATE_SAMPLING_PORT(
      /*in */ SAMPLING_PORT_NAME_D,
      /*in */ 11,
      /*in */ DESTINATION,
      /*in */ convertSecsToNano(5),
      /*out*/ &SAMPLING_PORT_ID_DEST,
      /*out*/ &RETURN_CODE);

  PERIODIC_ID = APEX_HELPER_CREATE_PROCESS(
      convertSecsToNano(5),             // SYSTEM_TIME_TYPE PERIOD,
      convertSecsToNano(5),             // SYSTEM_TIME_TYPE TIME_CAPACITY,
      (SYSTEM_ADDRESS_TYPE)doSomething, // SYSTEM_ADDRESS_TYPE ENTRY_POINT,
      0,                                // STACK_SIZE_TYPE STACK_SIZE,
      90,                               // PRIORITY_TYPE BASE_PRIORITY,
      HARD,                             // DEADLINE_TYPE DEADLINE,
      "Test",                           // std::string PROCESS_NAME,
      &RETURN_CODE);                    // RETURN_CODE_TYPE *RETURN_CODE)

  assert(RETURN_CODE == NO_ERROR);
  START(PERIODIC_ID, &RETURN_CODE);
  LOGGER(APP, "Setting Partition State", APP);
  SET_PARTITION_MODE(NORMAL, &RETURN_CODE);

  return 0;
}
