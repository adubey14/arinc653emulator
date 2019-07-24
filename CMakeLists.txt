cmake_minimum_required (VERSION 2.8)
project (F6ACM)
INCLUDE(ExternalProject)

IF (NOT DEFINED MBSHM_ROOT)
IF (NOT DEFINED $ENV{MBSHM_ROOT})
SET (ENV{MBSHM_ROOT} "${F6ACM_SOURCE_DIR}/ACM-I")
ENDIF()
set(MBSHM_ROOT $ENV{MBSHM_ROOT})
ENDIF()

IF (NOT DEFINED ACM_SRC)
IF (NOT DEFINED $ENV{ACM_SRC})
SET (ENV{ACM_SRC} "${F6ACM_SOURCE_DIR}/ACM-src")
ENDIF()
set(ACM_SRC $ENV{ACM_SRC})
ENDIF()


set(ACM_ROOT $ENV{ACM_ROOT})

set(ENV{PATH} "$ENV{PATH}:${MBSHM_ROOT}/bin:${ACE_ROOT}/bin")
set(ENV{LD_LIBRARY_PATH} "$ENV{LD_LIBRARY_PATH}:${MBSHM_ROOT}/lib")




set(acm_build_args -Dlogging-info=OFF -Dlogging-error=ON -DBUILD_EXTERNALS=0;-DCMAKE_INSTALL_PREFIX=${ACM_ROOT}) 
set(proj_acm_build ACM_BUILD)
ExternalProject_Add(${proj_acm_build}
  SOURCE_DIR ${ACM_SRC}
  INSTALL_DIR ${ACM_ROOT}
  CMAKE_ARGS ${acm_build_args}
  BUILD_COMMAND  $(MAKE)
  BUILD_IN_SOURCE 1
  INSTALL_COMMAND $(MAKE) install
)
set_property(TARGET ${proj_acm_build} PROPERTY FOLDER "${proj_acm_build}")
