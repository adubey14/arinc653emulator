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


#ifndef _MODULE_MANAGER_H
#define _MODULE_MANAGER_H
/* PartitionManager */


#define CONFIGURATION_PORT 4111
#include <fstream>
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
#include <ACM/USER/ACM_EXCEPTION.h>
#include <ACM/INTERNAL/QUEUE_PORT.h>
#include <ACM/INTERNAL/QUEUE_CHANNEL.h>
#include <ACM/INTERNAL/SAMPLING_PORT.h>
#include <ACM/INTERNAL/SAMPLING_CHANNEL.h>


#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <map>

#include <ACM/INTERNAL/UDPSocket.h>

using namespace ACM_USER;

typedef char NETWORK_PARTITION_PETRI_NET_TOKEN_TYPE [4];
const NETWORK_PARTITION_PETRI_NET_TOKEN_TYPE NETWORK_PARTITION_PETRI_NET_TOKEN ="GO";

//All Configuration Tags
namespace ACM_INTERNAL{

	extern const char* PRIMARY_KEY_NETWORK_PARTITION_NAME;
	
	extern const char* ADDITVE_KEY_CHANNEL;
	extern const char* PRIMARY_KEY_SYNCHRONIZED_EXECUTION;
	extern const char* PRIMARY_KEY_BROADCAST_ADDRESS;
	extern const char* PRIMARY_KEY_MULTICAST_ADDRESS;
	extern const char* PRIMARY_KEY_DISCOVERY_PROTOCOL;
	extern const char* PRIMARY_KEY_DISCOVERY_PROTOCOL_IOR;
	extern const char* PRIMARY_KEY_DISCOVERY_PROTOCOL_NS;
	extern const char* PRIMARY_KEY_DISCOVERY_PROTOCOL_NS_HOST;
	extern const char* PRIMARY_KEY_DISCOVERY_PROTOCOL_NS_PORT;
	extern const char* PRIMARY_KEY_HYPERPERIOD_KEY;
	extern const char* PRIMARY_KEY_MAXITERATIONS;
	extern const char* PRIMARY_KEY_PARTITION_NAME;
	extern const char* PRIMARY_KEY_CPU;
	extern const char* PRIMARY_KEY_PARTITION_INIT_TIMEOUT;
	extern const char* PRIMARY_KEY_CHANNEL_NAME;
	extern const char* PRIMARY_KEY_NUMBER_OF_INITIAL_STAGES;
	extern const char* PRIMARY_KEY_PROXY_PORT;
	extern const char* PRIMARY_KEY_CONFIGURATION_PORT;
	extern const char* PRIMARY_KEY_PARENT_PORT;
	extern const char* PRIMARY_KEY_MODULE_NAME;
	extern const char* PRIMARY_KEY_PARENT_MODULE_NAME;
	extern const char* PRIMARY_KEY_CHILD_MODULE_NAME;
	extern const char* ADDITVE_KEY_MM_HOSTNAME;
	extern const char* ADDITIVE_KEY__PORT_NUMBER;
	extern const char* ADDITIVE_KEY__MODULE_NAME;
	extern const char* ADDITIVE_KEY__TYPE;
	extern const char* PROXY_TYPE_SAMPLING;
	extern const char* PROXY_TYPE_QUEUEING;
	extern const char* ADDITIVE_KEY__DIRECTION;
	extern const char* ADDITIVE_KEY__DESTINATION;
	extern const char* ADDITIVE_KEY__SOURCE;
	extern const char* ADDITIVE_KEY__SCHEDULE;
	extern const char* ADDITIVE_KEY__EXECUTABLE;
	extern const char* ADDITIVE_KEY__SAMPLINGPORT;
	extern const char* ADDITIVE_KEY__MAXMESSAGESIZE;
	extern const char* ADDITIVE_KEY__REFRESHPERIOD;
	extern const char* ADDITIVE_KEY__QUEUINGPORT;
	extern const char* ADDITIVE_KEY__MAXNUMBEROFMESSAGES;

	void print_key_values();

};

namespace ACM_INTERNAL
{

