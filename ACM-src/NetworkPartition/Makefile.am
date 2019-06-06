INCLUDES= -I ../include
bin_PROGRAMS = NetworkPartition
		
NetworkPartition_CXXFLAGS=-DISNETWORK_PARTITION -DIS_MODULE_MANAGER 

NetworkPartition_LDADD=-lrt -lpthread ../src/QUEUE_PORT.lo ../src/SAMPLING_PORT.lo ../src/UDPSocket.lo ../src/ACM_THREAD_SUPPORT.lo ../src/APEX.lo ../src/APEX_BLACKBOARD_SERVICES.lo ../src/APEX_BUFFER_SERVICES.lo ../src/APEX_EVENT_SERVICES.lo ../src/APEX_HM_SERVICES.lo ../src/APEX_INTERFACE_IMPL.lo ../src/APEX_PROCESS_SERVICES.lo ../src/APEX_QUEUING_SERVICES.lo ../src/APEX_SAMPLING_SERVICES.lo ../src/APEX_SCHEDULING.lo ../src/APEX_SEMAPHORE_SERVICES.lo ../src/APEX_TIME_MANAGEMENT_SERVICES.lo ../src/BLACKBOARD.lo ../src/EVENT.lo ../src/EVENT_PORT.lo  ../src/BUFFER.lo 

NetworkPartition_SOURCES = ../ModuleManager/ModuleManager.cc ../ModuleManager/ModuleManagerExternal.cc ../ModuleManager/ModuleConfigurationDefinitions.cc ../src/QUEUE_CHANNEL.cc ../src/SAMPLING_CHANNEL.cc ../src/ACM_LOGGER.cc ../ModuleManager/NetworkPartition.cc ../src/PROCESS.cc 

