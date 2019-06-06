/** \section{Name}
This  file describes the creation and usage of a queuing port.
*/

#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
#include <iomanip>
#include <string.h>
using namespace ACM_USER;
using namespace ACM_INTERNAL;

int main(){
try{

RETURN_CODE_TYPE RETURN_CODE;
QUEUE_CHANNEL my_channel("my_channel",INTER_MODULE_SRC);
my_channel.ADD_SOURCE_QUEUE("temp2",10,10,&RETURN_CODE);
my_channel.ADD_DESTINATION_URL("localhost",3314,&RETURN_CODE);
//my_channel.ADD_DESTINATION_QUEUE("temp4",10,10,&RETURN_CODE);
assert(RETURN_CODE==NO_ERROR);
my_channel.SET_INITIALIZED();

assert(RETURN_CODE==NO_ERROR);
///channel created
QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME="temp2";
QUEUING_PORT_ID_TYPE QUEUING_PORT_ID;
CREATE_QUEUING_PORT (
			/*in */ QUEUING_PORT_NAME,
			/*in */ 10,
			/*in */ 10,
			/*in */ SOURCE,
			/*in */ FIFO,
			/*out*/ &QUEUING_PORT_ID,
			/*out*/ &RETURN_CODE );

//create channel

int i =4;
MESSAGE_SIZE_TYPE LENGTH;
SEND_QUEUING_MESSAGE (
			/*in */ QUEUING_PORT_ID,
			/*in */ (MESSAGE_ADDR_TYPE)&i, /* by reference */
			/*in */ sizeof(i),
			/*in */ 0,
			/*out*/ &RETURN_CODE);
i++;
SEND_QUEUING_MESSAGE (
			/*in */ QUEUING_PORT_ID,
			/*in */ (MESSAGE_ADDR_TYPE)&i, /* by reference */
			/*in */ sizeof(i),
			/*in */ 0,
			/*out*/ &RETURN_CODE);
i++;
SEND_QUEUING_MESSAGE (
			/*in */ QUEUING_PORT_ID,
			/*in */ (MESSAGE_ADDR_TYPE)&i, /* by reference */
			/*in */ sizeof(i),
			/*in */ 0,
			/*out*/ &RETURN_CODE);
//i++;
//SEND_QUEUING_MESSAGE (
//			/*in */ QUEUING_PORT_ID,
//			/*in */ (MESSAGE_ADDR_TYPE)&i, /* by reference */
//			/*in */ sizeof(i),
//			/*in */ 0,
//			/*out*/ &RETURN_CODE);

my_channel.MOVE_MESSAGES(&RETURN_CODE);
assert(RETURN_CODE==NO_ERROR);
return 0;
} // End of Try
catch(ACM_BASE_EXCEPTION_TYPE& ex){	
LOGGER( ERROR, "ACM_BASE_EXCEPTION: %s",ex.getMessage().c_str());	
return -1;
}
catch (...){
LOGGER( ERROR,"reached here. Exception Received");
return -1;

}
return 0;
}