	struct SocketSynchronizationMessageType
	{
		enum SyncType
		{
			BARRIER_READY,
			BARRIER_ACK,
			START,
			STOP,
			NETWORK_LATENCY_READY,
			NETWORK_LATENCY_CHILD_ACK,
			NEWORK_LATENCY_PARENT_ACK
		};
		SyncType Sync;
		ACM_USER::NAME_TYPE MESSAGE_SRC_MODULE;
		ACM_USER::NAME_TYPE MESSAGE_DST_MODULE;		
		SYSTEM_TIME_TYPE NANOSECONDS_TO_WAIT;

		ACM_USER::APEX_LONG_INTEGER seq_id;

		SocketSynchronizationMessageType():NANOSECONDS_TO_WAIT(0),seq_id(0){}
		SocketSynchronizationMessageType(SyncType sync1, std::string src_name,std::string dst_name,SYSTEM_TIME_TYPE WAIT=0):
		Sync(sync1),NANOSECONDS_TO_WAIT(WAIT),seq_id(0)
		{
			strncpy(MESSAGE_SRC_MODULE,src_name.c_str(),sizeof(ACM_USER::NAME_TYPE));
			strncpy(MESSAGE_DST_MODULE,dst_name.c_str(),sizeof(ACM_USER::NAME_TYPE));
		}

#define SyncTypeSize sizeof(SocketSynchronizationMessageType::SyncType)+sizeof(ACM_USER::NAME_TYPE)+sizeof(ACM_USER::NAME_TYPE)+sizeof(SYSTEM_TIME_TYPE)+sizeof(ACM_USER::APEX_LONG_INTEGER)
		typedef char SERIAL_BUFFER_TYPE[SyncTypeSize] ;

		void printout()
		{
			std::string syncName;

			switch(Sync)
			{
			case BARRIER_READY:
				syncName="BARRIER_READY";
				break;
			case BARRIER_ACK:
				syncName="BARRIER_ACK";
				break;
			case START:
				syncName="START";
				break;
			case STOP:
				syncName="STOP";
				break;
			case NETWORK_LATENCY_READY:
				syncName="NETWORK_LATENCY_READY";
				break;
			case NETWORK_LATENCY_CHILD_ACK:
				syncName="NETWORK_LATENCY_CHILD_ACK";
				break;
			case NEWORK_LATENCY_PARENT_ACK:
				syncName="NEWORK_LATENCY_PARENT_ACK";
				break;
			};

			LOGGER(INFO,"SyncType:%s, src:%s, dst:%s, wait: %lld ns: seq_id: %lld",syncName.c_str(),MESSAGE_SRC_MODULE,MESSAGE_DST_MODULE,NANOSECONDS_TO_WAIT,seq_id);			
		}

		void increment()
		{
			this->seq_id++;
		}



		static void TO_SERIAL_BUFFER(SERIAL_BUFFER_TYPE& SERIAL_BUFFER,SocketSynchronizationMessageType& message)
		{
			unsigned int written=0;
			memcpy(&(SERIAL_BUFFER[0]),&(message.Sync),sizeof(SyncType));written+=sizeof(SyncType);
			memcpy(&(SERIAL_BUFFER[0])+written,&(message.MESSAGE_SRC_MODULE[0]),sizeof(ACM_USER::NAME_TYPE));written+=sizeof(ACM_USER::NAME_TYPE);
			memcpy(&(SERIAL_BUFFER[0])+written,&(message.MESSAGE_DST_MODULE[0]),sizeof(ACM_USER::NAME_TYPE));written+=sizeof(ACM_USER::NAME_TYPE);
			memcpy(&(SERIAL_BUFFER[0])+written,&(message.NANOSECONDS_TO_WAIT),sizeof(SYSTEM_TIME_TYPE));written+=sizeof(SYSTEM_TIME_TYPE);
			memcpy(&(SERIAL_BUFFER[0])+written,&(message.seq_id),sizeof(APEX_LONG_INTEGER));written+=sizeof(APEX_LONG_INTEGER);
			return;
		}

