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

#ifndef ACM_EVENT_PORT_H
#define ACM_EVENT_PORT_H

#include <ACM/USER/APEX_TYPES.h>
#include <ACM/USER/ACM_HM_TYPES.h>
#include <ACM/USER/ACM_EXCEPTION.h>
#include <ACM/USER/ACM_UTILITIES.h>
#include <ACM/USER/ACM_LOGGER.h>
#include <ACM/USER/APEX_INTERFACE.h>

#include <ACM/INTERNAL/UDPSocket.h>
#include <stdio.h>


namespace ACM_USER{

	typedef APEX_UNSIGNED EVENT_PORT_ID_TYPE;
	typedef NAME_TYPE     EVENT_PORT_NAME_TYPE;
	enum EVENT_PORT_COMM_TYPE{READ,WRITE};

	//!A class that abstracts out the specifics of the communication port.
	class ACM_EVENT_PORT
	{
		enum COMM_IMPL_TYPE
		{
			UNDEFINED,
			BLACKBOARD, 
			BUFFER, 
			SAMPLINGPORT,
			QUEUEINGPORT 
		};
		struct DESTINATION_URL
		{
			NAME_TYPE node_name;
			unsigned int port;

			DESTINATION_URL();
			DESTINATION_URL(NAME_TYPE name,const unsigned int port_num);
			DESTINATION_URL(const DESTINATION_URL& url);

			void printHere();
			
		};

		EVENT_PORT_COMM_TYPE    EVENT_PORT_TYPE;
		EVENT_PORT_ID_TYPE      EVENT_PORT_ID;
		EVENT_PORT_NAME_TYPE    EVENT_PORT_NAME;
		SYSTEM_TIME_TYPE	    EVENT_PORT_VALIDITY_PERIOD;
		MESSAGE_ADDR_TYPE	    EVENT_PORT_PAYLOAD;
		MESSAGE_SIZE_TYPE		EVENT_PORT_TOTAL_SIZE;
		COMM_IMPL_TYPE			EVENT_PORT_IMPL_TYPE;
		bool					EVENT_PORT_DESTRUCTIVE_READ;

		std::vector  <DESTINATION_URL> DESTINATIONS_URLS;
		UDPSocket* sender_mySocket;
		UDPSocket* recv_mySocket;
		char* INTER_MODULE_DATA_BUFFER;

		//For Receiving Message
		string sourceAddress;             // Address of datagram source
		unsigned short sourcePort;        // Port of datagram source
		
	private:
		bool ONLY_INTER_MODULE;

	public:
		void SET_ONLY_INTER_MODULE(RETURN_CODE_TYPE* RETURN_CODE);

	public:

		void ADD_INTER_MODULE_DESTINATION_URL(NAME_TYPE nodename, unsigned int port_number,RETURN_CODE_TYPE *RETURN_CODE);
		void ADD_INTER_MODULE_SOURCE_SOCKET(unsigned int port_number,RETURN_CODE_TYPE *RETURN_CODE);

		ACM_EVENT_PORT ( EVENT_PORT_COMM_TYPE COMM_TYPE, 
			EVENT_PORT_NAME_TYPE NAME,
			MESSAGE_SIZE_TYPE SIZE, 
			SYSTEM_TIME_TYPE VALIDITY_PERIOD,
			MESSAGE_RANGE_TYPE MAX_NB_MESSAGE, 
			RETURN_CODE_TYPE* RETURN_CODE,
			bool isCOLOCATED,
			bool isDESTRUCTIVEREAD);

		~ACM_EVENT_PORT();

		void BLOCK_AND_TRANSFER_FROM_SOCKET_READ_PORT(RETURN_CODE_TYPE *RETURN_CODE,SYSTEM_TIME_TYPE PORT_TIME_OUT=0);

		void WRITE_TO_EVENT_PORT ( MESSAGE_ADDR_TYPE MESSAGE_ADDR, 
			MESSAGE_SIZE_TYPE LENGTH,
			SYSTEM_TIME_TYPE TIME_OUT,
			RETURN_CODE_TYPE *RETURN_CODE);

		void READ_FROM_EVENT_PORT ( MESSAGE_ADDR_TYPE MESSAGE_ADDR, 
			MESSAGE_SIZE_TYPE* LENGTH,
			bool* IS_VALID,
			SYSTEM_TIME_TYPE TIME_OUT,
			RETURN_CODE_TYPE *RETURN_CODE);

	private :

		void PARSE_AND_VALIDATE_DATA ( MESSAGE_ADDR_TYPE MESSAGE_ADDR,
			MESSAGE_SIZE_TYPE* LENGTH,
			VALIDITY_TYPE VALIDITY,
			bool* IS_VALID,
			RETURN_CODE_TYPE *RETURN_CODE);

		bool ASSEMBLE_DATA ( MESSAGE_ADDR_TYPE MESSAGE_ADDR,
			MESSAGE_SIZE_TYPE LENGTH,
			MESSAGE_SIZE_TYPE* NEW_LENGTH);


	};
}
#endif //ACM_EVENT_PORT_H

