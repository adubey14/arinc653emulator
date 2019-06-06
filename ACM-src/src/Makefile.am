lib_LTLIBRARIES= libACM-1.0.la
SUBDIRS =  $(src_sub_dir) .


include_dir=$(top_srcdir)/include
package_include_sub_dir= ACM
if generateciaotypes
BUILT_SOURCES = $(include_dir)/ACM/USER/ACM_IDL_TYPES.h $(include_dir)/ACM/USER/ACM_IDL_TYPES.cc  $(include_dir)/ACM/USER/ACM_IDL_TYPESC.h $(include_dir)/ACM/USER/ACM_IDL_TYPESC.cpp
cc_sources =APEX.cc APEX_BLACKBOARD_SERVICES.cc APEX_BUFFER_SERVICES.cc APEX_EVENT_SERVICES.cc APEX_HM_SERVICES.cc APEX_INTERFACE_IMPL.cc \
                APEX_PROCESS_SERVICES.cc APEX_QUEUING_SERVICES.cc APEX_SAMPLING_SERVICES.cc APEX_SCHEDULING.cc APEX_SEMAPHORE_SERVICES.cc \
                APEX_TIME_MANAGEMENT_SERVICES.cc BLACKBOARD.cc BUFFER.cc EVENT.cc ACM_LOGGER.cc QUEUE_PORT.cc \
                SAMPLING_PORT.cc ACM_THREAD_SUPPORT.cc PROCESS.cc EVENT_PORT.cc UDPSocket.cpp $(include_dir)/ACM/USER/ACM_IDL_TYPES.h $(include_dir)/ACM/USER/ACM_IDL_TYPES.cc

$(include_dir)/ACM/USER/ACM_IDL_TYPES.cc $(include_dir)/ACM/USER/ACM_IDL_TYPES.h $(include_dir)/ACM/USER/ACM_IDL_TYPESC.h $(include_dir)/ACM/USER/ACM_IDL_TYPESC.cpp : $(include_dir)/ACM/USER/ACM_IDL_TYPES.idl
	pushd $(include_dir)/ACM/USER/; \
        PATH=$$PATH:$(ACE_ROOT)/bin;LD_LIBRARY_PATH=$(ACE_ROOT)/lib:$$LD_LIBRARY_PATH;tao_idl -aw -SS ACM_IDL_TYPES.idl; \
	echo "#include <ACM/USER/ACM_IDL_TYPESC.cpp>" > ACM_IDL_TYPES.cc; \
	echo "#include <ACM/USER/ACM_IDL_TYPESC.h>" > ACM_IDL_TYPES.h; \
        popd

INCLUDES = -I$(top_srcdir) -I$(include_dir) -I$(ACE_ROOT) -I$(TAO_ROOT)
realclean: clean
	$(RM) $(BUILT_SOURCES)
else
INCLUDES = -I$(top_srcdir) -I$(include_dir) 
cc_sources =APEX.cc APEX_BLACKBOARD_SERVICES.cc APEX_BUFFER_SERVICES.cc APEX_EVENT_SERVICES.cc APEX_HM_SERVICES.cc APEX_INTERFACE_IMPL.cc \
		APEX_PROCESS_SERVICES.cc APEX_QUEUING_SERVICES.cc APEX_SAMPLING_SERVICES.cc APEX_SCHEDULING.cc APEX_SEMAPHORE_SERVICES.cc \
		APEX_TIME_MANAGEMENT_SERVICES.cc BLACKBOARD.cc EVENT.cc ACM_LOGGER.cc BUFFER.cc QUEUE_PORT.cc \
		SAMPLING_PORT.cc ACM_THREAD_SUPPORT.cc PROCESS.cc EVENT_PORT.cc UDPSocket.cpp  
endif

library_includedir =$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)
library_include_HEADERS = $(h_sources)


libACM_1_0_la_SOURCES= $(h_sources) $(cc_sources)
libACM_1_0_la_LIBADD = $(src_lib_add)

libACM_1_0_la_LDFLAGS=  @corba_libraries@ -lpthread -lrt -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE) 

install-data-local: 
		$(mkinstalldirs) $(prefix)
		$(mkinstalldirs) $(library_includedir)
		pushd $(include_dir)/$(package_include_sub_dir)/INTERNAL;\
		for d in `find . -type d |grep -v '.*svn'`; do \
		$(mkinstalldirs) $(library_includedir)/INTERNAL/$$d; \
			for file in $$d/*.h; do \
			$(INSTALL_DATA) $$file $(library_includedir)/INTERNAL/$$d/.; \
			done; \
			for file in $$d/*.idl; do \
			$(INSTALL_DATA) $$file $(library_includedir)/INTERNAL/$$d/.; \
			done; \
		done; \
		popd; 
		pushd $(include_dir)/$(package_include_sub_dir)/USER;\
		for d in `find . -type d |grep -v '.*svn'`; do \
		$(mkinstalldirs) $(library_includedir)/USER/$$d; \
			for file in $$d/*.h; do \
			$(INSTALL_DATA) $$file $(library_includedir)/USER/$$d/.; \
			done; \
			for file in $$d/*.idl; do \
			$(INSTALL_DATA) $$file $(library_includedir)/USER/$$d/.; \
			done; \
		done; \
		popd; 