		static void FROM_SERIAL_BUFFER(SocketSynchronizationMessageType& message,SERIAL_BUFFER_TYPE& SERIAL_BUFFER )
		{
			unsigned int written=0;
			memcpy(&(message.Sync),&(SERIAL_BUFFER[0]),sizeof(SyncType));written+=sizeof(SyncType);
			memcpy(&(message.MESSAGE_SRC_MODULE[0]),&(SERIAL_BUFFER[0])+written,sizeof(ACM_USER::NAME_TYPE));written+=sizeof(ACM_USER::NAME_TYPE);
			memcpy(&(message.MESSAGE_DST_MODULE[0]),&(SERIAL_BUFFER[0])+written,sizeof(ACM_USER::NAME_TYPE));written+=sizeof(ACM_USER::NAME_TYPE);
			memcpy(&(message.NANOSECONDS_TO_WAIT),&(SERIAL_BUFFER[0])+written,sizeof(SYSTEM_TIME_TYPE));written+=sizeof(SYSTEM_TIME_TYPE);
			memcpy(&(message.seq_id),&(SERIAL_BUFFER[0])+written,sizeof(APEX_LONG_INTEGER));written+=sizeof(APEX_LONG_INTEGER);
			return;
		}
	};

};


#define ENSURE_CONDITION(x) \
	if (!x)\
{ \
	printf("Error in program %s At line %d. Exiting Now. \n", __FILE__, __LINE__ ); \
	throw(-1); \
}




namespace ACM_INTERNAL{




	struct CHANNEL_DESCRIPTOR
	{
		std::string source;
		std::vector<std::string> destination;
	};
	typedef std::map<std::string,CHANNEL_DESCRIPTOR> CHANNEL_MAP_TYPE;

	struct SCHEDULING_TUPLE
	{
		SYSTEM_TIME_TYPE OFFSET;
		SYSTEM_TIME_TYPE DURATION;
		unsigned int PARTITION_INDEX;
		void printDescriptor(){
			LOGGER(INFO,"PARTITION_INDEX:%d OFFSET:%lld,DURATION:%lld",PARTITION_INDEX,OFFSET,DURATION);
		}
	};

	struct PROXY_PORT_DESCRIPTORS
	{ 
		enum PROXY_PORT_DESCRIPTORS_TYPE
		{
			SAMPLING,
			QUEUEING
		};
		PROXY_PORT_DESCRIPTORS_TYPE PROXY_TYPE;
		std::string Name;
		std::string hostName;
		unsigned int portNumber;
		MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE;
		MESSAGE_RANGE_TYPE MAX_NB_MESSAGE;
		PORT_DIRECTION_TYPE PORT_DIRECTION;
		void printout(){
			LOGGER(INFO," proxy Port Name:%s,Module_name:%s,port_number:%u,MAX_MESSAGE_SIZE:%d,MAX_NB_MESSAGE:%d,PORT_DIRECTION:%s"
				,Name.c_str(),hostName.c_str(),portNumber,MAX_MESSAGE_SIZE,MAX_NB_MESSAGE,(PORT_DIRECTION==SOURCE)?"SOURCE":"DESTINATION");
		}

	};

	struct QUEUING_PORT_DESCRIPTOR
	{
		std::string Name;
		MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE;
		MESSAGE_RANGE_TYPE MAX_NB_MESSAGE;
		PORT_DIRECTION_TYPE PORT_DIRECTION;
		void printout(){
			LOGGER(INFO," Queuing Port Name:%s,MAX_MESSAGE_SIZE:%d,MAX_NB_MESSAGE:%d,PORT_DIRECTION:%s",Name.c_str(),MAX_MESSAGE_SIZE,MAX_NB_MESSAGE,(PORT_DIRECTION==SOURCE)?"SOURCE":"DESTINATION");
		}
	};

	struct SAMPLING_PORT_DESCRIPTOR
	{
		std::string Name;
		MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE;
		SYSTEM_TIME_TYPE REFRESH_PERIOD;
		PORT_DIRECTION_TYPE PORT_DIRECTION;
		void printout(){
			LOGGER(INFO,"Sampling Port Name:%s,MAX_MESSAGE_SIZE:%d,REFRESH_PERIOD:%lld,PORT_DIRECTION:%s",Name.c_str(),MAX_MESSAGE_SIZE,REFRESH_PERIOD,(PORT_DIRECTION==SOURCE)?"SOURCE":"DESTINATION");
		}
	};

