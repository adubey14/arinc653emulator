include_directories(${ROOT_INCLUDE})
include_directories(${ACM_SOURCE_DIR})
link_directories(${ACM_SOURCE_DIR}/src)

add_library(StateMachine-1.0-static STATIC
${ACM_SOURCE_DIR}/StateMachine/src/StateMachine/globals.cpp 
${ACM_SOURCE_DIR}/StateMachine/src/StateMachine/StateMachine.cpp 
${ACM_SOURCE_DIR}/src/ACM_TIMER.cc )
target_link_libraries(StateMachine-1.0-static  rt pthread ACM-1.0) 
#SET_TARGET_PROPERTIES(StateMachine-1.0-static PROPERTIES LINK_FLAGS "-version-info ${GENERIC_LIBRARY_VERSION} -release ${GENERIC_RELEASE}")
SET_TARGET_PROPERTIES(StateMachine-1.0-static PROPERTIES OUTPUT_NAME "StateMachine-1.0")
SET_TARGET_PROPERTIES(StateMachine-1.0-static PROPERTIES PREFIX "lib")


add_library(StateMachine-1.0 SHARED
${ACM_SOURCE_DIR}/StateMachine/src/StateMachine/globals.cpp 
${ACM_SOURCE_DIR}/StateMachine/src/StateMachine/StateMachine.cpp 
${ACM_SOURCE_DIR}/src/ACM_TIMER.cc )
target_link_libraries(StateMachine-1.0  rt pthread ACM-1.0) 
#SET_TARGET_PROPERTIES(StateMachine-1.0 PROPERTIES LINK_FLAGS "-version-info ${GENERIC_LIBRARY_VERSION} -release ${GENERIC_RELEASE}")

INSTALL(DIRECTORY ${ACM_SOURCE_DIR}/include/StateMachine/ DESTINATION ${library_includedir}/StateMachine FILES_MATCHING PATTERN "*.h")




                                                                                              
