[![Build Status](https://travis-ci.org/adubey14/arinc653emulator.svg?branch=master)](https://travis-ci.org/adubey14/arinc653emulator)


README - F6ACM Runtime
----------------------
This archive contains the sources and binaries of the F6ACM runtime. Review https://wiki.isis.vanderbilt.edu/mbshm/index.php/ACMTOOLSUITE

for more inforamtion. 


RELEASE_NOTES
------------------
Date: 5/16/2012
- ACM-src 1.0.3.1
- The code base now uses cmake
- you need to set the ACM_LOG_ENTIES environment variable to see the log

The support for DDS and CIAO has been removed from this version.

CONTENTS AND HOW TO BUILD
---------------------------

0) $TOP_FOLDER
-------------
$TOP_FOLDER = folder where the repository was extracted

1) Setting the environment variables
---------------------------------
- Open a Terminal
- cd to the folder where the archive was extracted.
- run "source bootstrap.sh"

2) generate makefiles
---------------------------
- Open a terminal
- Follow the instructions above to set the environment variables
- run cmake .


3)Building the binaries
---------------------------
- Open a Terminal
- Follow the instructions above to set the environment variables
- Make sure the makefiles had been generated
- run make 

- This ensures that the new libraries have overwritten the pre-packaged libraries in $TOP/FOLDER/ACM-I

- to view options (does not print options of the external cmake projects)

cmake -L

- Since the above command does not print the options of external projects (with cmake build system),
- cd to the appropriate external project folder and type 

cmake -L


4) Build Samples
-------------------------------
- Open a Terminal and go to the arincdemo folder
- Follow the instructions above to set the environment variables
-run the following comands to build the sample (in the ACM-I/arincdemo folder)
$./autogen.sh
$make

5) Running Sample
--------------
Each sample folder has a readme at the top level that describes how to run the sample.

Note that one of the major changes in this version is in the ACM Logging infrastructure. 
Now, in order to generate the log files, export the variable ACM_LOG_ENTRIES to the number of log entries (counted tail-wise) that you want to generate
from the experiment. This can be done by

export ACM_LOG_ENTRIES=100


    
