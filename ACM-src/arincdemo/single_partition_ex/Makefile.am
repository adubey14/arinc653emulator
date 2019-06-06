#Build in these directories:

SUBDIRS = blackboard_use buffer_use event_use process_management_use sempahore_use periodic_wait_use
#Distribute these directories:
DIST_SUBDIRS =  blackboard_use buffer_use event_use process_management_use sempahore_use

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




