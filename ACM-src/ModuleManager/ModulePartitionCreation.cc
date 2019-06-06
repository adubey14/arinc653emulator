namespace ACM_INTERNAL{

	//create a partitions with the given parameters
	int MODULE_MANAGER::LOAD_AND_INITIALIZE_PARTITION_EXECUTABLE(unsigned int index){	
		LOGGER(INFO,"Entered Load Executable");

		if(index > PARTITION_VECTOR.size())
		{
			LOGGER(INFO,"LOAD_AND_INITIALIZE_PARTITION_EXECUTABLE index %d > PARTITION_VECTOR.size() %d ",index,PARTITION_VECTOR.size());
			return -1;
		}



		ACM_INTERNAL::AUTO_LOCK mylock (this->my_mutex);

		PARTITION& partitionToCreate=PARTITION_VECTOR[index];
		LOGGER(TRACE,"CREATE_PARTITION: %s ", partitionToCreate.Name.c_str() );
		pid_t newPartitionPID = fork();

		//check if fork succeeded
		if ( newPartitionPID == -1 )
		{
			LOGGER(INFO,"Process creation failed (fork) for process %s", partitionToCreate.Name.c_str() );
			return -1;
		}
		LOGGER(INFO,"Process fork succeeded for process %s", partitionToCreate.Name.c_str() );

		//process creation succeeded
		if ( newPartitionPID == 0 )
		{
			struct sched_param sp_PartitionProcess;
			sp_PartitionProcess.sched_priority = CONTROLLER_PROCESS_PRIORITY;
			sched_setparam( 0, &sp_PartitionProcess);
			//get the child process ABSOLUTE priority
			struct sched_param sp_child;
			sched_getparam( 0, &sp_child); 
			/* 11-26-11 AD: Adding to remove partitions from MM group. This will block automatic SIGINTS to be sent to partitions.
			*/
			LOGGER(INFO,"Partition %s is initialized with PID: %d and priority: %d", partitionToCreate.Name.c_str(), getpid() , sp_child.sched_priority );
			LOGGER(DEBUG, "Creating a new group session for parititon %s", partitionToCreate.Name.c_str());
			setpgrp ();
			LOGGER(DEBUG, "LAUNCHING PARTITION %s with group id %d", partitionToCreate.Name.c_str(), getpgrp ());


			//load the executable
			//int result = execv( partitionToCreate.Executable.c_str(), NULL );
			int result=0;
			if(partitionToCreate.isNetworkPartition)
			{
				result = execl( partitionToCreate.Executable.c_str(), partitionToCreate.Executable.c_str(), this->fileName.c_str(), partitionToCreate.Name.c_str(),(char *)NULL );			
			}
			else
			{
				//is a regular partition
				if(this->DISCOVERY_PROTOCOL==NAMINGSERVICE)
				{
					char namingservicebuffer[500];
					snprintf(namingservicebuffer,500,"NameService=corbaloc::%s:%d/NameService",this->NS_HOST_IP.c_str(),this->NS_PORT_NUMBER);
					result = execl( partitionToCreate.Executable.c_str(), partitionToCreate.Executable.c_str(), "-ORBInitRef", namingservicebuffer,(char *)NULL );			
				}
				else
				{
					//result = execv( partitionToCreate.Executable.c_str(), NULL );
					result = execl( partitionToCreate.Executable.c_str(), partitionToCreate.Executable.c_str(), (char* )NULL );
				}
			}
			if ( result == -1 )
			{
				//This failure happens when the user is not a root.	//The framework should log this error and EXIT.
				LOGGER(  FATAL, "Error when trying to execv( %s) : %s",partitionToCreate.Executable.c_str(), strerror( errno ));
			}

			//executable cannot be started
			LOGGER(FATAL,"Unable to launch process: %s (PID: %d), killing partition...", partitionToCreate.Executable.c_str(), getpid() );			
			//killing forked process
			DOSHUTDOWN_BEFORE_INIT();

			//throw(-1);		
		}
		else
		{
			//we are in the parent, check if the child process was successfully started			
			//store the it in the list
			PARTITION_VECTOR[ index ].PID  = newPartitionPID; 
#ifndef ISNETWORK_PARTITION
			if(killfile.good() && killfile.is_open())
			{

				killfile<<"ps -ef |grep `basename \""<<PARTITION_VECTOR[index].Executable<<"\"` | grep "<<newPartitionPID<<" && echo \"kill -9 "<<newPartitionPID<<" \" && kill -9 "<<newPartitionPID<<std::endl;


				killfile.flush();
			}
#endif

			//wait till process initializes itself
			//it will be unlocked when partition send SIGUSR1 or the partition process cannot be started


			SYSTEM_TIMESPEC now_time = currentTime();
			SYSTEM_TIME_TYPE WAIT_TIME=convertSecsToNano(MIN_WAIT_TIME);
			now_time=addTimeNano(now_time,WAIT_TIME);
			int mutexRetVal = this->my_mutex.timedlock(now_time);	
			LOGGER(INFO,"OUT of TIMED LOCK");
			if ( mutexRetVal == ETIMEDOUT )
			{
				//store the it in the list
				LOGGER(HMEVENT,"DID Not RECEIVE HANDSHAKE SIGNAL from %s",PARTITION_VECTOR[ index ].Name.c_str());
				LOGGER(HMEVENT,"STARTING SHUTDOWN PROCEDURE");
				DOSHUTDOWN_BEFORE_INIT();
				//throw(-1);

			}
		}

		return 0;

	}
	//Use this if Two Step synchronization is required.
	int MODULE_MANAGER::Synchronize(){
		LOGGER(INFO,"Entered Second Stage Of Synchronization With Partitions");	
		HELPER_CLOCK_NANOSLEEP(convertSecsToNano(1));
		for(unsigned int index =0; index<PARTITION_VECTOR.size();index++)
		{
			ACM_INTERNAL::AUTO_LOCK mylock (this->my_mutex);
			SYSTEM_TIMESPEC now_time = currentTime();
			SYSTEM_TIME_TYPE WAIT_TIME=convertSecsToNano(MIN_WAIT_TIME);
			now_time=addTimeNano(now_time,WAIT_TIME);
			SEND_SIGCONT_TO_PARTITION(PARTITION_VECTOR[ index ].PID);
			int mutexRetVal = this->my_mutex.timedlock(now_time);	
			LOGGER(INFO,"OUT of TIMED LOCK");
			if ( mutexRetVal == ETIMEDOUT )
			{
				//store the it in the list
				LOGGER(HMEVENT,"DID Not RECEIVE 2nd STAGE HANDSHAKE SIGNAL from %s",PARTITION_VECTOR[ index ].Name.c_str());
				LOGGER(HMEVENT,"STARTING SHUTDOWN PROCEDURE");
				DOSHUTDOWN_BEFORE_INIT();
				//throw(-1);

			}
		}
		return 0;
	}
};