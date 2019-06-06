/** @mainpage ARINC COMPONENT MODEL RUN TIME FRAMEWORK (1.0.0)
*
* @authors ISIS MBSHM TEAM (https://wiki.isis.vanderbilt.edu/mbshm/index.php/Main_Page) 
*
* <b>Acknowledgements</b>: This work was supported by NASA under award NNX08AY49A. 

* @todo  
* We currently do not support checking and restricting stack size of a process and partition. The values specified in the create_partition and create_process will be ignored. This feature may be supported in future.
* @section intro Introduction
* This framework provides an ARINC Emulator designed to run on Linux Platform.
* It also provides a top layer that support ARINC Component model. All header files to be accessible by user are installed in the include folder. Source files are Source files are located in the src directory. Once installed, all libraries are copied to the <install_location>/lib folder. The list of currently supported APEX Services is available in APEX_INTERFACE.h. All ARINC datatypes are defined in APEX_TYPES.h. We have also included certain helper fucntions that extend the typical ARINC services by wrapping them into one convenient call. Take a look at ACM_APEX_EXTENSIONS.h.
*
* The runtime comes with a logger. See ACM_LOGGER.h for more details.  The example files mentioned below show how to use the logging service.
* 
* We have included the following examples for demonstrating the use of arinc emulation library
* - single partition examples
* -# \subpage testBlackboard
* -# \subpage testBuffer
* -# \subpage testEvent
* -# \subpage testProcessServices
* -# \subpage testSemaphoreServices
* - two partition examples
* -# \subpage queueing_port_use
* -# \subpage sampling_port_use
*
* <b> Component Framework </b>
*  \subpage compframework describes the component model and also describes the included example.
* <hr>
* @section requirements Requirements
This library has been extensively tested on 32 bit linux kernels > 2.6.28 and g++ (Ubuntu 4.4.1-4ubuntu9) 4.4.1. It has also been 
tested on 2.6.17.9 and 2.6.9 32 bit kernels. If you can choose the configuration of your machine choose kernel >2.6.28 and g++ >4.4. This will ensure that
your kernel supports high resolution timers. The library will work without those timers. But, the periodic processes will show more jitter without 
high resolution timers.  Moreover, following development tools are required on the linux machine.
* -# ltmain.sh (GNU libtool) > 1.5.6
* -# autoconf (GNU Autoconf) > 2.59
* -# g++ (GCC) > 3.4.6
* -# automake (GNU automake) > 1.9.2
* -# doxygen
* -# pkg-config
* -# You must have root privileges on the linux machine to run any demo or code that you develop using this library. This is because we use the SCHED_FIFO realtime schedule and this fails if you do not have the root privilege. Also, we set the affinity of each module and its partition to a specific core. This is to ensure that we can provide  <b>Temporal separation<b> between the partitions.
* <hr> 
* @section build Build Instructions

 -# Run ./configure --help to see a list of options
	Some important Options
	 -# Use --prefix=install_directory to configure your project (default install directory is /usr/local/)
	 -# Use --enable-logging-[level] to enable logging (default is no logging). Do not use two levels at the same time. 
	 -# Use --enable-corba to enable the library to build the arinc component layer. By default, the library only provides the 
	  emulated Arinc runtime enviornment. You will have to build the library with this option in order to use the accompanying model-based tools (windows environment).
 -# Use make from  to make the library. Hint: to speed up the process you can use make - j [number of parallel jobs].
  Documentation will be generated in the docs/html folder.
 -# Use make clean to clean up compiled files.
 -# Use make distclean to clean up the make environment. Remember, to run autogen again to set the environment.
 -# Use make dist to prepare a distribution package containing all sources.
 -# Use 'make install' to install in the prefix directory configured above. This will install the library, header files, demos and the documentation.


 @page testBlackboard testBlackboard.cc
 \include testBlackboard.cc
<b>Module Configuration File</b>
\include blackboard_use/Configuration.cfg
*/
/** @page testBuffer testBuffer.cc

\include testBuffer.cc
<b>Module Configuration File</b>
Note the <b>MAXITERATIONS</b> parameter and <b>CPU</b> parameter are optional. If you do not provide <b>MAXITERATIONS</b> the module will keep on executing forever.
<b>CPU</b> parameter is used to choose the CPU core on which module will execute. If you do not provide this value, the default value is core 0.
\include buffer_use/Configuration.cfg
*/
/** @page testEvent testEvent.cc
\include testEvent.cc
<b>Module Configuration File</b>
Note the <b>MAXITERATIONS</b> parameter and <b>CPU</b> parameter are optional. If you do not provide <b>MAXITERATIONS</b> the module will keep on executing forever.
<b>CPU</b> parameter is used to choose the CPU core on which module will execute. If you do not provide this value, the default value is core 0.
\include event_use/Configuration.cfg
*/
/** @page testProcessServices testProcessServices.cc
\include testProcessServices.cc
<b>Module Configuration File</b>
Note the <b>MAXITERATIONS</b> parameter and <b>CPU</b> parameter are optional. If you do not provide <b>MAXITERATIONS</b> the module will keep on executing forever.
<b>CPU</b> parameter is used to choose the CPU core on which module will execute. If you do not provide this value, the default value is core 0.
\include process_management_use/Configuration.cfg
*/
/** @page testSemaphoreServices testSemaphoreServices.cc
\include testSemaphoreServices.cc
<b>Module Configuration File</b>
Note the <b>MAXITERATIONS</b> parameter and <b>CPU</b> parameter are optional. If you do not provide <b>MAXITERATIONS</b> the module will keep on executing forever.
<b>CPU</b> parameter is used to choose the CPU core on which module will execute. If you do not provide this value, the default value is core 0.
\include sempahore_use/Configuration.cfg
*/
/** @page queueing_port_use queueing_port_use
Partition1.cc
\include queueing_port_use/partition1.cc
Partition2.cc
\include queueing_port_use/partition2.cc
<b>Module Configuration File</b>
Note the <b>MAXITERATIONS</b> parameter and <b>CPU</b> parameter are optional. If you do not provide <b>MAXITERATIONS</b> the module will keep on executing forever.
<b>CPU</b> parameter is used to choose the CPU core on which module will execute. If you do not provide this value, the default value is core 0. Note the extension of this file is not significant.
\include queueing_port_use/Configuration.txt
*/
/** @page sampling_port_use sampling_port_use
Partition1.cc
\include sampling_port_use/partition1.cc
Partition2.cc
\include sampling_port_use/partition2.cc
<b>Module Configuration File</b>
Note the <b>MAXITERATIONS</b> parameter and <b>CPU</b> parameter are optional. If you do not provide <b>MAXITERATIONS</b> the module will keep on executing forever.
<b>CPU</b> parameter is used to choose the CPU core on which module will execute. If you do not provide this value, the default value is core 0. Note the extension of this file is not significant.
\include sampling_port_use/Configuration.txt
*/
/** @page compframework Component Framework Description
*  @section framework Component Framework Description

*\image html ImplementationBlockDiagram.jpg
\image latex ImplementationBlockDiagramlarge.jpg

Figure shown above describes the  layers of our framework that implements the ARINC Component Model (ACM).  The main purpose of this framework is to provide support for developing and experimenting with component-based systems using ARINC-653 abstractions on top of Linux. The secondary goal is to design the top layers: component and processes such that they can be easily rebuilt over an actual ARINC-653 kernel. The first two layers are a physical communication network and the physical computing platform. We have selected Linux as the operating system because it is widely available, supports a real-time scheduling policy SCHED_FIFO,  and provides an implementation of the POSIX thread library. memory partitioning between Linux processes provided by the Linux Kernel is used to implement the spatial partitioning between ARINC-653 partitions. Other layers from bottom to top are as follows.


<b>APEX Services Emulation Library</b> is the next layer.  This library provides implementation of ARINC-653 interface specifications for intra-partition process communication that includes Blackboards and Buffers. Buffers provide a queue for passing messages and Blackboards enable processes to read, write, and clear single message. Intra-partition process synchronization is supported through Semaphores and Events. We have also implemented process and time management services as described in the ARINC-653 specification. Inter-partition communication is provided by sampling ports and queuing ports. We can also provide inter-partition communication using the event channels and remote procedure calls supported by our ORB layer, which will be described later in this section. Overall, this layer was implemented in approximately 15,000 lines of C++ code.  Recall that, we implement ARINC-653 processes as POSIX threads. ARINC-653 processes, just like POSIX threads share the address space. Processes, both periodic and aperiodic, can only be created at initialization, following the ARINC-653 specification. Specified process properties include the expected worst case execution time, which cannot be changed at run-time.  We have designed this layer such that it can be replaced by a real APEX kernel without affecting the layers on the top.

 
<b>APEX Module Manager</b> is the next layer. It is responsible for providing temporal partitioning among partitions (i.e., Linux processes). Each partition inside a module is configured with an associated period that identifies the rate of execution. The partition properties also include the time duration of execution. The module manager is configured with a fixed cyclic schedule with pre-determined hyperperiod. The schedule is computed from the specified partition periods and durations. The module configuration also specifies the hyperperiod value, which is the least common multiple of all partition periods, the partition names, the partition executables, and their scheduling windows. Note that the module manager allows execution of one and only one partition inside a given scheduling window, which is specified with the offset from the start of the hyperperiod and a duration. The module manager is responsible for checking that the schedule is valid before the system can be initialized i.e. all scheduling windows within a hyperperiod can be executed without overlap. The included examples provide sample configuration files.


<b>APEX Partition Scheduler</b>, the next layer, is instantiated using the APEX services emulation library for each partition. It implements a priority-based preemptive scheduling algorithm. This scheduler initializes and schedules the (ARINC-653) processes inside the partition based on their periodicity and priority. It ensures that all processes finish their execution within the specified deadline. Upon \textit{deadline violation}, the faulty process is prevented from further execution, which is the specified default action. It is possible to change this action to allow a restart.
 
 <b>Object Request Broker (ORB)</b> is the next layer.  This framework uses an open source CCM implementation, called MICO (http://www.fpx.de/MicoCCM/) .   The main ORB thread is executed as an aperiodic ARINC-653 process within the respective partition. For controllability, the ORB runs at a lower priority than the partition scheduler. Since ARINC does not allow dynamic creation of processes at run-time, the ORB is configured to use a predefined number of worker threads (i.e. ARINC-653 Processes) that are created during initialization.

<b>Component and Process Layers</b> include the glue code (generated from the definitions of components their interfaces provided in an IDL file) and the user-provided implementation code. <VAR>WE advise you to use the associated windows-based modeling tools if you are using the component layer </VAR>.  The developer is responsible for specifying the necessary process properties such as periodicity, priority, stack size, and deadline in the models. The code-generator included in the associated windows installer provides glue code that maps each component interface method to an ARINC-653 process.
 
 
 For example, consider a GPS Assembly which contains three components - Sensor, GPS, NavDisplay. See the figure. The Sensor component  contains an asynchronous publisher interface (source port) that is triggered periodically (every 4 sec). The event published by this interface is consumed by a periodically triggered asynchronous consumer/event sink port on the GPS component (every 4 sec). Note that the event sink process is periodically released, and each such invocation reads the last event published by the Sensor. If the Sensor does not update the event frequently enough, the GPS may read stale data. The consumer process in the GPS, in turn, produces an event that is published through the GPS's event publisher port. This event triggers the aperiodic consumer / event sink port on the Navigation Display component. Upon activation, the display component uses an interface provided by the GPS to retrieve the position data via a synchronous method invocation call into the GPS component. Sensor Component publishes time-stamped data periodically.

* \image  html ComponentExamplesmall.png
* \image latex ComponentExample.png

 
 
<b>Component Level Health Managers</b>  react to detected events and takes mitigation actions. It can also report events to higher-level manager (defined at the partition level or the module manager level). The code-generator provides the glue code to create these health-manager processes for the components. They are instantiated as aperiodic processes that run at the highest priority in the partition. The code-generator also provides the glue code for all processes belonging to a component to register with the respective health manager. Upon detection of an error, a component method (process) can use an API to inform the respective health manager that can then take the necessary mitigation section. 

* @section ACMDemo Examples for Demonstrating the Use of Component Framework
* For component examples refer to the documentation provided with the ACM modeling tools available at https://wiki.isis.vanderbilt.edu/mbshm/index.php/ACMTOOLSUITE.

 */
*/