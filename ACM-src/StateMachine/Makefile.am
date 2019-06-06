lib_LTLIBRARIES= libStateMachine-1.0.la
include_dir=$(top_srcdir)/include
package_include_sub_dir=. 

cc_sources = src/StateMachine/globals.cpp src/StateMachine/StateMachine.cpp ../src/ACM_TIMER.cc 

library_includedir =$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/
library_include_HEADERS = $(h_sources)

INCLUDES = -I $(top_srcdir) -I $(include_dir) -I $(top_srcdir)/3rdParty/mico-source/include/
libStateMachine_1_0_la_SOURCES= $(h_sources) $(cc_sources)


libStateMachine_1_0_la_LDFLAGS= -lpthread -lrt -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)                                                                                                 
libStateMachine_1_0_la_LIBADD=  $(src_lib_add) ../src/libACM-1.0.la
install-data-local: 
		$(mkinstalldirs) $(prefix)
		$(mkinstalldirs) $(library_includedir)
		$(mkinstalldirs) $(library_includedir)/StateMachine
		pushd $(include_dir)/StateMachine;\
		for d in `find . -type d |grep -v '.*svn'`; do \
		$(mkinstalldirs) $(library_includedir)/StateMachine/$$d; \
			for file in $$d/*.h; do \
			$(INSTALL_DATA) $$file $(library_includedir)/StateMachine/$$d/.; \
			done; \
		done; \
		popd; 

