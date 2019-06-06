#ifndef _MODULE_CONFIGURATION_DEFN_CC
#define _MODULE_CONFIGURATION_DEFN_CC

#include <ACM/INTERNAL/ModuleManager.h>

namespace ACM_INTERNAL{
	// Definition of String Names
	const char* PRIMARY_KEY_NETWORK_PARTITION_NAME="NETWORK_PARTITION_NAME";
	
	const char* ADDITVE_KEY_CHANNEL= "_CHANNEL";
	const char* PRIMARY_KEY_SYNCHRONIZED_EXECUTION="SYNCHRONIZED_EXECUTION";
	const char* PRIMARY_KEY_BROADCAST_ADDRESS="BROADCAST_ADDRESS";
	const char* PRIMARY_KEY_MULTICAST_ADDRESS="MULTICAST_ADDRESS";
	const char* PRIMARY_KEY_DISCOVERY_PROTOCOL="DISCOVERY_PROTOCOL";
	const char* PRIMARY_KEY_DISCOVERY_PROTOCOL_IOR="IOR";
	const char* PRIMARY_KEY_DISCOVERY_PROTOCOL_NS="NS";
	const char* PRIMARY_KEY_DISCOVERY_PROTOCOL_NS_HOST="NS_HOST";
	const char* PRIMARY_KEY_DISCOVERY_PROTOCOL_NS_PORT="NS_PORT";
	const char* PRIMARY_KEY_HYPERPERIOD_KEY="HYPERPERIOD";
	const char* PRIMARY_KEY_MAXITERATIONS="MAXITERATIONS";
	const char* PRIMARY_KEY_PARTITION_NAME="PARTITION_NAME";
	const char* PRIMARY_KEY_CPU="CPU";
	// const char* PRIMARY_KEY_PARENT_INIT_TIMEOUT="PARENT_INIT_TIMEOUT";
	const char* PRIMARY_KEY_PARTITION_INIT_TIMEOUT="PARTITION_INIT_TIMEOUT";
	//ztry to connect to parent. if failed wait till timeout...try again...if failed stop.
	const char* PRIMARY_KEY_CHANNEL_NAME="CHANNEL_NAME";
	const char* PRIMARY_KEY_NUMBER_OF_INITIAL_STAGES="NUMBER_OF_INIT_STAGES";
	const char* PRIMARY_KEY_PROXY_PORT="PROXY_PORT";
	const char* PRIMARY_KEY_CONFIGURATION_PORT="CONFIGURATION_PORT";
	const char* PRIMARY_KEY_PARENT_PORT="PARENT_PORT";
	const char* PRIMARY_KEY_MODULE_NAME="MODULE_NAME";
	const char* PRIMARY_KEY_PARENT_MODULE_NAME="PARENT_MODULE_NAME";
	const char* PRIMARY_KEY_CHILD_MODULE_NAME="CHILD_MODULE_NAME";
	const char* ADDITVE_KEY_MM_HOSTNAME="_HOSTNAME";
	// const char* ADDITVE_KEY_MM_PORTNUM="_PORTNUM";
	const char* ADDITIVE_KEY__PORT_NUMBER="_PORT_NUMBER";
	const char* ADDITIVE_KEY__MODULE_NAME="_MODULE_NAME";
	const char* ADDITIVE_KEY__TYPE="_TYPE";
	const char* PROXY_TYPE_SAMPLING="SAMPLING";
	const char* PROXY_TYPE_QUEUEING="QUEUING";
	const char* ADDITIVE_KEY__DIRECTION="_DIRECTION";
	const char* ADDITIVE_KEY__DESTINATION="_DESTINATION";
	const char* ADDITIVE_KEY__SOURCE="_SOURCE";
	const char* ADDITIVE_KEY__SCHEDULE="_SCHEDULE";
	const char* ADDITIVE_KEY__EXECUTABLE="_EXECUTABLE";
	const char* ADDITIVE_KEY__SAMPLINGPORT="_SAMPLINGPORT";
	const char* ADDITIVE_KEY__MAXMESSAGESIZE="_MAXMESSAGESIZE";
	const char* ADDITIVE_KEY__REFRESHPERIOD="_REFRESHPERIOD";
	const char* ADDITIVE_KEY__QUEUINGPORT="_QUEUINGPORT";
	const char* ADDITIVE_KEY__MAXNUMBEROFMESSAGES="_MAXNUMBEROFMESSAGES";


