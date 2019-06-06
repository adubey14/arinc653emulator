## top-level Makefile.am
#Build in these directories:
SUBDIRS = single_partition_ex two_partition_ex three_partition_ex
#Distribute these directories:
DIST_SUBDIRS =  single_partition_ex two_partition_ex three_partition_ex

EXTRA_DIST=autogen.sh


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