	struct PARTITION 
	{
		pid_t PID;
		SYSTEM_TIME_TYPE DURATION; //In Nanoseconds
		std::string Name;
		std::string Executable;
		bool isNetworkPartition;

		//Check and Ensure that All Ports are unique.
		std::vector<QUEUING_PORT_DESCRIPTOR> QUEUING_PORTS;
		std::vector<SAMPLING_PORT_DESCRIPTOR> SAMPLING_PORTS;

		//	std::vector<SAMPLING_CHANNEL*> SAMPLING_CHANNEL_SOURCE_VECTOR;;

		std::vector<std::string> SAMPLING_CHANNEL_SOURCE_VECTOR;
		std::vector<std::string> QUEUE_CHANNEL_SOURCE_VECTOR;
		std::vector<std::string> QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR;
		std::vector<std::string> SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR;

		std::string  CONFIGURATION_QUEUE;
		inline bool CheckIfChannelExists(std::string& name)
		{


			for (unsigned int i=0;i<SAMPLING_CHANNEL_SOURCE_VECTOR.size();i++)
			{
				if(SAMPLING_CHANNEL_SOURCE_VECTOR[i].compare(name)==0 ) return true;
			}

			for (unsigned int i=0;i<QUEUE_CHANNEL_SOURCE_VECTOR.size();i++)
			{
				if(QUEUE_CHANNEL_SOURCE_VECTOR[i].compare(name)==0 ) return true;
			}

			for (unsigned int i=0;i<QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR.size();i++)
			{
				if(QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR[i].compare(name)==0 ) return true;
			}

			for (unsigned int i=0;i<SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR.size();i++)
			{
				if(SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR[i].compare(name)==0 ) return true;
			}

			return false;

		}

		//	std::vector<QUEUE_CHANNEL*> QUEUE_CHANNEL_SOURCE_VECTOR;

		//	std::vector<QUEUE_CHANNEL*> QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR;


		//	std::vector<SAMPLING_CHANNEL*> SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR;


		void printDescriptor(){
			if(isNetworkPartition)
			{
				LOGGER(INFO,"Network Partition: %s ",Name.c_str());					
			}
			else
			{
				LOGGER(INFO,"Partition: %s, Executable: %s",Name.c_str(),Executable.c_str());					
			}
			for (unsigned int i=0;i<QUEUING_PORTS.size();i++)
				LOGGER(INFO,"Partition: %s, Queuing Port Name:%s,MAX_MESSAGE_SIZE:%d,MAX_NB_MESSAGE:%d,PORT_DIRECTION:%s",Name.c_str(),QUEUING_PORTS[i].Name.c_str(),QUEUING_PORTS[i].MAX_MESSAGE_SIZE,QUEUING_PORTS[i].MAX_NB_MESSAGE,(QUEUING_PORTS[i].PORT_DIRECTION==SOURCE)?"SOURCE":"DESTINATION");

			for (unsigned int i=0;i<SAMPLING_PORTS.size();i++)
				LOGGER(INFO,"Partition: %s, Sampling Port Name:%s,MAX_MESSAGE_SIZE:%d,REFRESH_PERIOD:%lld,PORT_DIRECTION:%s",Name.c_str(), SAMPLING_PORTS[i].Name.c_str(),SAMPLING_PORTS[i].MAX_MESSAGE_SIZE,SAMPLING_PORTS[i].REFRESH_PERIOD,(SAMPLING_PORTS[i].PORT_DIRECTION==SOURCE)?"SOURCE":"DESTINATION");

			for (unsigned int i=0;i<QUEUE_CHANNEL_SOURCE_VECTOR.size();i++)
				LOGGER(INFO,"Partition: %s, Queuing Channel Name: %s",Name.c_str(),QUEUE_CHANNEL_SOURCE_VECTOR[i].c_str());

			for (unsigned int i=0;i<SAMPLING_CHANNEL_SOURCE_VECTOR.size();i++)
				LOGGER(INFO,"Partition: %s, SAMPLING_CHANNEL_SOURCE_VECTOR Channel Name: %s",Name.c_str(),SAMPLING_CHANNEL_SOURCE_VECTOR[i].c_str());

			for (unsigned int i=0;i<QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR.size();i++)
				LOGGER(INFO,"Partition: %s, QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR Channel Name: %s",Name.c_str(),QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR[i].c_str());



			//for (unsigned int i=0;i<SAMPLING_CHANNEL_SOURCE_VECTOR.size();i++)
			//LOGGER(INFO,"Partition: %s, Sampling Channel Name: %s, Channel Source %s, Destination Names: %s",Name.c_str(),SAMPLING_CHANNEL_SOURCE_VECTOR[i]->GET_NAME().c_str(),SAMPLING_CHANNEL_SOURCE_VECTOR[i]->GET_SOURCE_NAME().c_str(),SAMPLING_CHANNEL_SOURCE_VECTOR[i]->GET_DESTINATION_NAMES().c_str());

			//for (unsigned int i=0;i<QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR.size();i++)
			//{
			//LOGGER(INFO,"Partition: %s, Queuing Channel Name: %s, Channel Source: %s, Destination Names: %s",Name.c_str(),QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR[i]->GET_NAME().c_str(),QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR[i]->GET_SOURCE_NAME().c_str(),QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR[i]->GET_DESTINATION_NAMES().c_str());

			//}
			//for (unsigned int i=0;i<SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR.size();i++)
			//{
			//LOGGER(INFO,"Partition: %s, Sampling Channel Name: %s, Channel Source %s, Destination Names: %s",Name.c_str(),SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR[i]->GET_NAME().c_str(),SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR[i]->GET_SOURCE_NAME().c_str(),SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR[i]->GET_DESTINATION_NAMES().c_str());

			//}

		}

