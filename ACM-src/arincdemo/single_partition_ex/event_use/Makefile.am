bin_PROGRAMS = testEvent
testEvent_SOURCES= testEvent.cc
INCLUDES = $(DEPS_CFLAGS)
LDADD = $(DEPS_LIBS)

run: all
	ModuleManager Configuration.cfg

realclean: clean
	rm -f *.log

