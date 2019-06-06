#CONF_DEBUG_FLAGS=-g
CXXFLAGS=-O2
CONF_LOGGER_FLAGS=-DENABLE_LOGGING
dnl --------------------------------
dnl Initialization macros.
dnl --------------------------------
MICOVERSION  = 2.3.13
AC_INIT(src/PROCESS.cc)
AC_CHECK_LIB(pthread, open, use_pthreads=yes, use_pthreads=no)
if test X"$use_pthreads" = Xyes; then
	AC_CHECK_HEADERS(pthread.h sched.h,,AC_MSG_ERROR(pthread header files incomplete))
 	else
	AC_MSG_ERROR("Your platform does not support POSIX-threads !")
	exit -1
fi
AC_CANONICAL_HOST
AC_CONFIG_HEADERS(include/config.h)
dnl -----------------------------------------------
dnl Package name and version number (user defined)
dnl -----------------------------------------------
GENERIC_LIBRARY_NAME=ACM
#release versioning
GENERIC_MAJOR_VERSION=1
GENERIC_MINOR_VERSION=0
GENERIC_MICRO_VERSION=3

#API version (often = GENERIC_MAJOR_VERSION.GENERIC_MINOR_VERSION)
GENERIC_API_VERSION=1.0
AC_SUBST(GENERIC_API_VERSION)

#shared library versioning
GENERIC_LIBRARY_VERSION=1:0:0
#                       | | |
#                +------+ | +---+
#                |        |     |
#             current:revision:age
#                |        |     |
#                |        |     +- increment if interfaces have been added
#                |        |        set to zero if interfaces have been removed
#                                  or changed
#                |        +- increment if source code has changed
#                |           set to zero if current is incremented
#                +- increment if interfaces have been added, removed or changed

LT_VERSION=3:0:0
AC_SUBST([LT_VERSION])

dnl --------------------------------
dnl Package name and version number
dnl --------------------------------

AC_SUBST(GENERIC_LIBRARY_VERSION)

PACKAGE=$GENERIC_LIBRARY_NAME
AC_SUBST(GENERIC_LIBRARY_NAME)

GENERIC_VERSION=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION.$GENERIC_MICRO_VERSION
GENERIC_RELEASE=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION
AC_SUBST(GENERIC_RELEASE)
AC_SUBST(GENERIC_VERSION)
dnl Debugging or release build?

AC_ARG_ENABLE([debug],
    [  --enable-debug           build with additional debugging code],
    [AX_CXXFLAGS_GCC_OPTION([-g])])

AC_ARG_VAR(ACE_ROOT, [path to ACE ROOT])
AC_ARG_VAR(MPC_ROOT, [path to MPC ROOT])
AC_ARG_VAR(TAO_ROOT, [path to TAO ROOT])
AC_ARG_VAR(CIAO_ROOT, [path to CIAO ROOT])
AC_ARG_VAR(DDS_ROOT, [path to OPEN DDS ROOT])
mico=no
ciao=no
ciaodds=no
DRengine=no
ciao=yes
ciaodds=yes
echo "configure ciao and Open DDS"
AX_CXXFLAGS_GCC_OPTION([-DUSE_CIAO])
AX_CXXFLAGS_GCC_OPTION([-DF6ACM])
generateciaotypes=yes
generateddstypes=yes
if test "$ACE_ROOT" = ""; then
        echo "ACE_ROOT environment variable not set"
        exit -1
else
        echo "Using ACE_ROOT=$ACE_ROOT"
fi
if test "$MPC_ROOT" = ""; then
        echo "MPC_ROOT environment variable not set"
        exit -1
else
        echo "Using MPC_ROOT=$MPC_ROOT"
fi
if test "$TAO_ROOT" = ""; then
        echo "TAO_ROOT environment variable not set"
        exit -1
else
        echo "Using TAO_ROOT=$TAO_ROOT"
fi
if test "$CIAO_ROOT" = ""; then
        echo "CIAO_ROOT environment variable not set"
        exit -1
else
        echo "Using CIAO_ROOT=$CIAO_ROOT"
fi
if test "$DDS_ROOT" = ""; then
        echo "DDS_ROOT environment variable not set"
        exit -1
else
        echo "Using DDS_ROOT=$DDS_ROOT"
fi
ACM_CORBA_INCLUDE_FILES="#include <ACM/USER/ACM_IDL_HELPER.h>"
corba_cflags="-DACE_NO_INLINE -I$ACE_ROOT -I$TAO_ROOT  -I$CIAO_ROOT  -I$CIAO_ROOT/ccm  -I$TAO_ROOT/orbsvcs -I$DDS_ROOT"
corba_libraries="-L$ACE_ROOT/lib -lCIAO_Servant_Impl -lCIAO_Session_Container -lCIAO_Session_Container_stub -lCIAO_Container_Base_stub -lCIAO_Base_stub -lCCM_Cookie_Factory -lCCM_svnt -lCCM_session_stub -lCCM_lem_stub -lCCM_stub -lCIAO_Servant_Activator_impl -lCIAO_Servant_Activator_stub -lCIAO_Port_Activator_stub  -lTAO_IFR_Client -lTAO_Utils -lTAO_CosNaming -lTAO_Messaging -lTAO_PortableServer -lTAO_Valuetype -lTAO_PI -lTAO_CodecFactory -lTAO_AnyTypeCode -lTAO -lCIAO_Logger -lACE -L$DDS_ROOT/lib -lOpenDDS_Dcps"
rm -f ./include/ACM/USER/ACM_IDL_TYPES.h ./include/ACM/USER/ACM_IDL_TYPES.cc ./include/ACM/USER/ACM_IDL_TYPESC.h ./include/ACM/USER/ACM_IDL_TYPESC.cpp

