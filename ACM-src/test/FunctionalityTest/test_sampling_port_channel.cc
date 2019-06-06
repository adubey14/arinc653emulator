/* Used to test the internal methods used by the Module Manager for sampling port channels */
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
#include <iomanip>
#include <string.h>
using namespace ACM_USER;
using namespace ACM_INTERNAL;
int main(){


	RETURN_CODE_TYPE RETURN_CODE;


	SAMPLING_CHANNEL channel("testChannel");
	channel.ADD_SOURCE("/temp1",11,convertSecsToNano(5),&RETURN_CODE);
	channel.ADD_DESTINATION("/temp2",11,convertSecsToNano(5),&RETURN_CODE);
	channel.SET_INITIALIZED();

	//////////////

	LOGGER(INFO,"CHANNEL FINISHED");

	SAMPLING_PORT_HELPER SP ("/temp1",11,PORT_SOURCE_DEST,convertSecsToNano(5),&RETURN_CODE,O_CREAT);
	assert(RETURN_CODE==NO_ERROR);
	int i=0;
	int j=4;
	char buf[11]="4256767890";

	SP.WRITE_SAMPLING_MESSAGE((MESSAGE_ADDR_TYPE)&buf[0],11,&RETURN_CODE);

	VALIDITY_TYPE VALIDITY;
	MESSAGE_SIZE_TYPE LENGTH;
	char buf2[11];
	SP.READ_SAMPLING_MESSAGE((MESSAGE_ADDR_TYPE)&buf2[0],&LENGTH,&VALIDITY,&RETURN_CODE);
	std::cout<<"Read size"<<LENGTH<<" number " <<std::string(buf2)<<std::endl;
	std::cout<<"Validity is "<<VALIDITY<<std::endl;
	assert(RETURN_CODE==NO_ERROR);
	LOGGER(INFO,"GOING TO SLEEP FOR 6 %lld",currentTimeInNanoSeconds());
	HELPER_CLOCK_NANOSLEEP(convertSecsToNano(6));
	LOGGER(INFO,"OUT OF SLEEP FOR 6 %lld",currentTimeInNanoSeconds());

	SAMPLING_PORT_STATUS_TYPE SAMPLING_PORT_STATUS;
	SP.GET_SAMPLING_PORT_STATUS(&SAMPLING_PORT_STATUS,&RETURN_CODE);
	std::cout<<"Last Message Validity is "<<SAMPLING_PORT_STATUS.LAST_MSG_VALIDITY<<std::endl;
	SP.READ_SAMPLING_MESSAGE((MESSAGE_ADDR_TYPE)&buf2[0],&LENGTH,&VALIDITY,&RETURN_CODE);


	channel.MOVE_MESSAGES(&RETURN_CODE);
	assert(RETURN_CODE==NO_ERROR);
	LOGGER(INFO,"MOVED MESSAGES");
	SAMPLING_PORT_HELPER SP2 ("/temp2",11,PORT_SOURCE_DEST,convertSecsToNano(5),&RETURN_CODE,O_CREAT);
	assert(RETURN_CODE==NO_ERROR);
	LOGGER(INFO,"DOING COPY");
	assert(RETURN_CODE==NO_ERROR);
	SP2.READ_SAMPLING_MESSAGE((MESSAGE_ADDR_TYPE)&buf2[0],&LENGTH,&VALIDITY,&RETURN_CODE);
	std::cout<<"Read size"<<LENGTH<<" number " <<std::string(buf2)<<std::endl;
	SP2.GET_SAMPLING_PORT_STATUS(&SAMPLING_PORT_STATUS,&RETURN_CODE);
	std::cout<<"Last Message Validity is "<<SAMPLING_PORT_STATUS.LAST_MSG_VALIDITY<<std::endl;




	return 0;

}
