This example describes the use of a queueing  port to send data from partition1 to partition2.
Partition1 has a periodic process that sends Payload (defined in common.h) to the port every 4 seconds.
Partition2 has an aperiodic process that waits for the data to arrive and consumes it.
Note the use of channel specification in the configuration file.