		PARTITION():isNetworkPartition(false) {
			//QUEUE_CHANNEL_SOURCE_VECTOR.reserve(32);
			//SAMPLING_CHANNEL_SOURCE_VECTOR.reserve(32);
		}
		~PARTITION(){
			LOGGER(INFO, "Destryoing Partition %s",Name.c_str());
			QUEUING_PORTS.clear();
			SAMPLING_PORTS.clear();
			QUEUE_CHANNEL_SOURCE_VECTOR.clear();
			SAMPLING_CHANNEL_SOURCE_VECTOR.clear();
			QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR.clear();
			SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR.clear();

		}		
	};

	class MODULE_MANAGER
	{
	public:
		//how many scheduling cycle will the controller do
#define SCHEDULING_CYCLES 100
		//maximum number of allowed partitions
#define MAX_PARTITION_NUM 10
		//possible return value of CreatePartition
		//says that the maximum number of partitions is already reached, no more partitions can be created
#define EMAXPARTNUM -1


		enum DISCOVERY_PROTOCOL_ENUM{IOR,NAMINGSERVICE};

	public:
		static bool SCHEDULE_COMP(const SCHEDULING_TUPLE& first, const SCHEDULING_TUPLE& second)  
		{
			return first.OFFSET<second.OFFSET;
		}

	public:
		std::vector<PROXY_PORT_DESCRIPTORS> PROXY_PORTS;

		//	std::vector<QUEUE_CHANNEL*> QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR;
		//	std::vector<SAMPLING_CHANNEL*> SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR;




		std::map<std::string,QUEUE_CHANNEL*> MAP_OF_QUEUEING_CHANNELS;
		std::map<std::string,SAMPLING_CHANNEL*> MAP_OF_SAMPLING_CHANNELS;

		std::map<std::string,MESSAGE_QUEUE_TYPE*> MAP_OF_CONFIGURATION_QUEUE;



	private:
		//local variable for the the singleton instance 
		static MODULE_MANAGER* _myInstance;		
		//private constructor
		MODULE_MANAGER(); 	
	public:
		//starts the scheduling of the partitions
		int FIND_PARTITION_FROM_QUEUING_PORT(std::string& PORT_NAME,QUEUING_PORT_DESCRIPTOR& descriptor);
		int FIND_PARTITION_FROM_SAMPLING_PORT(std::string& PORT_NAME,SAMPLING_PORT_DESCRIPTOR& descriptor);

