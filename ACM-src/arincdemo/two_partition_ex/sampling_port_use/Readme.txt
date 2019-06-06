This example describes the use of a sampling  port to send data from partition1 to partition2.
Partition1 has a periodic process that sends Payload (defined in common.h) to the port every 4 seconds.
Partition2 has another periodic process that reads data from the port every 4 seconds
Note the use of channel specification in the configuration file.