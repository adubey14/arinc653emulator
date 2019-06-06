#ifndef NETWORK_PARTITION_CC
#define NETWORK_PARTITION_CC

#include <ACM/INTERNAL/NetworkPartition.h>
#include <ACM/INTERNAL/ModuleManager.h>

using namespace ACM_USER;
using namespace ACM_INTERNAL;
QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME;
QUEUING_PORT_ID_TYPE QUEUING_PORT_ID;

NETWORK_PARTITION* net_partition=0;
ACM_INTERNAL::MODULE_MANAGER* main_instance =0;


static void FireChannels()
{
	RETURN_CODE_TYPE RETURN_CODE = NO_ERROR;
	NETWORK_PARTITION_PETRI_NET_TOKEN_TYPE NETWORK_PARTITION_PETRI_NET_TOKEN_READ;
	MESSAGE_SIZE_TYPE LENGTH;
	while(true)
	{
		pthread_testcancel();

		
		RECEIVE_QUEUING_MESSAGE (
			/*in */ QUEUING_PORT_ID,
			/*in */ INFINITE_TIME_VALUE,
			/*out*/ (MESSAGE_ADDR_TYPE)&NETWORK_PARTITION_PETRI_NET_TOKEN_READ[0],//buf[0],
			/*out*/ &LENGTH,
			/*out*/ &RETURN_CODE );

		if(strcmp(NETWORK_PARTITION_PETRI_NET_TOKEN,NETWORK_PARTITION_PETRI_NET_TOKEN_READ)!=0) continue;

		if (net_partition ==0 || main_instance==0)
		{
			LOGGER(ERROR,"NULL REFERENCE TO NETWORK_PARTITION* or main_instance");
			return;
		}
		for (unsigned counter=0;counter<net_partition->QUEUEING_CHANNELS.size();counter++)
		{
			if (main_instance->MAP_OF_QUEUEING_CHANNELS[net_partition->QUEUEING_CHANNELS[counter]]!=0)
			{
				main_instance->MAP_OF_QUEUEING_CHANNELS[net_partition->QUEUEING_CHANNELS[counter]]->MOVE_MESSAGES(&RETURN_CODE);
				pthread_testcancel();
			}
		}
		for (unsigned counter=0;counter<net_partition->SAMPLING_CHANNELS.size();counter++)
		{
			if (main_instance->MAP_OF_SAMPLING_CHANNELS[net_partition->SAMPLING_CHANNELS[counter]]!=0)
			{
				main_instance->MAP_OF_SAMPLING_CHANNELS[net_partition->SAMPLING_CHANNELS[counter]]->MOVE_MESSAGES(&RETURN_CODE);
				
				pthread_testcancel();
			}
		}
	}
}


int main(int argc, char** argv)
{
	RETURN_CODE_TYPE RETURN_CODE = NO_ERROR;
	if(argc <3) 
	{
		LOGGER(ERROR,"Usage: %s <Config file> <NetworkPartName>",argv[0]);
		return -1;
	}

	net_partition= new NETWORK_PARTITION();
	net_partition->ConfigurationFile=argv[1];
	net_partition->name=argv[2];
	SET_PARTITION_NAME(argv[2]);

	
	ACM_INTERNAL::MODULE_MANAGER::fileName=argv[1];


	main_instance = ACM_INTERNAL::MODULE_MANAGER::Instance(); //create partition vectors


	main_instance->SET_MAJOR_FRAME_SIZE();
	main_instance->SET_MAX_NUMBER_OF_FRAMES();
	main_instance->READ_NUMBER_OF_INIT_STAGES();
	main_instance->SET_MIN_WAIT_TIME();
	main_instance->CREATE_PARTITION_VECTOR();

	unsigned int index=0;
	bool found =false;
	for (unsigned int counter=0;counter<main_instance->PARTITION_VECTOR.size();counter++)
	{
		if (main_instance->PARTITION_VECTOR[counter].Name.compare(net_partition->name)==0)
		{
			found=true;
			index=counter;
			break;
		}

	}

	assert(found);

	
 
	for (unsigned int i=0;i<main_instance->PARTITION_VECTOR[index].QUEUE_CHANNEL_SOURCE_VECTOR.size();i++)
		net_partition->QUEUEING_CHANNELS.push_back(main_instance->PARTITION_VECTOR[index].QUEUE_CHANNEL_SOURCE_VECTOR[i]);

	for (unsigned int i=0;i<main_instance->PARTITION_VECTOR[index].SAMPLING_CHANNEL_SOURCE_VECTOR.size();i++)
			net_partition->SAMPLING_CHANNELS.push_back(main_instance->PARTITION_VECTOR[index].SAMPLING_CHANNEL_SOURCE_VECTOR[i]);

	for (unsigned int i=0;i<main_instance->PARTITION_VECTOR[index].QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR.size();i++)
			net_partition->QUEUEING_CHANNELS.push_back(main_instance->PARTITION_VECTOR[index].QUEUE_CHANNEL_INTER_MODULE_DST_VECTOR[i]);

	
	for (unsigned int i=0;i<main_instance->PARTITION_VECTOR[index].SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR.size();i++)
				net_partition->SAMPLING_CHANNELS.push_back(main_instance->PARTITION_VECTOR[index].SAMPLING_CHANNEL_INTER_MODULE_DST_VECTOR[i]);


	LOGGER(APP,"Inside %s",argv[2]);

	if(main_instance->NumberOfInitStages==2)
	{
		Send_Synchronization_To_MM();
	}


	snprintf(QUEUING_PORT_NAME,MAX_NAME_LENGTH,"%s_config",argv[2]);
	
	CREATE_QUEUING_PORT (
		/*in */ QUEUING_PORT_NAME,
		/*in */ sizeof(NETWORK_PARTITION_PETRI_NET_TOKEN_TYPE),
		/*in */ 10,
		/*in */ DESTINATION,
		/*in */ FIFO,
		/*out*/ &QUEUING_PORT_ID,
		/*out*/ &RETURN_CODE );



	PROCESS_ID_TYPE NET_PART_PROCESS_ID= APEX_HELPER_CREATE_PROCESS(INFINITE_TIME_VALUE, //SYSTEM_TIME_TYPE TIME_CAPACITY,
		INFINITE_TIME_VALUE, //SYSTEM_TIME_TYPE TIME_CAPACITY,
		(SYSTEM_ADDRESS_TYPE)FireChannels, //SYSTEM_ADDRESS_TYPE ENTRY_POINT,
		0,//STACK_SIZE_TYPE STACK_SIZE,
		90,//PRIORITY_TYPE BASE_PRIORITY,
		HARD,//DEADLINE_TYPE DEADLINE,
		argv[2], //std::string PROCESS_NAME,
		&RETURN_CODE); //RETURN_CODE_TYPE *RETURN_CODE)

	assert(RETURN_CODE==NO_ERROR);
	START(NET_PART_PROCESS_ID,&RETURN_CODE);
	assert(RETURN_CODE==NO_ERROR);

	LOGGER(APP,"Setting Partition State", INFO);
	SET_PARTITION_MODE(NORMAL,&RETURN_CODE);
	LOGGER(APP,"%s Finished",argv[2]);

	delete(main_instance);

	delete(net_partition);
}


#endif //NETWORK_PARTITION_CC
