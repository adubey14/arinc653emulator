/** \section{Name}
This  file tests the interModule queuing channel
*/
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
#include <iomanip>
#include <string.h>
using namespace ACM_USER;
using namespace ACM_INTERNAL;

int main() {
  try {

    RETURN_CODE_TYPE RETURN_CODE;
    QUEUE_CHANNEL my_channel("my_channel", INTER_MODULE_DST, 3314);
    my_channel.SET_MAX_SIZE_OF_MESSAGE(10, &RETURN_CODE);
    my_channel.ADD_DESTINATION_QUEUE("temp4", 10, 10, &RETURN_CODE);
    assert(RETURN_CODE == NO_ERROR);

    my_channel.SET_MAX_SOCKET_MESSAGE(4, &RETURN_CODE);

    my_channel.SET_INITIALIZED();

    assert(RETURN_CODE == NO_ERROR);
    /// channel created
    QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME2 = "temp4";
    QUEUING_PORT_ID_TYPE QUEUING_PORT_ID2;
    CREATE_QUEUING_PORT(
        /*in */ QUEUING_PORT_NAME2,
        /*in */ 10,
        /*in */ 10,
        /*in */ DESTINATION,
        /*in */ FIFO,
        /*out*/ &QUEUING_PORT_ID2,
        /*out*/ &RETURN_CODE);
    assert(RETURN_CODE == NO_ERROR);
    sleep(15);

    my_channel.MOVE_MESSAGES(&RETURN_CODE);
    // assert(RETURN_CODE==NO_ERROR);
    int j = 0;
    // char buf[10];

    // MESSAGE_QUEUE_TYPE MQ ("/temp1",10,10,PORT_SOURCE_DEST,&RETURN_CODE);

    LOGGER(USER, "Moved Messages");

    QUEUING_PORT_STATUS_TYPE QUEUING_PORT_STATUS;
    MESSAGE_SIZE_TYPE LENGTH;
    // QUEUING_PORT_STATUS_TYPE QUEUING_PORT_STATUS;
    GET_QUEUING_PORT_STATUS(
        /*in */ QUEUING_PORT_ID2,
        /*out*/ &QUEUING_PORT_STATUS,
        /*out*/ &RETURN_CODE);

    assert(RETURN_CODE == NO_ERROR);
    LOGGER(USER, "queing port 2 Q Num Messages = %d. Q direction =%d",
           QUEUING_PORT_STATUS.NB_MESSAGE, QUEUING_PORT_STATUS.PORT_DIRECTION);

    RECEIVE_QUEUING_MESSAGE(
        /*in */ QUEUING_PORT_ID2,
        /*in */ 0,
        /*out*/ (MESSAGE_ADDR_TYPE)&j, // buf[0],
        /*out*/ &LENGTH,
        /*out*/ &RETURN_CODE);

    // if (LENGTH>0)
    //	memcpy(&j,&buf[0],LENGTH);
    assert(RETURN_CODE == NO_ERROR);
    LOGGER(USER, "Received length of message %d. Received message %d", LENGTH,
           j);

    assert(RETURN_CODE == NO_ERROR);

    // int i=2;
    // RETURN_CODE_TYPE RETURN_CODE;
    // MESSAGE_QUEUE_TYPE MQ("/temp4",10,10,PORT_SOURCE_DEST,&RETURN_CODE);
    // MQ.SEND_MESSAGE(
    // (MESSAGE_ADDR_TYPE)&i,4,convertSecsToNano(4),&RETURN_CODE);
    // LOGGER(USER,"Written i");
    ////assert (mq_send (MQ.Q_MQDES,(char*)&i,sizeof(i),0)!=-1);
    ////char buf[10];
    // int k;
    // SYSTEM_TIME_TYPE timeout= convertSecsToNano(5);
    // QUEUING_PORT_STATUS_TYPE QUEUING_PORT_STATUS;
    // MQ.GET_QUEUING_PORT_STATUS(&QUEUING_PORT_STATUS, &RETURN_CODE);
    // LOGGER(USER,"queing port 2 Q Num Messages = %d. Q direction
    // =%d",QUEUING_PORT_STATUS.NB_MESSAGE,QUEUING_PORT_STATUS.PORT_DIRECTION);
    // assert(RETURN_CODE==NO_ERROR);
    //
    //
    // LOGGER(USER,"DOING MQRECIEVE");
    // int sizereceived=0;
    // int j=0;
    // MQ.RECEIVE_MESSAGE((MESSAGE_ADDR_TYPE)&j,(MESSAGE_SIZE_TYPE*)&sizereceived,timeout,&RETURN_CODE);
    // LOGGER(USER,"DONE MQRECIEVE");
    // assert (sizereceived!=-1||RETURN_CODE==TIMED_OUT);
    // std::cout<<sizereceived<<" "<<j<<std::endl;
    // if (sizereceived>0)
    //	memcpy(&k,&buf[0],sizereceived);
    //
    // std::cout<<sizereceived<<" "<<k<<std::endl;
    //
    // MQ.RECEIVE_MESSAGE((MESSAGE_ADDR_TYPE)&buf[0],(MESSAGE_SIZE_TYPE*)&sizereceived,timeout,&RETURN_CODE);
    // LOGGER(USER,"DONE MQRECIEVE");
    // assert (sizereceived!=-1||RETURN_CODE==TIMED_OUT);
    // if (sizereceived>0)
    //	memcpy(&k,&buf[0],sizereceived);
    //
    // std::cout<<sizereceived<<" "<<k<<std::endl;
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
