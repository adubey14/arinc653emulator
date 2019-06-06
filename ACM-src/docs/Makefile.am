include_dir=../include
h_sources = $(include_dir)/$(GENERIC_LIBRARY_NAME)/INTERNAL/PROCESS.h \
	$(include_dir)/$(GENERIC_LIBRARY_NAME)/INTERNAL/ACM_THREAD_SUPPORT.h \
	$(include_dir)/$(GENERIC_LIBRARY_NAME)/INTERNAL/APEX.h  \
	$(include_dir)/$(GENERIC_LIBRARY_NAME)/USER/ACM_COMPONENT.h  \
	$(include_dir)/$(GENERIC_LIBRARY_NAME)/USER/ACM_LOGGER.h \
	$(include_dir)/$(GENERIC_LIBRARY_NAME)/USER/ACM_UTILITIES.h \
	$(include_dir)/$(GENERIC_LIBRARY_NAME)/USER/ACM_EXCEPTION.h \
	$(include_dir)/$(GENERIC_LIBRARY_NAME)/USER/APEX_INTERFACE.h \
	$(include_dir)/$(GENERIC_LIBRARY_NAME)/INTERNAL/ACM_INTERNAL_INCLUDE.h  \
	$(include_dir)/$(GENERIC_LIBRARY_NAME)/USER/APEX_TYPES.h \
	$(include_dir)/$(GENERIC_LIBRARY_NAME)/INTERNAL/QUEUE_PORT.h \
	$(include_dir)/$(GENERIC_LIBRARY_NAME)/INTERNAL/SAMPLING_PORT.h

all: html/index.html userhtml/index.html

html/index.html: $(h_sources)
	cd ..;doxygen Doxyfile;cd ..;
userhtml/index.html: $(h_sources)
	cd ..;doxygen DoxyfileUser;cd ..;	

clean:
	rm -rf html
	rm -rf userhtml
	rm -rf latex
	rm -rf UserLatex

install-data-local: all
	    $(mkinstalldirs) $(prefix)
	    $(mkinstalldirs) $(prefix)/doc		
		$(mkinstalldirs) -m 755 $(prefix)/doc/userhtml
		cp -r userhtml/* $(prefix)/doc/userhtml/.
		$(INSTALL_DATA) ModuleManager.pdf $(prefix)/doc/.
		$(INSTALL_DATA) ArincEmulatorServices.pdf $(prefix)/doc/.


EXTRA_DIST = ComponentExample.png ComponentExamplesmall.png ImplementationBlockDiagram.jpg ImplementationBlockDiagramlarge.jpg mainpage.h ModuleManager.pdf ArincEmulatorServices.pdf
	
