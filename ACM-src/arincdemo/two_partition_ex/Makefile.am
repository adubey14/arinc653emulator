#Build in these directories:

SUBDIRS = queueing_port_use sampling_port_use
#Distribute these directories:
DIST_SUBDIRS =  queueing_port_use sampling_port_use

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