AC_SUBST(TAO_ROOT)
AC_SUBST(ACE_ROOT)
AC_SUBST(CIAO_ROOT)
AC_SUBST(DDS_ROOT)
AC_SUBST(MPC_ROOT)

AM_CONDITIONAL(generatemicotypes, test "x$mico" = "xyes")
AM_CONDITIONAL(generateciaotypes, test "x$ciao" = "xyes")
AM_CONDITIONAL(generateciaotypes, test "x$ciao" = "xyes")
AM_CONDITIONAL(generateddstypes, test "x$ciaodds" = "xyes")
  AC_SUBST(extra_compile_dir)
  AC_SUBST(corba_libraries)  
  AC_SUBST(corba_cflags)  
AC_SUBST(src_lib_add)

echo "Scheduling policy set to SCHED_RR"
AX_CXXFLAGS_GCC_OPTION([-DSCHEDRR])
  

 

  
  
  
  AC_ARG_ENABLE([logging-info],
  [AS_HELP_STRING([--enable-logging-info],
    [enableLogging at info level. INFO<WARNING<DEBUG<TRACE<HMEVENT<APP< ERROR<FATAL])],
  [AX_CXXFLAGS_GCC_OPTION([-DENABLE_LOGGING_INFO])],
  [echo enable-logging-info=no])

  AC_ARG_ENABLE([logging-warning],
  [AS_HELP_STRING([--enable-logging-warning],
    [enableLogging at warning level.])],
  [AX_CXXFLAGS_GCC_OPTION([-DENABLE_LOGGING_WARNING])],
  [echo enable-logging-warning=no])



  AC_ARG_ENABLE([logging-debug],
  [AS_HELP_STRING([--enable-logging-debug],
    [enableLogging at debug level.])],
  [AX_CXXFLAGS_GCC_OPTION([-DENABLE_LOGGING_DEBUG])],
  [echo enable-logging-debug=no])

    AC_ARG_ENABLE([logging-error],
  [AS_HELP_STRING([--enable-logging-error],
    [enableLogging at error level.])],
  [AX_CXXFLAGS_GCC_OPTION([-DENABLE_LOGGING_ERROR])],
  [echo enable-logging-error=no])

       AC_ARG_ENABLE([logging-fatal],
  [AS_HELP_STRING([--enable-logging-fatal],
    [enableLogging at fatal level.])],
  [AX_CXXFLAGS_GCC_OPTION([-DENABLE_LOGGING_FATAL])],
  [echo enable-logging-fatal=no])

  AC_ARG_ENABLE([logging-trace],
  [AS_HELP_STRING([--enable-logging-trace],
    [enableLogging at trace level.])],
  [AX_CXXFLAGS_GCC_OPTION([-DENABLE_LOGGING_TRACE])],
  [echo enable-logging-trace=no])



   AC_ARG_ENABLE([logging-hmevent],
  [AS_HELP_STRING([--enable-logging-hmevent],
    [enableLogging at hmevent level.])],
  [AX_CXXFLAGS_GCC_OPTION([-DENABLE_LOGGING_HMEVENT])],
  [echo enable-logging-hmevent=no])
  
       AC_ARG_ENABLE([logging-app],
  [AS_HELP_STRING([--enable-logging-app],
    [enableLogging at app level.])],
  [AX_CXXFLAGS_GCC_OPTION([-DENABLE_LOGGING_APP])],
  [echo enable-logging-app=no])

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi

AM_CONDITIONAL([HAVE_DOXYGEN],[test -n "$DOXYGEN"])

dnl AM_COND_IF([HAVE_DOXYGEN],[AC_CONFIG_FILES([Doxyfile DoxyfileUser docs/Makefile])])

AC_SUBST(src_sub_dir)

dnl Checks for programs.
AC_SEARCH_LIBS([strerror], [cposix])

AX_CXXFLAGS_GCC_OPTION([-Wall])

AC_CHECK_LIB([rt], [nanosleep])

VERSION=$GENERIC_VERSION
DR_VERSION=1.0.0

AM_INIT_AUTOMAKE($PACKAGE, $VERSION, no-define)
AC_CHECK_FUNC(pthread_condattr_setclock, have_pthread_condattr_setclock=yes, have_pthread_condattr_setclock=no)

if test x$have_pthread_condattr_setclock = xyes; then

   AC_DEFINE_UNQUOTED(HAVE_PTHREAD_CONDATTR_SETCLOCK, , Define wether pthread_condattr_setclock is available)

fi




dnl -----------------------------------------------
dnl Checks for programs.
dnl -----------------------------------------------

#AC_PROG_CC
AC_PROG_CXX
AM_PROG_LIBTOOL
AC_PROG_INSTALL

AM_SANITY_CHECK

AC_LANG_CPLUSPLUS




dnl -----------------------------------------------
dnl Generates Makefile's, configuration files and scripts
dnl -----------------------------------------------


AC_SUBST(ACM_CORBA_INCLUDE_FILES)

AC_OUTPUT(Makefile \
	  include/ACM/USER/ACM_USER_INCLUDE.h \
      src/Makefile \
	  ModuleManager/Makefile \
	 NetworkPartition/Makefile \
	  test/Makefile \
	  test/FunctionalityTest/Makefile \
	  test/InterfaceReturnCodeTests/Makefile \
	  StateMachine/Makefile \
	Doxyfile DoxyfileUser docs/Makefile \
	  ACM-1.0.pc
)

sed s/': distdir'/': distdir \n\t rm -rf `find $(distdir) -name .*svn` '/g Makefile  -i

