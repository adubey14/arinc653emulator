INCLUDES= -I ../include
bin_PROGRAMS = ModuleManager 
AM_CXXFLAGS= -DIS_MODULE_MANAGER
ModuleManager_SOURCES = ModuleManager.cc ModuleManagerExternal.cc ModuleConfigurationDefinitions.cc ../src/ACM_LOGGER.cc ../src/QUEUE_CHANNEL.cc ../src/SAMPLING_CHANNEL.cc ModuleManagerMain.cc

ModuleManager_LDADD=-lrt -lpthread ../src/QUEUE_PORT.lo ../src/SAMPLING_PORT.lo ../src/UDPSocket.lo ../src/ACM_THREAD_SUPPORT.lo
		

