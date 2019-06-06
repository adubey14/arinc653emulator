dnl --------------------------------

dnl Initialization macros.

dnl --------------------------------



AC_INIT(single_partition_ex/Makefile.am)
AM_CONFIG_HEADER(config.h)


dnl -----------------------------------------------

dnl Package name and version number (user defined)

dnl -----------------------------------------------



AM_INIT_AUTOMAKE(ACM_demo, 0.3.0)





dnl -----------------------------------------------

dnl Checks for programs.

dnl -----------------------------------------------



AC_PROG_CC

AC_PROG_CXX

AM_PROG_LIBTOOL

AM_SANITY_CHECK



dnl -----------------------------------------------

dnl Checks for libraries.

dnl -----------------------------------------------



# Paths to examplelib stuff:

AC_LANG_CPLUSPLUS



PKG_CHECK_MODULES(DEPS, ACM-1.0 >= 0.0.0)

AC_SUBST(DEPS_CFLAGS)

AC_SUBST(DEPS_LIBS)




dnl -----------------------------------------------

dnl Generates Makefiles.

dnl -----------------------------------------------


AC_OUTPUT(Makefile \
	  single_partition_ex/Makefile \
	  single_partition_ex/blackboard_use/Makefile \
	  single_partition_ex/buffer_use/Makefile \
	  single_partition_ex/event_use/Makefile \
	  single_partition_ex/process_management_use/Makefile \
	  single_partition_ex/periodic_wait_use/Makefile \
	  single_partition_ex/sempahore_use/Makefile \
	  two_partition_ex/Makefile \
	  two_partition_ex/sampling_port_use/Makefile \
	  two_partition_ex/queueing_port_use/Makefile \
	  three_partition_ex/Makefile \
	  three_partition_ex/sampling_port_use/Makefile \
	  three_partition_ex/queueing_port_use/Makefile

)
