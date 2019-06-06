//# Copyright (c) Vanderbilt University, 2010
//# ALL RIGHTS RESERVED
//# Vanderbilt University disclaims all warranties with regard to this
//# software, including all implied warranties of merchantability
//# and fitness.  In no event shall Vanderbilt University be liable for
//# any special, indirect or consequential damages or any damages
//# whatsoever resulting from loss of use, data or profits, whether
//# in an action of contract, negligence or other tortious action,
//# arising out of or in connection with the use or performance of
//# this software.

#ifndef _ACM_SAMPLING_PORT_CHANNEL_H
#define _ACM_SAMPLING_PORT_CHANNEL_H

#include <ACM/INTERNAL/SAMPLING_PORT.h>
#include <ACM/INTERNAL/UDPSocket.h>

using namespace ACM_USER;

struct DESTINATION_URL{
	std::string node_name;
	unsigned short port;
	
	DESTINATION_URL(){};
	DESTINATION_URL(const std::string& name,const unsigned short& port_num):node_name(name),port(port_num){}
};

struct SOCKET_MARSHAL_DATA{
	unsigned long message_size;

};



namespace ACM_INTERNAL{
	enum COMMUNICATION_TYPE_ENUM
	{
		INTRA_MODULE,
		INTER_MODULE_SRC,
		INTER_MODULE_DST,
	};

	/*class SAMPLING_PORT_SOCKET_SOURCE
	{
		private:
			std::string NAME;
			UDPSocket* sender_mySocket;
			unsigned int MAX_SOCKET_MESSAGE;
			std::vector<DESTINATION_URL> destination_url_vector;
			char DataBuffer[sizeof(SYSTEM_TIME_TYPE)+sizeof(MESSAGE_SIZE_TYPE)+SYSTEM_LIMIT_MAX_SAMPLING_MESSAGE_SIZE];
	public:
		SAMPLING_PORT_SOCKET_SOURCE(std::string name,

	};*/

	class SAMPLING_CHANNEL
	{
	public:
	
	private: 
		std::string CHANNEL_NAME;
		SAMPLING_PORT_HELPER* SOURCE_PORT;
		bool INITIALIZED;
		MESSAGE_SIZE_TYPE MAX_SIZE_OF_MESSAGE;
		std::vector<SAMPLING_PORT_HELPER*> DESTINATION_PORT_VECTOR;		
		//socket descriptions
		UDPSocket* sender_mySocket;
		UDPSocket* recv_mySocket;
		unsigned int MAX_SOCKET_MESSAGE;
		std::vector<DESTINATION_URL> destination_url_vector;
		char DataBuffer[sizeof(SYSTEM_TIME_TYPE)+sizeof(MESSAGE_SIZE_TYPE)+SYSTEM_LIMIT_MAX_SAMPLING_MESSAGE_SIZE];
		COMMUNICATION_TYPE_ENUM  COMMUNICATION_TYPE;



	public:
		SAMPLING_CHANNEL(std::string NAME,COMMUNICATION_TYPE_ENUM COMMUNICATION_TYPE_VAL=INTRA_MODULE,unsigned int srcportNumber=0);
		SAMPLING_CHANNEL(COMMUNICATION_TYPE_ENUM COMMUNICATION_TYPE_VAL=INTRA_MODULE,unsigned int srcportNumber=0);	
		SAMPLING_CHANNEL(const SAMPLING_CHANNEL& my_channel);
		void SET_NAME(std::string data);
		std::string GET_NAME();
		std::string GET_SOURCE_NAME();
		std::string GET_DESTINATION_NAMES();
		void SET_MAX_SIZE_OF_MESSAGE(MESSAGE_SIZE_TYPE data,RETURN_CODE_TYPE* RETURN_CODE);
		void SET_COMMUNICATION_TYPE_ENUM(COMMUNICATION_TYPE_ENUM data,RETURN_CODE_TYPE* RETURN_CODE,unsigned int srcportNum=0);
		COMMUNICATION_TYPE_ENUM GET_COMMUNICATION_TYPE(){return this->COMMUNICATION_TYPE;}
		MESSAGE_SIZE_TYPE GET_MAX_SIZE_OF_MESSAGE(RETURN_CODE_TYPE* RETURN_CODE);
		~SAMPLING_CHANNEL();
		void SET_INITIALIZED();
		void SET_MAX_SOCKET_MESSAGE(unsigned int data,RETURN_CODE_TYPE* RETURN_CODE);
		bool GET_INITIALIZED();
		void ADD_SOURCE (std::string Name,MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,SYSTEM_TIME_TYPE REFRESH_PERIOD,RETURN_CODE_TYPE *RETURN_CODE);
		void ADD_DESTINATION (std::string Name,MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,SYSTEM_TIME_TYPE REFRESH_PERIOD,RETURN_CODE_TYPE *RETURN_CODE);
		void ADD_DESTINATION_URL(const std::string& nodename,const unsigned short& port_number,RETURN_CODE_TYPE *RETURN_CODE);
		void MOVE_MESSAGES (RETURN_CODE_TYPE *RETURN_CODE);
		void SET_SOCKET_BLOCKING()
		{
			if(sender_mySocket!=0)
				sender_mySocket->make_blocking();


			if(recv_mySocket!=0)
				recv_mySocket->make_blocking();

		}
		void SET_SOCKET_NON_BLOCKING()
		{
			if(sender_mySocket!=0)
				sender_mySocket->make_non_blocking();


			if(recv_mySocket!=0)
				recv_mySocket->make_non_blocking();

		}
	};


}
#endif //_ACM_SAMPLING_PORT_HELPER_H
