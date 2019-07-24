cmake_minimum_required (VERSION 2.8)
project (ACM)
INCLUDE (CheckIncludeFiles) 
INCLUDE (CheckLibraryExists) 
INCLUDE (CheckFunctionExists)
INCLUDE(CheckCXXSourceCompiles)
INCLUDE(FindDoxygen)
INCLUDE(ExternalProject)

option(ENABLE_DEBUG "Build With Additional Debugging Code" OFF)
option(BUILD_EXTERNALS "Build External Projects" ON)
option(USE_PTHREADS "Use PThreads" ON)

option(logging-info "Log Info" ON)
option(logging-warning "Log warning" OFF)
option(logging-debug "Log debug" OFF)
option(logging-error "Log error" OFF)
option(logging-fatal "Log fatal" OFF)
option(logging-trace "Log trace" OFF)
option(logging-hmevent "Log hmevent" OFF)
option(logging-app "Log app" OFF)


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSCHEDRR")

IF(ENABLE_DEBUG)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
ENDIF(ENABLE_DEBUG)


# The version number.
set(GENERIC_LIBRARY_NAME "ACM")
set (GENERIC_MAJOR_VERSION "1")
set (GENERIC_MINOR_VERSION "0")
set (GENERIC_MICRO_VERSION "3")
set(GENERIC_LIBRARY_VERSION "1:0:0")
set(GENERIC_RELEASE "${GENERIC_MAJOR_VERSION}.${GENERIC_MINOR_VERSION}")
set (ACM_VERSION_MAJOR 1)
set (ACM_VERSION_MINOR 0)
set(PACKAGE ${GENERIC_LIBRARY_NAME})
set(GENERIC_API_VERSION 1.0)
set(VERSION ${GENERIC_MAJOR_VERSION}.${GENERIC_MINOR_VERSION}.${GENERIC_MICRO_VERSION})

