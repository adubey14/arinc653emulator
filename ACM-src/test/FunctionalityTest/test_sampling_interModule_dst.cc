/* This program tests the intermodule sampling channel used by the network
 * partition. use it with test_sampling_interModule_src */

#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
#include <iomanip>
#include <string.h>
using namespace ACM_USER;
using namespace ACM_INTERNAL;
int main() {
  try {

    RETURN_CODE_TYPE RETURN_CODE;

    SAMPLING_CHANNEL channel(std::string("testChannel2"), INTER_MODULE_DST,
                             2314);
    channel.SET_MAX_SIZE_OF_MESSAGE(11, &RETURN_CODE);
    assert(RETURN_CODE == NO_ERROR);
    channel.ADD_DESTINATION("/temp2", 11, convertSecsToNano(5), &RETURN_CODE);
    assert(RETURN_CODE == NO_ERROR);
    channel.SET_INITIALIZED();

    //////////////
    LOGGER(APP, "CHANNEL FINISHED");
    SAMPLING_PORT_HELPER SP2("/temp2", 11, PORT_SOURCE_DEST,
                             convertSecsToNano(5), &RETURN_CODE, O_CREAT);
    char buf2[11];
    VALIDITY_TYPE VALIDITY;
    MESSAGE_SIZE_TYPE LENGTH;
    SAMPLING_PORT_STATUS_TYPE SAMPLING_PORT_STATUS;
    ///////////////////////////////////
    while (true) {
      sleep(4);
      LOGGER(APP, "Trying To Move");
      channel.MOVE_MESSAGES(&RETURN_CODE);
      LOGGER(APP, "Finished Move");
      assert(RETURN_CODE == NO_ERROR);
      LOGGER(APP, "DOING COPY");
      assert(RETURN_CODE == NO_ERROR);
      SP2.READ_SAMPLING_MESSAGE((MESSAGE_ADDR_TYPE)&buf2[0], &LENGTH, &VALIDITY,
                                &RETURN_CODE);
      std::cout << "Read size" << LENGTH << " number " << std::string(buf2)
                << std::endl;
      SP2.GET_SAMPLING_PORT_STATUS(&SAMPLING_PORT_STATUS, &RETURN_CODE);
      std::cout << "Last Message Validity"
                << SAMPLING_PORT_STATUS.LAST_MSG_VALIDITY << std::endl;
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
