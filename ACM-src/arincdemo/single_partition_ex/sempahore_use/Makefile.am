bin_PROGRAMS = testSemaphoreServices
testSemaphoreServices_SOURCES= testSemaphoreServices.cc
INCLUDES = $(DEPS_CFLAGS)
LDADD = $(DEPS_LIBS)

run: all
	ModuleManager Configuration.cfg

realclean: clean
	rm -f *.log

