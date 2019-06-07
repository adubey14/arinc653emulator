/* This program tests the intermodule sampling channel used by the network
 * partition. use it with test_sampling_interModule_dst */

#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
#include <iomanip>
#include <string.h>
using namespace ACM_USER;
using namespace ACM_INTERNAL;
int main() {
  try {

    RETURN_CODE_TYPE RETURN_CODE;

    SAMPLING_CHANNEL channel("testChannel", INTER_MODULE_SRC);
    channel.ADD_SOURCE("/temp1", 11, convertSecsToNano(5), &RETURN_CODE);
    channel.ADD_DESTINATION_URL("localhost", 2314, &RETURN_CODE);
    channel.SET_INITIALIZED();
    assert(RETURN_CODE == NO_ERROR);
    //////////////
    LOGGER(INFO, "CHANNEL FINISHED");
    SAMPLING_PORT_HELPER SP("/temp1", 11, PORT_SOURCE_DEST,
                            convertSecsToNano(5), &RETURN_CODE, O_CREAT);
    assert(RETURN_CODE == NO_ERROR);
    char buf[11] = "here";
    int i = 0;
    while (true) {
      i++;
      sprintf(buf, "%d", i);
      sleep(4);
      SP.WRITE_SAMPLING_MESSAGE((MESSAGE_ADDR_TYPE)&buf[0], 11, &RETURN_CODE);
      channel.MOVE_MESSAGES(&RETURN_CODE);
    }
    return 0;
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
