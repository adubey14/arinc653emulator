bin_PROGRAMS = partition1 partition2 partition3
partition1_SOURCES= partition1.cc
partition2_SOURCES = partition2.cc
partition3_SOURCES = partition3.cc
INCLUDES = $(DEPS_CFLAGS) -I. -I$(top_srcdir)
LDADD = $(DEPS_LIBS)

run: all
	ModuleManager Configuration.txt

realclean: clean
	rm -f *.log

