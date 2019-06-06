include_directories(${ROOT_INCLUDE})
link_directories(${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir)

set (MM_SOURCES 
ModuleManager.cc 
ModuleManagerExternal.cc
ModuleConfigurationDefinitions.cc
${ACM_BINARY_DIR}/src/ACM_LOGGER.cc
${ACM_BINARY_DIR}/src/QUEUE_CHANNEL.cc 
${ACM_BINARY_DIR}/src/SAMPLING_CHANNEL.cc
ModuleManagerMain.cc
)
message(ACM-1.0-binary "${ACM_BINARY_DIR}/src")
set(MM_EXT_OBJS
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/QUEUE_PORT.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/SAMPLING_PORT.cc.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/UDPSocket.cpp.o
${ACM_BINARY_DIR}/src/CMakeFiles/ACM-1.0.dir/ACM_THREAD_SUPPORT.cc.o
)

#SET_SOURCE_FILES_PROPERTIES(${MM_EXT_OBJS} PROPERTIES EXTERNAL_OBJECT 1 GENERATED 1)

#add_definitions(-DIS_MODULE_MANAGER)
add_executable(ModuleManager ${MM_SOURCES} )
SET_PROPERTY(TARGET ModuleManager APPEND PROPERTY COMPILE_DEFINITIONS IS_MODULE_MANAGER)
ADD_DEPENDENCIES(ModuleManager ACM-1.0)
target_link_libraries(ModuleManager ${MM_EXT_OBJS} rt pthread)

INSTALL(TARGETS  ModuleManager DESTINATION bin)