		void SEND_SIGCONT_TO_PARTITION(pid_t partitionpid);
		void SEND_SIGSTOP_TO_PARTITION(pid_t partitionpid);


		//custom struct for storing scheduling information
		struct scheduling_info
		{
			int partitionNum;
			SYSTEM_TIME_TYPE time;
		};

		void SET_CPU_NUMBER();



		//by sending SIGUSR1 the partition will notify the contoller that it initialized itself
		static void sigUsrHandler(int sig);

		//mutex for synchronize partition start with controller run


		void SET_FIFO_POLICY();
		void SET_CPU_AFFINITY();


		bool SYNC_WITH_OTHER_MODULE_MANAGERS();
		int RUN_WITHOUT_SYSTEM_MANAGER();
		int RUN_SYSTEM_MODULE_MANAGER();
		int RUN_CHILD_MODULE_MANAGER();
		void SEND_HYPER_PERIOD_ECHO(bool Stop=false);
		bool GET_READY_MM_FOR_HYPERPERIOD();

		DISCOVERY_PROTOCOL_ENUM DISCOVERY_PROTOCOL;
		std::string NS_HOST_IP;
		int NS_PORT_NUMBER;
		ACM_INTERNAL::MUTEX my_mutex;
		int cpunumber;
		//id of the fake process
		pid_t DUMMY_PROCESS_ID;
		void SET_MAX_NUMBER_OF_FRAMES();
		int MAX_NUMBER_OF_FRAMES;
		bool isMAX_NUMBER_OF_FRAMES_SET;
		void SET_MIN_WAIT_TIME();
		int MIN_WAIT_TIME;
		bool isMIN_WAIT_TIME_SET;
		CHANNEL_MAP_TYPE CHANNEL_MAP;
		unsigned int NumberOfInitStages;
		std::string MODULE_NAME;
		std::string PARENT_NAME;			
		std::string PARENT_HOST;
		unsigned int PARENT_PORT;
		unsigned int CONFIGURATION_PORT_NUM;
		bool isSystemModuleManager;
		bool isBroadcast;
		bool isMulticast;

		bool isrunningUnderSystemModuleManager;
		std::string broadcastaddress;
		UDPSocket* Recv_Socket;
		UDPSocket* Send_Socket;
		SYSTEM_TIME_TYPE AVE_NETWORK_LATENCY_FROM_PARENT;


		std::vector <PARTITION> PARTITION_VECTOR;
		//local storage for scheduling info
		struct scheduling_info schedules[ SCHEDULING_CYCLES * MAX_PARTITION_NUM ];
		std::vector<SCHEDULING_TUPLE> SCHEDULING_VECTOR;

		//number of partitions we have after initialization
		int partitionCounter;
		std::multimap<std::string,std::string> PropertyMap;
		typedef std::multimap<std::string,std::string>::iterator PropertyMapIteratorType;

		void INIT_PARTITION_PROPERTY_MAP(std::string filename);

		void SET_MAJOR_FRAME_SIZE();

		SYSTEM_TIME_TYPE MAJOR_FRAME_SIZE;

		void CREATE_PROXY_PORT_DESCRIPTORS();
		void CREATE_PARTITION_VECTOR();
		void CREATE_NETWORK_PARTITION_VECTOR();
		void CREATE_CHANNEL_DSCRIPTOR();
		void CREATE_QUEUEING_CHANNELS();

		void CREATE_SAMPLING_CHANNELS();
		void SET_DISCOVERY_PROTOCOL();
		void SET_NAMING_SERVICE();
		void READ_NUMBER_OF_INIT_STAGES();


	public:
		//give back the singleton instance
		static MODULE_MANAGER* Instance();
		static std::string fileName;

		bool CanAddChannel(std::string name);

		//create a partitions with the given parameters
		int LOAD_AND_INITIALIZE_PARTITION_EXECUTABLE(unsigned int index);
		int Synchronize();

		//destructor
		~MODULE_MANAGER();
		//we need to have a dummy process with priority level between the active and sleeping worker processec
		void CREATE_DUMMY_PROCESS(void);



		static void DOSHUTDOWN(bool proper=false);

