bin_PROGRAMS = testTimer testSemaphore testSchedulerServices testBlackBoard testReplenish testSuspendResume testQ   testBuffer testBuffer2 testHM testRWLOCK testLoad testEventPort_BlackBoard test_sampling_interModule_src test_sampling_interModule_dst test_queue_intermodule_src test_queue_intermodule_dst test_sampling_port_data_buffer testEventPort_buffer testEventPort_queuing_src testEventPort_queuing_dst test_sampling_port_channel  
testTimer_SOURCES=testTimer.cc
testEventPort_queuing_src_SOURCES=testEventPort_queuing_src.cc
testEventPort_queuing_dst_SOURCES=testEventPort_queuing_dst.cc

#testEventPort_Sampling_SOURCES=testEventPort_Sampling.cc
testEventPort_buffer_SOURCES=testEventPort_buffer.cc
test_sampling_port_data_buffer_SOURCES=test_sampling_port_data_buffer.cc
test_sampling_interModule_src_SOURCES=test_sampling_interModule_src.cc ../../src/UDPSocket.cpp ../../src/SAMPLING_CHANNEL.cc
test_sampling_interModule_dst_SOURCES=test_sampling_interModule_dst.cc ../../src/UDPSocket.cpp ../../src/SAMPLING_CHANNEL.cc
testSemaphore_SOURCES=testSemaphore.cc
testLoad_SOURCES=testLoadSystem.cc
testRWLOCK_SOURCES=testRWLOCK.cc
testHM_SOURCES=testHM.cc
testBuffer_SOURCES=testBuffer.cc
testBuffer2_SOURCES=testBuffer2.cc

testQ_SOURCES=testQ.cc ../../src/QUEUE_CHANNEL.cc ../../src/SAMPLING_CHANNEL.cc ../../src/UDPSocket.cpp
test_queue_intermodule_src_SOURCES=test_queue_intermodule_src.cc ../../src/QUEUE_CHANNEL.cc ../../src/SAMPLING_CHANNEL.cc ../../src/UDPSocket.cpp
test_queue_intermodule_dst_SOURCES=test_queue_intermodule_dst.cc ../../src/QUEUE_CHANNEL.cc ../../src/SAMPLING_CHANNEL.cc ../../src/UDPSocket.cpp
testSchedulerServices_SOURCES = testSchedulerServices.cc
testBlackBoard_SOURCES= testBlackBoard.cc
testReplenish_SOURCES = testReplenish.cc
testSuspendResume_SOURCES = testSuspendResume.cc
 

test_sampling_port_channel_SOURCES=test_sampling_port_channel.cc ../../src/UDPSocket.cpp ../../src/SAMPLING_CHANNEL.cc
testEventPort_BlackBoard_SOURCES = testEventPort_BlackBoard.cc
INCLUDES= -I$(top_srcdir)/include -I$(top_srcdir)/3rdParty/mico-source/include/               
LDADD = -L$(top_srcdir)/src/.libs/   -lACM-1.0 -L$(top_srcdir)/StateMachine/.libs/ -lStateMachine-1.0


