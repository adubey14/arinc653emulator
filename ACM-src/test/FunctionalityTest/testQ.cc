/** \section{Name}
This  file describes the creation and usage of a queuing port.
it also test channels, which are used by the module manager.
*/


#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
#include <iomanip>
#include <string.h>
using namespace ACM_USER;
using namespace ACM_INTERNAL;

int main(){

	SET_PARTITION_NAME("testQ");
	RETURN_CODE_TYPE RETURN_CODE;
	QUEUE_CHANNEL my_channel("my_channel");
	my_channel.ADD_SOURCE_QUEUE("temp2",10,10,&RETURN_CODE);
	my_channel.ADD_DESTINATION_QUEUE("temp4",10,10,&RETURN_CODE);
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
	QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME2="temp4";
	QUEUING_PORT_ID_TYPE QUEUING_PORT_ID2;
	CREATE_QUEUING_PORT (
		/*in */ QUEUING_PORT_NAME2,
		/*in */ 10,
		/*in */ 10,
		/*in */ DESTINATION,
		/*in */ FIFO,
		/*out*/ &QUEUING_PORT_ID2,
		/*out*/ &RETURN_CODE );
	assert(RETURN_CODE==NO_ERROR);


	//create channel

	int i =4;
	MESSAGE_SIZE_TYPE LENGTH;
	SEND_QUEUING_MESSAGE (
		/*in */ QUEUING_PORT_ID,
		/*in */ (MESSAGE_ADDR_TYPE)&i, /* by reference */
		/*in */ 4,
		/*in */ 0,
		/*out*/ &RETURN_CODE);
	SEND_QUEUING_MESSAGE (
		/*in */ QUEUING_PORT_ID,
		/*in */ (MESSAGE_ADDR_TYPE)&i, /* by reference */
		/*in */ 4,
		/*in */ 0,
		/*out*/ &RETURN_CODE);
	SEND_QUEUING_MESSAGE (
		/*in */ QUEUING_PORT_ID,
		/*in */ (MESSAGE_ADDR_TYPE)&i, /* by reference */
		/*in */ 4,
		/*in */ 0,
		/*out*/ &RETURN_CODE);

	my_channel.MOVE_MESSAGES(&RETURN_CODE);
	assert(RETURN_CODE==NO_ERROR);
	int j=0;
	//char buf[10];

	//MESSAGE_QUEUE_TYPE MQ ("/temp1",10,10,PORT_SOURCE_DEST,&RETURN_CODE);

	LOGGER(USER,"Moved Messages");

	QUEUING_PORT_STATUS_TYPE QUEUING_PORT_STATUS;
	GET_QUEUING_PORT_STATUS (
		/*in */ QUEUING_PORT_ID,
		/*out*/ &QUEUING_PORT_STATUS,
		/*out*/ &RETURN_CODE );


	assert(RETURN_CODE==NO_ERROR);
	LOGGER(USER,"queing port 1  Q Num Messages = %d. Q direction =%d",QUEUING_PORT_STATUS.NB_MESSAGE,QUEUING_PORT_STATUS.PORT_DIRECTION);



	//QUEUING_PORT_STATUS_TYPE QUEUING_PORT_STATUS;
	GET_QUEUING_PORT_STATUS (
		/*in */ QUEUING_PORT_ID2,
		/*out*/ &QUEUING_PORT_STATUS,
		/*out*/ &RETURN_CODE );


	assert(RETURN_CODE==NO_ERROR);
	LOGGER(USER,"queing port 2 Q Num Messages = %d. Q direction =%d",QUEUING_PORT_STATUS.NB_MESSAGE,QUEUING_PORT_STATUS.PORT_DIRECTION);

	RECEIVE_QUEUING_MESSAGE (
		/*in */ QUEUING_PORT_ID2,
		/*in */ 0,
		/*out*/ (MESSAGE_ADDR_TYPE)&j,//buf[0],
		/*out*/ &LENGTH,
		/*out*/ &RETURN_CODE );

	//if (LENGTH>0)
	//	memcpy(&j,&buf[0],LENGTH);
	assert(RETURN_CODE==NO_ERROR);
	LOGGER(USER,"Received length of message %d. Received message %d",LENGTH,j);



	assert(RETURN_CODE==NO_ERROR);



}
