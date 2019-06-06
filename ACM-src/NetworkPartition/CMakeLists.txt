include_directories(${ROOT_INCLUDE})
#link_directories(${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir)

set (NP_SOURCES 
${ACM_SOURCE_DIR}/ModuleManager/ModuleManager.cc
${ACM_SOURCE_DIR}/ModuleManager/ModuleManagerExternal.cc
${ACM_SOURCE_DIR}/ModuleManager/ModuleConfigurationDefinitions.cc
${ACM_SOURCE_DIR}/src/ACM_LOGGER.cc
${ACM_SOURCE_DIR}/src/QUEUE_CHANNEL.cc 
${ACM_SOURCE_DIR}/src/SAMPLING_CHANNEL.cc
${ACM_SOURCE_DIR}/ModuleManager/NetworkPartition.cc 
${ACM_SOURCE_DIR}/src/PROCESS.cc
)

set (NP_EXT_OBJS
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/QUEUE_PORT.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/SAMPLING_PORT.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/UDPSocket.cpp.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/ACM_THREAD_SUPPORT.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/APEX.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/APEX_BLACKBOARD_SERVICES.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/APEX_BUFFER_SERVICES.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/APEX_EVENT_SERVICES.cc.o 
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/APEX_HM_SERVICES.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/APEX_INTERFACE_IMPL.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/APEX_PROCESS_SERVICES.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/APEX_QUEUING_SERVICES.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/APEX_SAMPLING_SERVICES.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/APEX_SCHEDULING.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/APEX_SEMAPHORE_SERVICES.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/APEX_TIME_MANAGEMENT_SERVICES.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/BLACKBOARD.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/EVENT.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/EVENT_PORT.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/BUFFER.cc.o 
)
#SET_SOURCE_FILES_PROPERTIES(${NP_EXT_OBJS} PROPERTIES EXTERNAL_OBJECT 1 GENERATED 1)

#add_definitions(-DISNETWORK_PARTITION -DIS_MODULE_MANAGER )
add_executable(NetworkPartition ${NP_SOURCES}  )
SET_PROPERTY(TARGET NetworkPartition APPEND PROPERTY COMPILE_DEFINITIONS ISNETWORK_PARTITION IS_MODULE_MANAGER)


ADD_DEPENDENCIES(NetworkPartition ACM-1.0)
target_link_libraries(NetworkPartition ${NP_EXT_OBJS} rt pthread) 

INSTALL(TARGETS  NetworkPartition DESTINATION bin)

