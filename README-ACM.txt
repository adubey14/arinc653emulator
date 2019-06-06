README - F6ACM Runtime
----------------------
This archive contains the sources and binaries of the F6ACM runtime.


RELEASE_NOTES
------------------
Date: 5/16/2012
- Added support for DDS 3.1
- CIAO version 1.0.3
- ACM-src 1.0.3.1
- Added support for Diffserv

CONTENTS AND HOW TO BUILD
---------------------------

0) $TOP_FOLDER
-------------
$TOP_FOLDER = folder where the archive was extracted

1) Setting the environment variables
---------------------------------
- Open a Terminal
- cd to the folder where the archive was extracted.
- run "source bootstrap.sh"

2) generate makefiles
---------------------------
- Open a terminal
- Follow the instructions above to set the environment variables
- run generateMakefiles.sh

3)Building the binaries
---------------------------
- Open a Terminal
- Follow the instructions above to set the environment variables
- Make sure the makefiles had been generated
- run ./build.sh (provide -jn for n concurrent jobs, e.g. -j4)

- This ensures that the new libraries have overwritten the pre-packaged libraries in $TOP/FOLDER/ACM-I

4) Build Samples
-------------------------------
- Open a Terminal
- Follow the instructions above to set the environment variables
-cd to gen_deployments/$DEP_FOLDER ($DEP_FOLDER varies for each sample based on the Deployment folder name in the GME-Model)
-run the following comands to build the sample
$source prepareExperiments.sh
$make distclean"
$./autogen.sh
$make

5) Running Sample
--------------
Each sample folder has a readme at the top level that describes how to run the sample.

Note that one of the major changes in this version is in the ACM Logging infrastructure. 
Now, in order to generate the log files, export the variable ACM_LOG_ENTRIES to the number of log entries (counted tail-wise) that you want to generate
from the experiment. This can be done by

export ACM_LOG_ENTRIES=100

  
