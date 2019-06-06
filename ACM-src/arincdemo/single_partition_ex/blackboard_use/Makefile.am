bin_PROGRAMS = testBlackboard
testBlackboard_SOURCES= testBlackboard.cc
INCLUDES = $(DEPS_CFLAGS) -I$(top_srcdir)
LDADD = $(DEPS_LIBS)

run: all
	ModuleManager Configuration.cfg

realclean: clean
	rm -f *.log

