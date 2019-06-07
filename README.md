[![Build Status](https://travis-ci.org/adubey14/arinc653emulator.svg?branch=master)](https://travis-ci.org/adubey14/arinc653emulator)

https://wiki.isis.vanderbilt.edu/mbshm/index.php/ACMTOOLSUITE
The code base now uses cmake
you need to set the ACM_LOG_ENTIES environment variable to see the log

# arinc653emulator
This code base contains a linux emulator for the ARINC-653 operating system services

    
# cmake build instructions
-----------------------------
- install cmake > 2.8.7
- cd to F6ACM
- to set up the environment type command
 source bootstrap.sh
- to generate make files type command
cmake .
- to build type
make -jN

- to view options (does not print options of the external cmake projects)
cmake -L

- Since the above command does not print the options of external projects (with cmake build system),
- cd to the appropriate external project folder and type 
cmake -L