CHECK_INCLUDE_FILES(dlfcn.h HAVE_DLFCN_H)
CHECK_INCLUDE_FILES(inttypes.h HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES(memory.h HAVE_MEMORY_H)
CHECK_INCLUDE_FILES(pthread.h HAVE_PTHREAD_H)
CHECK_LIBRARY_EXISTS(pthread pthread_condattr_setclock "" HAVE_PTHREAD_CONDATTR_SETCLOCK)
CHECK_LIBRARY_EXISTS(pthread pthread_create "" HAVE_PTHREAD_CREATE)
CHECK_LIBRARY_EXISTS(rt clock_nanosleep "" HAVE_RT)
CHECK_INCLUDE_FILES(sched.h HAVE_SCHED_H)
CHECK_INCLUDE_FILES(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILES(stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(strings.h HAVE_STRINGS_H)
CHECK_INCLUDE_FILES(string.h HAVE_STRING_H)
CHECK_INCLUDE_FILES("sys/stat.h" HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES("sys/types.h" HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES(unistd.h HAVE_UNISTD_H)

IF(USE_PTHREADS)
IF(NOT (HAVE_PTHREAD_H OR HAVE_SCHED_H OR HAVE_PTHREAD_CONDATTR_SETCLOCK OR HAVE_PTHREAD_CREATE))
MESSAGE(SEND_ERROR "PTHREADS NOT AVAILABLE")
ENDIF()
ENDIF(USE_PTHREADS)

CHECK_LIBRARY_EXISTS(pthread pthread_create "" HAVE_PTHREAD_CREATE)

CHECK_FUNCTION_EXISTS(strerror HAVE_STRERROR) 
if(NOT HAVE_STRERROR)
CHECK_LIBRARY_EXISTS(cposix strerror "" HAVE_STRERROR)
if(HAVE_STRERROR)
find_library(LIBCPOSIX cposix)
set(LIBS ${LIBS} ${LIBCPOSIX_LIBRARY})
endif(HAVE_STRERROR)
endif(NOT HAVE_STRERROR)
  
configure_file (
  "${ACM_SOURCE_DIR}/include/config.h.in"
  "${ACM_SOURCE_DIR}/include/config.h"
  )
  

set(acm_top_builddir ${ACM_SOURCE_DIR})
set(acm_top_srcdir ${ACM_SOURCE_DIR})
set(library_includedir "include/${GENERIC_LIBRARY_NAME}-${GENERIC_API_VERSION}")
set(ROOT_INCLUDE ${ACM_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(ROOT_INCLUDE)

IF (NOT DEFINED ACM_ROOT)
IF (NOT DEFINED $ENV{ACM_ROOT})
SET (ENV{ACM_ROOT} "${ACM_SOURCE_DIR}/../ACM-I")
ENDIF()
set(ACM_ROOT $ENV{ACM_ROOT})
ENDIF()

IF (NOT DEFINED MBSHM_ROOT)
IF (NOT DEFINED $ENV{MBSHM_ROOT})
SET (ENV{MBSHM_ROOT} "${ACM_SOURCE_DIR}/../ACM-I")
ENDIF()
set(MBSHM_ROOT $ENV{MBSHM_ROOT})
ENDIF()

IF (NOT DEFINED ACM_SRC)
IF (NOT DEFINED $ENV{ACM_SRC})
SET (ENV{ACM_SRC} "${ACM_SOURCE_DIR}")
ENDIF()
set(ACM_SRC $ENV{ACM_SRC})
ENDIF()


IF (NOT DEFINED ACE_ROOT)
IF (NOT DEFINED $ENV{ACE_ROOT})
SET (ENV{ACE_ROOT} "${ACM_SOURCE_DIR}/../DOC_ROOT/ACE")
ENDIF()
set(ACE_ROOT $ENV{ACE_ROOT})
ENDIF()

IF (NOT DEFINED MPC_ROOT)
IF (NOT DEFINED $ENV{MPC_ROOT})
SET (ENV{MPC_ROOT} "${ACM_SOURCE_DIR}/../DOC_ROOT/MPC")
ENDIF()
set(MPC_ROOT $ENV{MPC_ROOT})
ENDIF()

IF (NOT DEFINED TAO_ROOT)
IF (NOT DEFINED $ENV{TAO_ROOT})
SET (ENV{TAO_ROOT} "${ACM_SOURCE_DIR}/../DOC_ROOT/TAO")
ENDIF()
set(TAO_ROOT $ENV{TAO_ROOT})
ENDIF()

IF (NOT DEFINED CIAO_ROOT)
IF (NOT DEFINED $ENV{CIAO_ROOT})
SET (ENV{CIAO_ROOT} "${ACM_SOURCE_DIR}/../DOC_ROOT/CIAO")
ENDIF()
set(CIAO_ROOT $ENV{CIAO_ROOT})
ENDIF()

IF (NOT DEFINED DDS_ROOT)
IF (NOT DEFINED $ENV{DDS_ROOT})
SET (ENV{DDS_ROOT} "${ACM_SOURCE_DIR}/../DOC_ROOT/DDS")
ENDIF()
set(DDS_ROOT $ENV{DDS_ROOT})
ENDIF()

set(ENV{PATH} "$ENV{PATH}:${MBSHM_ROOT}/bin:${ACE_ROOT}/bin:${DDS_ROOT}/bin")
set(ENV{LD_LIBRARY_PATH} "$ENV{LD_LIBRARY_PATH}:${MBSHM_ROOT}/lib:${ACE_ROOT}/lib:${DDS_ROOT}/lib")


IF (logging-info)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_LOGGING_INFO")
ENDIF()
IF (logging-warning)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_LOGGING_WARNING")
ENDIF()
IF (logging-debug)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_LOGGING_DEBUG")
ENDIF()
IF (logging-error)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_LOGGING_ERROR")
ENDIF()
IF (logging-fatal)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_LOGGING_FATAL")
ENDIF()
IF (logging-trace)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_LOGGING_TRACE")
ENDIF()
IF (logging-hmevent)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_LOGGING_HMEVENT")
ENDIF()
IF (logging-app)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_LOGGING_APP")
ENDIF()


configure_file (
  "${ACM_SOURCE_DIR}/include/ACM/USER/ACM_USER_INCLUDE.h.in"
  "${ACM_BINARY_DIR}/include/ACM/USER/ACM_USER_INCLUDE.h"
  )
set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${prefix})
set(libdir ${exec_prefix}/lib)
set(includedir ${prefix}/include)
configure_file (
  "${ACM_SOURCE_DIR}/ACM-1.0.pc.in"
  "${ACM_BINARY_DIR}/ACM-1.0.pc"
  )

#ADD_CUSTOM_TARGET(ACM-1.0.pc ALL)
#ADD_CUSTOM_COMMAND(
#  TARGET ACM-1.0.pc
#  OUTPUTS ${ACM_BINARY_DIR}/ACM-1.0.pc
#  COMMAND cp ${ACM_SOURCE_DIR}/ACM-1.0.pc.in ${ACM_BINARY_DIR}/ACM-1.0.pc
#  DEPENDS ${ACM_SOURCE_DIR}/ACM-1.0.pc.in ${proj_ciao_acm} 
#)

add_subdirectory(src)
add_subdirectory(ModuleManager)
add_subdirectory(NetworkPartition)
add_subdirectory(StateMachine)


FIND_PACKAGE(Doxygen)
if(DOXYGEN_FOUND)
set(HAVE_DOXYGEN 1)
configure_file (
  "${ACM_SOURCE_DIR}/Doxyfilecmake.in"
  "${ACM_BINARY_DIR}/Doxyfile"
  )
 configure_file (
  "${ACM_SOURCE_DIR}/DoxyfileUsercmake.in"
  "${ACM_BINARY_DIR}/DoxyfileUser"
  )
else()
set(HAVE_DOXYGEN 0)
endif()

#if (HAVE_DOXYGEN)
#add_subdirectory(docs)
#endif()

ADD_CUSTOM_TARGET(realclean 
                make clean
				COMMAND ${ACM_SOURCE_DIR}/realclean.sh)

ADD_CUSTOM_TARGET(distclean 
                make clean
				COMMAND ${ACM_SOURCE_DIR}/realclean.sh
				COMMAND ${ACM_SOURCE_DIR}/distclean.sh)				
INSTALL(DIRECTORY DESTINATION ${library_includedir})
INSTALL(DIRECTORY utilityscripts/ DESTINATION bin FILES_MATCHING PATTERN "*.py")
INSTALL(DIRECTORY arincdemo/ DESTINATION arincdemo  PATTERN "svn" EXCLUDE)
INSTALL(CODE " execute_process( COMMAND chmod +x autogen.sh  WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/arincdemo) " )
#INSTALL(CODE " execute_process( COMMAND make install  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/3rdParty/mico-source) " )
#INSTALL(FILES 3rdParty/tfpg/lib/libdiagtfpg-g++4.3.3-2.6.28-32bit.so RENAME libdiagtfpg.so DESTINATION lib )
#INSTALL(DIRECTORY 3rdParty/tfpg/include/ DESTINATION include/tfpg FILES_MATCHING PATTERN "*.h")
INSTALL(FILES LICENSE.txt LICENSE.pdf DESTINATION .)
FILE(WRITE bootstrap.sh "export ACM_ROOT=${CMAKE_INSTALL_PREFIX}\n")
FILE(APPEND bootstrap.sh "export MBSHM_ROOT=${CMAKE_INSTALL_PREFIX}\n")
FILE(APPEND bootstrap.sh "export PATH=\$PATH:\$MBSHM_ROOT/bin\n")
FILE(APPEND bootstrap.sh "export ACM_LOG_ENTRIES=1000\n")
FILE(APPEND bootstrap.sh "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:\$MBSHM_ROOT/lib\n")
FILE(APPEND bootstrap.sh "export PKG_CONFIG_PATH=\$MBSHM_ROOT/lib/pkgconfig/\n") 
INSTALL(PROGRAMS ${ACM_SOURCE_DIR}/bootstrap.sh DESTINATION .)
#FILE(REMOVE ${PROJECT_SOURCE_DIR}/bootstrap.sh)
set(install_pkgconfig_dir ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
INSTALL(FILES ACM-1.0.pc DESTINATION ${install_pkgconfig_dir})
INSTALL(CODE "MESSAGE(\"----------------------------------------------------------------------\")")
INSTALL(CODE "MESSAGE(\" \")")
INSTALL(CODE "MESSAGE(\" \")")
INSTALL(CODE "MESSAGE(\" A pkg_config file is being installed in ${install_pkgconfig_dir}\")")
INSTALL(CODE "MESSAGE( \" Put the export PKG_CONFIG_PATH=$$PKG_CONFIG_PATH:${install_pkgconfig_dir} command in you .profile or .bashrc\")")
INSTALL(CODE "MESSAGE( \" ${install_pkgconfig_dir}/arincdemo contains examples on how to use pkg-config for linking against libraries\")")
INSTALL(CODE "MESSAGE( \" see man pkg-config more information about pkg-config\")")
INSTALL(CODE "MESSAGE( \" \")")
INSTALL(CODE "MESSAGE( \"----------------------------------------------------------------------\")")
