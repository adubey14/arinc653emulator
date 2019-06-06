#Build in these directories:

SUBDIRS = FunctionalityTest InterfaceReturnCodeTests
#Distribute these directories:
DIST_SUBDIRS =  FunctionalityTest InterfaceReturnCodeTests

run:
	for d in $(SUBDIRS); do \
	pushd $$d; \
	make run; \
	popd; \
	done;
realclean:
	for d in $(SUBDIRS); do \
	pushd $$d; \
	make realclean; \
	popd; \
	done;




