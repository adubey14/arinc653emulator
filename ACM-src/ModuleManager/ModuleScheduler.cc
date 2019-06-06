namespace ACM_INTERNAL{

	void MODULE_MANAGER::SEND_SIGCONT_TO_PARTITION(pid_t partitionpid){
		kill(partitionpid,SIGCONT);
		LOGGER(INFO,"Sending SIGCONT to partition pid: %d",partitionpid);
	}
	void MODULE_MANAGER::SEND_SIGSTOP_TO_PARTITION(pid_t partitionpid){
		kill(partitionpid,SIGSTOP);
		LOGGER(INFO,"Sending SIGSTOP to partition pid: %d",partitionpid);
	}


	/*======================================	SET FIFO			==========================================================*/
	void MODULE_MANAGER::SET_FIFO_POLICY(){
		//set CPU affinity to bind this process it to processor 1
		//the setting will be inherited by fork, execv, and to threads
		//get self scheduling, and if it's not FIFO, set it to be FIFO
		int policy = -1;
		struct sched_param sp_Process;
		int retVal = pthread_getschedparam( pthread_self(), &policy, & sp_Process );
		if (retVal!= 0)
		{
			LOGGER(ERROR,"pthread_getschedparam at %d failed with %d",__LINE__,retVal); 
		}
		//when the current is not ACM_SCHED_POLICY
		if ( policy != ACM_SCHED_POLICY )
		{
			//set it to ACM_SCHED_POLICY
			struct sched_param sp_Controller;
			sp_Controller.sched_priority = PARTITION_PROCESS_HIGHPRIORITY;
			int schedRetValue = sched_setscheduler( 0, ACM_SCHED_POLICY, &sp_Controller);
			//when changing the scheduling failed
			if ( schedRetValue == -1 )
			{
				//This failure happens when the user is not a root.	//The framework should log this error and throw.
				LOGGER(  FATAL, "Error when trying to set FIFO scheduling: %s", strerror( errno ));
				throw(-1);
			}
			//after setting FIFO scheduling, get controllers ABSOLUTE priority	
			sched_getparam( 0, &sp_Controller ); 
			LOGGER(ACM_USER::DEBUG,  "Controller's ABSOLUTE priority: %d", sp_Controller.sched_priority );
			//get thread priority
			retVal=0;
			retVal = pthread_getschedparam( pthread_self(), &policy, & sp_Process );
			if (retVal!= 0)
			{
				LOGGER(ERROR,"pthread_getschedparam at %d failed with %d",__LINE__,retVal); 
			}
			if ( sp_Process.sched_priority < MODULE_MANAGER_PRIORITY)
			{
				LOGGER(ACM_USER::DEBUG,  "setting main thread priority to %d", MODULE_MANAGER_PRIORITY );
				sp_Process.sched_priority = MODULE_MANAGER_PRIORITY;
				pthread_setschedparam(pthread_self(), ACM_SCHED_POLICY, &sp_Process);
			}
		}
	}

	


	int MODULE_MANAGER::RUN_SYSTEM_MODULE_MANAGER()
	{
		long long int NumberofIterations=0;
		SYSTEM_TIMESPEC MM_START_TIME=currentTime();
		SYSTEM_TIMESPEC START_HYPER_PERIOD=MM_START_TIME;
		while ((this->isMAX_NUMBER_OF_FRAMES_SET && NumberofIterations<this->MAX_NUMBER_OF_FRAMES) || (!this->isMAX_NUMBER_OF_FRAMES_SET))
		{

			SEND_HYPER_PERIOD_ECHO();

			if (this->isMAX_NUMBER_OF_FRAMES_SET)
				NumberofIterations++;
			LOGGER(INFO,"START Hyperperiod");

			//SYSTEM_TIMESPEC NEXT_HYPER_PERIOD=addTimeNano(START_HYPER_PERIOD,this->MAJOR_FRAME_SIZE);
			for(unsigned int i =0; i<SCHEDULING_VECTOR.size();i++)
			{
				SYSTEM_TIMESPEC ABSOLUTE_OFFSET_TIME=addTimeNano(START_HYPER_PERIOD,SCHEDULING_VECTOR[i].OFFSET);
				SYSTEM_TIME_TYPE CRITICAL_INSTANCE = SCHEDULING_VECTOR[i].OFFSET+SCHEDULING_VECTOR[i].DURATION;
				SYSTEM_TIMESPEC ABSOLUTE_DURATION_TIME=addTimeNano(START_HYPER_PERIOD,CRITICAL_INSTANCE);
				int clockresult =1;
				// you can spuriously get out of nanosleep
				struct timespec rem = {0,0};
				while ( clockresult != 0) {
					clockresult = clock_nanosleep( CLOCK_REALTIME ,TIMER_ABSTIME, &ABSOLUTE_OFFSET_TIME,&rem);
				}
				RUN_PARTITION(SCHEDULING_VECTOR[i].PARTITION_INDEX,ABSOLUTE_DURATION_TIME);
			}
			LOGGER(INFO,"Waiting for next Hyperperiod");
			//SYSTEM_TIME_TYPE FRAME_OFFSET=this->MAJOR_FRAME_SIZE*(SYSTEM_TIME_TYPE)(NumberofIterations);			
			START_HYPER_PERIOD=START_HYPER_PERIOD + this->MAJOR_FRAME_SIZE;
			struct timespec rem = {0,0};
			int clockresult =1;
			while ( clockresult != 0) {
				clockresult = clock_nanosleep( CLOCK_REALTIME ,TIMER_ABSTIME, &START_HYPER_PERIOD,&rem);
			}
		}

		SEND_HYPER_PERIOD_ECHO(true);

		DOSHUTDOWN(true);
		return -1;		
	}
	int MODULE_MANAGER::RUN_CHILD_MODULE_MANAGER()
	{
		int NumberofIterations=0;

		ACM_INTERNAL::SocketSynchronizationMessageType::SERIAL_BUFFER_TYPE BUFFER;
		ACM_INTERNAL::SocketSynchronizationMessageType syncrecv;
		//Wait to Receive Start from parent

		while(true)
		{
			strcpy(syncrecv.MESSAGE_SRC_MODULE," ");
			while(strcmp(syncrecv.MESSAGE_SRC_MODULE,this->PARENT_NAME.c_str())!=0 )
			{
				unsigned int recvMsgSize=0;
				string sourceAddress;             // Address of datagram source
				unsigned short sourcePort;        // Port of datagram source
				LOGGER(INFO,"Trying to Receive START from Parent ");
				recvMsgSize = this->Recv_Socket->recvFrom(&(BUFFER[0]), SyncTypeSize, sourceAddress,	sourcePort);
				LOGGER(INFO,"Received Message Size %u",recvMsgSize);
				if (recvMsgSize!=SyncTypeSize || errno==EAGAIN || errno==EWOULDBLOCK)
				{
					LOGGER(ERROR,"Error in Receiving START from Parent");
					continue;
				}

				ACM_INTERNAL::SocketSynchronizationMessageType::FROM_SERIAL_BUFFER(syncrecv,BUFFER);
				if(strcmp(syncrecv.MESSAGE_SRC_MODULE,this->PARENT_NAME.c_str())!=0)
				{
					LOGGER(ERROR,"RECEIVED WRONG ENTRY");
					syncrecv.printout();
				}
			}

			if (syncrecv.Sync==SocketSynchronizationMessageType::START)
			{
				//do Something
				SYSTEM_TIMESPEC MM_START_TIME=currentTime();
				SYSTEM_TIMESPEC START_HYPER_PERIOD=MM_START_TIME;

				if (this->isMAX_NUMBER_OF_FRAMES_SET)
					NumberofIterations++;
				LOGGER(INFO,"START Hyperperiod");


				//SYSTEM_TIMESPEC NEXT_HYPER_PERIOD=addTimeNano(START_HYPER_PERIOD,this->MAJOR_FRAME_SIZE);
				for(unsigned int i =0; i<SCHEDULING_VECTOR.size();i++)
				{
					SYSTEM_TIMESPEC ABSOLUTE_OFFSET_TIME=addTimeNano(START_HYPER_PERIOD,SCHEDULING_VECTOR[i].OFFSET);
					SYSTEM_TIME_TYPE CRITICAL_INSTANCE = SCHEDULING_VECTOR[i].OFFSET+SCHEDULING_VECTOR[i].DURATION;
					SYSTEM_TIMESPEC ABSOLUTE_DURATION_TIME=addTimeNano(START_HYPER_PERIOD,CRITICAL_INSTANCE);
					int clockresult =1;
					// you can spuriously get out of nanosleep
					struct timespec rem = {0,0};
					while ( clockresult != 0) {
						clockresult = clock_nanosleep( CLOCK_REALTIME ,TIMER_ABSTIME, &ABSOLUTE_OFFSET_TIME,&rem);
					}

					RUN_PARTITION(SCHEDULING_VECTOR[i].PARTITION_INDEX,ABSOLUTE_DURATION_TIME);
				}
				LOGGER(INFO,"Waiting for next Hyperperiod");
				continue;
			}
			else if (syncrecv.Sync==SocketSynchronizationMessageType::STOP)
			{
				//do Something
				break;
			}
			else
			{
				LOGGER(ERROR,"RECEIVED WRONG Hyper period sync - while running system");
				syncrecv.printout();
			}
		}

		DOSHUTDOWN(true);
		return -1;
	}

	int MODULE_MANAGER::RUN_WITHOUT_SYSTEM_MANAGER()
	{
		long long int NumberofIterations=0;
		SYSTEM_TIMESPEC MM_START_TIME=currentTime();
		SYSTEM_TIMESPEC START_HYPER_PERIOD=MM_START_TIME;
		while ((this->isMAX_NUMBER_OF_FRAMES_SET && NumberofIterations<this->MAX_NUMBER_OF_FRAMES) || (!this->isMAX_NUMBER_OF_FRAMES_SET))
		{
			if (this->isMAX_NUMBER_OF_FRAMES_SET)
				NumberofIterations++;
			LOGGER(INFO,"START Hyperperiod");

			//SYSTEM_TIMESPEC NEXT_HYPER_PERIOD=addTimeNano(START_HYPER_PERIOD,this->MAJOR_FRAME_SIZE);
			for(unsigned int i =0; i<SCHEDULING_VECTOR.size();i++)
			{
				SYSTEM_TIMESPEC ABSOLUTE_OFFSET_TIME=addTimeNano(START_HYPER_PERIOD,SCHEDULING_VECTOR[i].OFFSET);
				SYSTEM_TIME_TYPE CRITICAL_INSTANCE = SCHEDULING_VECTOR[i].OFFSET+SCHEDULING_VECTOR[i].DURATION;
				SYSTEM_TIMESPEC ABSOLUTE_DURATION_TIME=addTimeNano(START_HYPER_PERIOD,CRITICAL_INSTANCE);
				int clockresult =1;
				// you can spuriously get out of nanosleep
				struct timespec rem = {0,0};
				while ( clockresult != 0) {
					clockresult = clock_nanosleep( CLOCK_REALTIME ,TIMER_ABSTIME, &ABSOLUTE_OFFSET_TIME,&rem);
				}
				RUN_PARTITION(SCHEDULING_VECTOR[i].PARTITION_INDEX,ABSOLUTE_DURATION_TIME);	

			}
			LOGGER(INFO,"Waiting for next Hyperperiod");
			//SYSTEM_TIME_TYPE FRAME_OFFSET=this->MAJOR_FRAME_SIZE*(SYSTEM_TIME_TYPE)(NumberofIterations);			
			START_HYPER_PERIOD=START_HYPER_PERIOD + this->MAJOR_FRAME_SIZE;
			struct timespec rem = {0,0};
			int clockresult =1;
			while ( clockresult != 0) {
				clockresult = clock_nanosleep( CLOCK_REALTIME ,TIMER_ABSTIME, &START_HYPER_PERIOD,&rem);
			}
		}

		DOSHUTDOWN(true);
		return -1;

	}

	int MODULE_MANAGER::RUN_MODULE_MANAGER()
	{

		LOGGER(INFO,"creating sockets");
		createSockets(); // Create your Sockets;

		LOGGER(INFO,"created sockets");

		SYNC_WITH_OTHER_MODULE_MANAGERS();

		LOGGER(INFO,"Done Sync 1.");

		//LOAD_AND_INITIALIZE_PARTITION_EXECUTABLE
		for(unsigned int i =0; i<PARTITION_VECTOR.size();i++)
		{
			LOAD_AND_INITIALIZE_PARTITION_EXECUTABLE(i);
		}
#ifndef ISNETWORK_PARTITION
		if(killfile.good() && killfile.is_open())
		{
			LOGGER(APP,"Cleanup File Closed");
			killfile.close();
			//chmod(killfilename,S_IRWXU);
		}

#endif
		LOGGER(INFO,"Loaded partition");

		// If number of stages = 2 go for another step of synchronization

		//Determine the network Delay

		//	DETERMINE_AVERAGE_NETWORK_LATENCY();

		SYNC_WITH_OTHER_MODULE_MANAGERS(); //Barrier

		/*DOSHUTDOWN(true);
		exit(0);*/




		if(NumberOfInitStages==2)
		{
			SYNC_WITH_OTHER_MODULE_MANAGERS();
			Synchronize();
		}

		HELPER_CLOCK_NANOSLEEP(convertSecsToNano(1));

		LOGGER(TRACE,"GET_READY_MM_FOR_HYPERPERIOD");

		GET_READY_MM_FOR_HYPERPERIOD(); // This will be Used for Hyper period synchronization...

		//HELPER_CLOCK_NANOSLEEP(convertSecsToNano(1)); //Ensure all have reached this point

		if((!this->isSystemModuleManager)&&(!this->isrunningUnderSystemModuleManager))
			return (RUN_WITHOUT_SYSTEM_MANAGER());
		else if (this->isSystemModuleManager)
			return RUN_SYSTEM_MODULE_MANAGER();
		else
			return RUN_CHILD_MODULE_MANAGER();

	}

	void MODULE_MANAGER::CREATE_DUMMY_PROCESS(void)
	{
		DUMMY_PROCESS_ID = 0;

		int i = 0;
		pid_t newPid;

		//create the process
		newPid = fork();

		if ( newPid == -1 )
		{
			LOGGER(INFO,"Fake process creation failed (fork) for process: %d\n", i );
		}
		else
		{	
			//process creation succeeded
			if ( newPid == 0 )
			{
				//set its priority to between the controller and workers
				struct sched_param sp_fakeDummy;
				sp_fakeDummy.sched_priority = DUMMY_PROCESS_PRIORITY;
				int schedRetValue = sched_setscheduler( 0, ACM_SCHED_POLICY, &sp_fakeDummy);
				if ( schedRetValue == -1 )
				{
					//This failure happens when the user is not a root.	//The framework should log this error and EXIT.
					LOGGER(  FATAL, "Error when trying to set FIFO scheduling: %s", strerror( errno ));
				}
				//we are in the child
				while ( 1) {
					pthread_testcancel();
					//LOGGER(INFO,"In DUMMY");
				}
			}
			else
			{
				//we are in the parent
				//store the PID of the fake process
				DUMMY_PROCESS_ID = newPid;
				//LOGGER(INFO,"Dummy process PID: %d\n", DUMMY_PROCESS_ID );
			}
		}
	}
};