		static void DOSHUTDOWN_BEFORE_INIT();

		void SORTALLLOGS();

		//creates a partiton definition with the given parameters
		//return value: 0 if successfull
		//EMAXPARTNUM if the maximum number of partitions is already reached
		//int CreatePartition( SYSTEM_TIME_TYPE duration, const char* name, const char* configfile );

		//creates partitions from configuration file
		//filename name of the configuration file, i.e. PartitionManager.cfg
		//int CreatePartitionsFromConfigurationFile( const char* filename );

		//runs the partition controller for the predefined amount of cycles
		int RUN_MODULE_MANAGER();

		inline static void HELPER_CLOCK_NANOSLEEP(SYSTEM_TIME_TYPE TIME_OUT){
			SYSTEM_TIMESPEC actualtime=currentTime();
			SYSTEM_TIMESPEC absolute_time=addTimeNano(actualtime,TIME_OUT);				
			int clockresult =1;
			// you can spuriously get out of nanosleep
			struct timespec rem = {0,0};
			while ( clockresult != 0) {
				clockresult = clock_nanosleep(CLOCK_REALTIME ,TIMER_ABSTIME, &absolute_time,&rem);
			}

		}
		static void SIGINT_HANDLER(int sig);
		static void SIGTERM_HANDLER(int sig);

		static void SIGUSR_HANDLER(int sig);


