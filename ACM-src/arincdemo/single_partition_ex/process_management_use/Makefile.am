bin_PROGRAMS = testProcessServices
testProcessServices_SOURCES= testProcessServices.cc
INCLUDES = $(DEPS_CFLAGS)
LDADD = $(DEPS_LIBS)

run: all
	ModuleManager Configuration.cfg

realclean: clean
	rm -f *.log

