bin_PROGRAMS = getStatusTest resumeTest createProcessTest delayedStartTest getMyIDTest setPriorityTest stopTest suspendSelfTest suspendTest startTest getIDTest stopSelfTest createEventTest getEventIDTest getEventStatusTest resetEventTest setEventTest waitEventTest createSemaphoreTest waitSemaphoreTest signalSemaphoreTest getSemaphoreIDTest getSemaphoreStatusTest createBlackboardTest clearBlackboardTest displayBlackboardTest getBlackboardIDTest getBlackboardStatusTest readBlackboardTest getBufferIDTest getBufferStatusTest createBufferTest sendBufferTest receiveBufferTest getPartitionStatusTest setPartitionModeTest
getStatusTest_SOURCES= getStatusTest.cc
resumeTest_SOURCES= resumeTest.cc
createProcessTest_SOURCES= createProcessTest.cc
delayedStartTest_SOURCES= delayedStartTest.cc
getMyIDTest_SOURCES= getMyIDTest.cc
getIDTest_SOURCES= getIDTest.cc
setPriorityTest_SOURCES= setPriorityTest.cc
stopTest_SOURCES= stopTest.cc
suspendSelfTest_SOURCES= suspendSelfTest.cc
suspendTest_SOURCES= suspendTest.cc
startTest_SOURCES= startTest.cc
stopSelfTest_SOURCES= stopSelfTest.cc
createEventTest_SOURCES= createEventTest.cc
getEventIDTest_SOURCES= getEventIDTest.cc
getEventStatusTest_SOURCES= getEventStatusTest.cc
resetEventTest_SOURCES= resetEventTest.cc
setEventTest_SOURCES= setEventTest.cc
waitEventTest_SOURCES= waitEventTest.cc
createSemaphoreTest_SOURCES= createSemaphoreTest.cc
waitSemaphoreTest_SOURCES= waitSemaphoreTest.cc
signalSemaphoreTest_SOURCES= signalSemaphoreTest.cc
getSemaphoreIDTest_SOURCES= getSemaphoreIDTest.cc
getSemaphoreStatusTest_SOURCES= getSemaphoreStatusTest.cc
createBlackboardTest_SOURCES= createBlackboardTest.cc
clearBlackboardTest_SOURCES= clearBlackboardTest.cc
displayBlackboardTest_SOURCES= displayBlackboardTest.cc
getBlackboardIDTest_SOURCES= getBlackboardIDTest.cc
getBlackboardStatusTest_SOURCES= getBlackboardStatusTest.cc
readBlackboardTest_SOURCES= readBlackboardTest.cc
getBufferIDTest_SOURCES= getBufferIDTest.cc
getBufferStatusTest_SOURCES= getBufferStatusTest.cc
createBufferTest_SOURCES= createBufferTest.cc
sendBufferTest_SOURCES= sendBufferTest.cc
receiveBufferTest_SOURCES= receiveBufferTest.cc
getPartitionStatusTest_SOURCES= getPartitionStatusTest.cc
setPartitionModeTest_SOURCES= setPartitionModeTest.cc

INCLUDES= -I$(top_srcdir)/include -I$(top_srcdir)/3rdParty/mico-source/include/               
LDADD =  -L$(top_srcdir)/src/.libs/   -lACM-1.0 -L$(top_srcdir)/StateMachine/.libs/ -lStateMachine-1.0

realclean: clean
	rm -f *.log