	void print_key_values()		 
	{
		printf("PRIMARY_KEY_NETWORK_PARTITION_NAME=%s\n",PRIMARY_KEY_NETWORK_PARTITION_NAME);
		printf("ADDITVE_KEY_CHANNEL=%s\n",ADDITVE_KEY_CHANNEL);
		printf("PRIMARY_KEY_SYNCHRONIZED_EXECUTION=%s\n",PRIMARY_KEY_SYNCHRONIZED_EXECUTION);
		printf("PRIMARY_KEY_BROADCAST_ADDRESS=%s\n",PRIMARY_KEY_BROADCAST_ADDRESS);
		printf("PRIMARY_KEY_MULTICAST_ADDRESS=%s\n",PRIMARY_KEY_MULTICAST_ADDRESS);
		printf("PRIMARY_KEY_DISCOVERY_PROTOCOL=%s\n",PRIMARY_KEY_DISCOVERY_PROTOCOL);
		printf("PRIMARY_KEY_DISCOVERY_PROTOCOL_IOR=%s\n",PRIMARY_KEY_DISCOVERY_PROTOCOL_IOR);
		printf("PRIMARY_KEY_DISCOVERY_PROTOCOL_NS=%s\n",PRIMARY_KEY_DISCOVERY_PROTOCOL_NS);
		printf("PRIMARY_KEY_DISCOVERY_PROTOCOL_NS_HOST=%s\n",PRIMARY_KEY_DISCOVERY_PROTOCOL_NS_HOST);
		printf("PRIMARY_KEY_DISCOVERY_PROTOCOL_NS_PORT=%s\n",PRIMARY_KEY_DISCOVERY_PROTOCOL_NS_PORT);
		printf("PRIMARY_KEY_HYPERPERIOD_KEY=%s\n",PRIMARY_KEY_HYPERPERIOD_KEY);
		printf("PRIMARY_KEY_MAXITERATIONS=%s\n",PRIMARY_KEY_MAXITERATIONS);
		printf("PRIMARY_KEY_PARTITION_NAME=%s\n",PRIMARY_KEY_PARTITION_NAME);
		printf("PRIMARY_KEY_CPU=%s\n",PRIMARY_KEY_CPU);
		printf("PRIMARY_KEY_PARTITION_INIT_TIMEOUT=%s\n",PRIMARY_KEY_PARTITION_INIT_TIMEOUT);
		printf("PRIMARY_KEY_CHANNEL_NAME=%s\n",PRIMARY_KEY_CHANNEL_NAME);
		printf("PRIMARY_KEY_NUMBER_OF_INITIAL_STAGES=%s\n",PRIMARY_KEY_NUMBER_OF_INITIAL_STAGES);
		printf("PRIMARY_KEY_PROXY_PORT=%s\n",PRIMARY_KEY_PROXY_PORT);
		printf("PRIMARY_KEY_CONFIGURATION_PORT=%s\n",PRIMARY_KEY_CONFIGURATION_PORT);
		printf("PRIMARY_KEY_PARENT_PORT=%s\n",PRIMARY_KEY_PARENT_PORT);
		printf("PRIMARY_KEY_MODULE_NAME=%s\n",PRIMARY_KEY_MODULE_NAME);
		printf("PRIMARY_KEY_PARENT_MODULE_NAME=%s\n",PRIMARY_KEY_PARENT_MODULE_NAME);
		printf("PRIMARY_KEY_CHILD_MODULE_NAME=%s\n",PRIMARY_KEY_CHILD_MODULE_NAME);
		printf("ADDITVE_KEY_MM_HOSTNAME=%s\n",ADDITVE_KEY_MM_HOSTNAME);
		printf("ADDITIVE_KEY__PORT_NUMBER=%s\n",ADDITIVE_KEY__PORT_NUMBER);
		printf("ADDITIVE_KEY__MODULE_NAME=%s\n",ADDITIVE_KEY__MODULE_NAME);
		printf("ADDITIVE_KEY__TYPE=%s\n",ADDITIVE_KEY__TYPE);
		printf("PROXY_TYPE_SAMPLING=%s\n",PROXY_TYPE_SAMPLING);
		printf("PROXY_TYPE_QUEUEING=%s\n",PROXY_TYPE_QUEUEING);
		printf("ADDITIVE_KEY__DIRECTION=%s\n",ADDITIVE_KEY__DIRECTION);
		printf("ADDITIVE_KEY__DESTINATION=%s\n",ADDITIVE_KEY__DESTINATION);
		printf("ADDITIVE_KEY__SOURCE=%s\n",ADDITIVE_KEY__SOURCE);
		printf("ADDITIVE_KEY__SCHEDULE=%s\n",ADDITIVE_KEY__SCHEDULE);
		printf("ADDITIVE_KEY__EXECUTABLE=%s\n",ADDITIVE_KEY__EXECUTABLE);
		printf("ADDITIVE_KEY__SAMPLINGPORT=%s\n",ADDITIVE_KEY__SAMPLINGPORT);
		printf("ADDITIVE_KEY__MAXMESSAGESIZE=%s\n",ADDITIVE_KEY__MAXMESSAGESIZE);
		printf("ADDITIVE_KEY__REFRESHPERIOD=%s\n",ADDITIVE_KEY__REFRESHPERIOD);
		printf("ADDITIVE_KEY__QUEUINGPORT=%s\n",ADDITIVE_KEY__QUEUINGPORT);
		printf("ADDITIVE_KEY__MAXNUMBEROFMESSAGES=%s\n",ADDITIVE_KEY__MAXNUMBEROFMESSAGES);
	};


};

#endif