		inline void RUN_PARTITION(unsigned int& PARTITION_INDEX,SYSTEM_TIMESPEC& ABSOLUTE_DURATION_TIME )
		{

			RETURN_CODE_TYPE RETURN_CODE;
			if(!PARTITION_VECTOR[PARTITION_INDEX].isNetworkPartition)
			{

				for (unsigned int j =0; j< PARTITION_VECTOR[PARTITION_INDEX].QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR.size();j++)
				{
					this->MAP_OF_QUEUEING_CHANNELS[PARTITION_VECTOR[PARTITION_INDEX].QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR[j]]->MOVE_MESSAGES(&RETURN_CODE);
					if(RETURN_CODE!=NO_ERROR)
						LOGGER(HMEVENT,"ERROR while writing to %s",PARTITION_VECTOR[PARTITION_INDEX].QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR[j].c_str());
					//ENSURE_CONDITION(RETURN_CODE==NO_ERROR);
				}
				for (unsigned int j =0; j< PARTITION_VECTOR[PARTITION_INDEX].SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR.size();j++)
				{
					this->MAP_OF_SAMPLING_CHANNELS[PARTITION_VECTOR[PARTITION_INDEX].SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR[j]]->MOVE_MESSAGES(&RETURN_CODE);
					if(RETURN_CODE!=NO_ERROR)
						LOGGER(HMEVENT,"ERROR while writing to %s",PARTITION_VECTOR[PARTITION_INDEX].SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR[j].c_str());
					//ENSURE_CONDITION(RETURN_CODE==NO_ERROR);
				}
			}


			LOGGER(TRACE,"START: %s",PARTITION_VECTOR[PARTITION_INDEX].Name.c_str());
			if(PARTITION_VECTOR[PARTITION_INDEX].isNetworkPartition)
			{
				if(this->MAP_OF_CONFIGURATION_QUEUE.find(PARTITION_VECTOR[PARTITION_INDEX].CONFIGURATION_QUEUE)==this->MAP_OF_CONFIGURATION_QUEUE.end())
					LOGGER(ERROR,"CONFIGURATION QUEUE FOR NETWORK PAR is null %s",PARTITION_VECTOR[PARTITION_INDEX].Name.c_str());

				else if (this->MAP_OF_CONFIGURATION_QUEUE[PARTITION_VECTOR[PARTITION_INDEX].CONFIGURATION_QUEUE]==0)
				{
					LOGGER(ERROR,"CONFIGURATION QUEUE FOR NETWORK PAR is null %s",PARTITION_VECTOR[PARTITION_INDEX].Name.c_str());
				}
				else
				{
					RETURN_CODE_TYPE return_code;
					this->MAP_OF_CONFIGURATION_QUEUE[PARTITION_VECTOR[PARTITION_INDEX].CONFIGURATION_QUEUE]->SEND_MESSAGE((MESSAGE_ADDR_TYPE)&NETWORK_PARTITION_PETRI_NET_TOKEN[0],sizeof(NETWORK_PARTITION_PETRI_NET_TOKEN_TYPE),0,&return_code);
					if(return_code!=NO_ERROR)
					{
						LOGGER(ERROR,"Failed sending message to CONFIGURATION QUEUE FOR NETWORK part %s",PARTITION_VECTOR[PARTITION_INDEX].Name.c_str());
					}
				}
				//send a message
			}

			SEND_SIGCONT_TO_PARTITION(PARTITION_VECTOR[PARTITION_INDEX].PID);

			struct timespec rem1 = {0,0};
			int clockresult=1;
			while ( clockresult != 0) {
				clockresult = clock_nanosleep( CLOCK_REALTIME ,TIMER_ABSTIME, &ABSOLUTE_DURATION_TIME,&rem1);
			}
			LOGGER(TRACE,"STOP: %s",PARTITION_VECTOR[PARTITION_INDEX].Name.c_str());
			SEND_SIGSTOP_TO_PARTITION(PARTITION_VECTOR[PARTITION_INDEX].PID);

			//Now move channels
			//Now move channels
			//RETURN_CODE_TYPE RETURN_CODE;
			if(!PARTITION_VECTOR[PARTITION_INDEX].isNetworkPartition)
			{
				for (unsigned int j =0; j< PARTITION_VECTOR[PARTITION_INDEX].QUEUE_CHANNEL_SOURCE_VECTOR.size();j++)
				{
					this->MAP_OF_QUEUEING_CHANNELS[PARTITION_VECTOR[PARTITION_INDEX].QUEUE_CHANNEL_SOURCE_VECTOR[j]]->MOVE_MESSAGES(&RETURN_CODE);
					if(RETURN_CODE!=NO_ERROR)
						LOGGER(HMEVENT,"ERROR while writing to %s",PARTITION_VECTOR[PARTITION_INDEX].QUEUE_CHANNEL_SOURCE_VECTOR[j].c_str());
					//ENSURE_CONDITION(RETURN_CODE==NO_ERROR);
				}
				for (unsigned int j =0; j< PARTITION_VECTOR[PARTITION_INDEX].SAMPLING_CHANNEL_SOURCE_VECTOR.size();j++)
				{
					this->MAP_OF_SAMPLING_CHANNELS[PARTITION_VECTOR[PARTITION_INDEX].SAMPLING_CHANNEL_SOURCE_VECTOR[j]]->MOVE_MESSAGES(&RETURN_CODE);
					if(RETURN_CODE!=NO_ERROR)
						LOGGER(HMEVENT,"ERROR while writing to %s",PARTITION_VECTOR[PARTITION_INDEX].SAMPLING_CHANNEL_SOURCE_VECTOR[j].c_str());
					//ENSURE_CONDITION(RETURN_CODE==NO_ERROR);
				}
			}
		}

	public:

		std::ofstream killfile;
		char killfilename[30];
		struct child_url
		{
			bool receivedanswer;
			std::string childname;
			std::string hostname;
			unsigned int portnum;

			child_url():receivedanswer(false),portnum(0){}
		};

		//	std::vector<child_url> childrenModules;
		std::map<std::string,child_url>childrenModules;

		void resetchildrenModules()
		{
			for (std::map<std::string,child_url>::iterator iter=childrenModules.begin();iter!=childrenModules.end();iter++)
				iter->second.receivedanswer=false;
		}

		unsigned int numchildrensyncremaining()
		{
			unsigned int counter=0;
			for (std::map<std::string,child_url>::iterator iter=childrenModules.begin();iter!=childrenModules.end();iter++)
			{
				if (! iter->second.receivedanswer)
				{
					counter++;
				}
			}

			return counter;
		}



		bool SocketsInitialized;
		void createSockets();
		void DETERMINE_AVERAGE_NETWORK_LATENCY();

		void PREPARE_PARENT_CHILD_INFORMATION();
	};




} //namespace ACM{
#endif// _MODULE_MANAGER_H
