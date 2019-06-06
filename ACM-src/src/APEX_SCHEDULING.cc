//# Copyright (c) Vanderbilt University, 2010
//# ALL RIGHTS RESERVED
//# Vanderbilt University disclaims all warranties with regard to this
//# software, including all implied warranties of merchantability
//# and fitness.  In no event shall Vanderbilt University be liable for
//# any special, indirect or consequential damages or any damages
//# whatsoever resulting from loss of use, data or profits, whether
//# in an action of contract, negligence or other tortious action,
//# arising out of or in connection with the use or performance of
//# this software.
#ifndef _APEX_SCHEDULING_cc
#define _APEX_SCHEDULING_cc
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

using namespace ACM_USER;
namespace ACM_INTERNAL {
	/*======================================	SIGNAL HANDLING BUSINESS			==========================================================*/
	bool APEX::moduleManagerIsParent =false;
	void APEX::getParentInfo()
	{
		struct sched_param sp_Process;

		//after setting FIFO scheduling, get controllers ABSOLUTE priority	
		if ( sched_getparam( getppid(), &sp_Process ) == 0 ) 
		{	//PartitionManager has 99 ACM_SCHED_POLICY priority
			if ( sp_Process.sched_priority == PARTITION_MANAGER_PRIORITY )
			{
				//parent seems to be PartitionManager
				//set senddignal to true
				APEX::moduleManagerIsParent = true;
			}
		}

		//LOGGER(INFO,"partitionManagerIsParent is %d",partitionManagerIsParent);
	}

	int APEX::sigIntCount=0;

	//send the signal to the partition controller that this partition is ready with initialization
	//signal is SIGUSR1
	void APEX::sendInitializationReadySignalToPartitionController()
	{

		if ( moduleManagerIsParent ) {
			kill( getppid() , SIGUSR1 );

		}
	}

	/** AD: 11/25/2011 
	Adding new function addresses for user specified signal handlers. These if provided will be invoked before the signal handler code implemented by the emulator
	*/
	void APEX::SET_USER_SIGINT_HANDLER(SYSTEM_ADDRESS_TYPE  entry_point)
	{
		USER_SIGINT_HANDLER=entry_point;

	}
	
	 void APEX::SET_USER_PRE_START_HANDLER(SYSTEM_ADDRESS_TYPE  entry_point)
	{
		USER_PRE_START_HANDLER=entry_point;
	}
	/** AD: 11/25/2011 
	Adding new function addresses for user specified signal handlers. These if provided will be invoked before the signal handler code implemented by the emulator
	*/
	void APEX::SET_USER_SIGTERM_HANDLER(SYSTEM_ADDRESS_TYPE  entry_point)
	{
		USER_SIGTERM_HANDLER=entry_point;

	}


	void APEX::sigIntHandler(int sig){      
		LOGGER (INFO,"Received Sigint in the Partition");
		APEX::sigIntCount++;
		if(APEX::sigIntCount>1){
			LOGGER(ERROR,"There was some problem in clean termination. You asked for immediate termination");
			APEX::CLEAR_VECTORS();
			LOG2FILE();
			exit (-1);
		}

		// Called only for first sigint
		/** AD: 11/25/2011 
		Adding new function addresses for user specified signal handlers. These if provided will be invoked before the signal handler code implemented by the emulator
		*/
		if(USER_SIGINT_HANDLER!=0)
		{
			void (*fptr)(void);
			fptr=(void (*)(void)) (USER_SIGINT_HANDLER);
			LOGGER(DEBUG,"Calling User Sigint handler");
			fptr(); 
			LOGGER(DEBUG,"Out of User Sigint handler");
		}

	
		if (_pinstance!=0 && _pinstance->CURRENT_OPERATING_MODE==NORMAL) 
		{
			_pinstance->flagToTerminate=true;
		}
		else
		{
			APEX::TERMINATE();
			LOGGER(APP,"PARTITION EXITING");
			exit(0);
		}
		return;                         
	}   


	void APEX::sigTermHandler(int sig)
	{
		LOGGER (INFO,"Received SigTerm in the Partition");
		// Calling user specified handler
		/** AD: 11/25/2011 
		Adding new function addresses for user specified signal handlers. These if provided will be invoked before the signal handler code implemented by the emulator
		*/
		if(USER_SIGTERM_HANDLER!=0)
		{
			void (*fptr)(void);
			fptr=(void (*)(void)) (USER_SIGTERM_HANDLER);
			LOGGER(DEBUG,"Calling User Sigterm handler");
			fptr(); 
			LOGGER(DEBUG,"Out of User Sigterm handler");
		}

		LOGGER(ERROR,"Received SIGTERM. You asked for immediate termination");
		APEX::CLEAR_VECTORS();
		LOG2FILE();
		exit (0);
	}

	void APEX::sigsegvHandler(int sig)
	{
		LOGGER (FATAL,"Received sigsegv in the Partition. Segementation Violation Happened");
		printf("Partition %s: Segementation Violation Happened \n", APEX::PARTITION_NAME.c_str());				
		APEX::CLEAR_VECTORS();
		LOG2FILE();
		exit (1); 
	}
	


	/*======================================	SET FIFO			==========================================================*/

	void APEX::SET_FIFO_POLICY() throw (ACM_BASE_EXCEPTION_TYPE){
		//set CPU affinity to bind this process it to processor 1
		//the setting will be inherited by fork, execv, and to threads
		LOGGER(INFO,"Running without Module Manager, Setting the FIFO Policy");
		cpu_set_t mask;    
		CPU_ZERO(&mask); /* clear mask */
		CPU_SET(CPU_CORE_TO_USE,&mask); /* processors 0 */ 
		unsigned int len = sizeof(mask);
		if (sched_setaffinity(0, len, &mask) < 0) {
			LOGGER(FATAL, "Error when trying to set CPU affinity: %s", strerror( errno ));
		}
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
				//This failure happens when the user is not a root.	//The framework should log this error and EXIT.
				LOGGER(  FATAL, "Error when trying to set FIFO scheduling: %s", strerror( errno ));
				char buffer[100];snprintf(buffer,100, "Error when trying to set FIFO scheduling: %s", strerror( errno ));
				throw ACM_BASE_EXCEPTION_TYPE(buffer);
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
			if ( sp_Process.sched_priority < CONTROLLER_PROCESS_PRIORITY)
			{
				LOGGER(ACM_USER::DEBUG,  "setting main thread priority to %d", CONTROLLER_PROCESS_PRIORITY );
				sp_Process.sched_priority = CONTROLLER_PROCESS_PRIORITY;
				pthread_setschedparam(pthread_self(), ACM_SCHED_POLICY, &sp_Process);
			}
		}
	}
	
	void APEX::SET_PARTITION_PRIORITY()
	{
	
		if (! (APEX::moduleManagerIsParent)) return;
		
		//set CPU affinity to bind this process it to processor 1
		//the setting will be inherited by fork, execv, and to threads
		LOGGER(INFO,"Running With Module Manager, Setting Controller priority to CONTROLLER_PROCESS_PRIORITY");
		
		
			
		//get self scheduling, and if it's not FIFO, set it to be FIFO
		int policy = -1;
		struct sched_param sp_Process;
		int retVal = pthread_getschedparam( pthread_self(), &policy, & sp_Process );
		if (retVal!= 0)
		{
			LOGGER(ERROR,"pthread_getschedparam at %d failed with %d",__LINE__,retVal); 
		}
		
		if(policy!=ACM_SCHED_POLICY)
		{
			LOGGER(ACM_USER::ERROR,"Partition FIFO Policy is not set - Running with MM");
			exit(-1);
		}
		else
		{
			struct sched_param sp_Controller;
			sched_getparam( 0, &sp_Controller ); 
			LOGGER(ACM_USER::DEBUG,  "Controller's ABSOLUTE priority: %d", sp_Controller.sched_priority );
			
			retVal = pthread_getschedparam( pthread_self(), &policy, & sp_Process );
			if ( sp_Process.sched_priority > CONTROLLER_PROCESS_PRIORITY)
			{
				LOGGER(ACM_USER::DEBUG,  "setting main thread priority to %d", CONTROLLER_PROCESS_PRIORITY );
				sp_Process.sched_priority = CONTROLLER_PROCESS_PRIORITY;
				pthread_setschedparam(pthread_self(), ACM_SCHED_POLICY, &sp_Process);
			}		
		}		
	}

	/*====================================Scheduler Initialization and Scheduling services==================================================*/

	int APEX::InitPartitionScheduler(/*!out*/ RETURN_CODE_TYPE *RETURN_CODE)
	{
		//This method is invoked from set_partition_mode before the mode is set to normal
		//set_partition_mode ensures that this method is not invoked if the partition is already in mode = normal.
		APEX* partition_instance = APEX::_pinstance;
		if (partition_instance ==NULL)
		{
			*RETURN_CODE=INVALID_CONFIG;
			return -1;
		}
		partition_instance->CURRENT_OPERATING_MODE = NORMAL;
		LOGGER(ACM_USER::DEBUG,"STARTING RunPartitionScheduler()");

		//signal to the parent

#ifndef OPTIMAL

		//wait for scheduling
		if(APEX::moduleManagerIsParent)
		{
			sendInitializationReadySignalToPartitionController();
			LOGGER(ACM_USER::DEBUG,"Waiting for partition manager");
			raise (SIGSTOP);
			LOGGER(ACM_USER::DEBUG,"Received SIG Continue from partition Manager");
		}

		
		if(USER_PRE_START_HANDLER!=0)
		{
			void (*fptr)(void);
			fptr=(void (*)(void)) (USER_PRE_START_HANDLER);
			LOGGER(DEBUG,"Calling USER_PRE_START_HANDLER");
			fptr(); 
			LOGGER(DEBUG,"Out of USER_PRE_START_HANDLER");
		}
		

#endif	

	char *entries = getenv("ACM_INIT_TIME");
	if (entries!=NULL)
    {
      long init_wait_time = atol(entries);
      if (init_wait_time>0)
      {
	  
		LOGGER(APP,"*** Setting Init Wait Time to %ld and starting the wait phase",init_wait_time);
		SYSTEM_TIMESPEC actualtime=currentTime();
		SYSTEM_TIMESPEC absolute_time=addTimeNano(actualtime,convertSecsToNano(init_wait_time));
		struct timespec rem = {0,0};
		int clockresult =1;
		while ( clockresult != 0) {
				clockresult = clock_nanosleep( CLOCK_REALTIME ,TIMER_ABSTIME, &absolute_time,&rem);
				}
		LOGGER(APP,"***Finished Init Wait phase.");
	  }
    }
  
		
		RunPartitionScheduler();
		APEX::TERMINATE();
		LOGGER(APP,"Partition Exiting");
		return 0;
	}

	/*====================================Run Partition Scheduler==================================================*/

	void APEX::RunPartitionScheduler()
	{


		PROCESS_RESPONSE_TYPE HARD_DEADLINE_PROCESS_RESPONSE;		
		HARD_DEADLINE_PROCESS_RESPONSE.NO_ERROR=false;
		HARD_DEADLINE_PROCESS_RESPONSE.RESPONDING_ENTITY=PARTITIONSCHEDULER;
		HARD_DEADLINE_PROCESS_RESPONSE.ERROR_RESPONSE.ERROR_CODE=ACM_USER::DEADLINE_MISSED;
		HARD_DEADLINE_PROCESS_RESPONSE.ERROR_RESPONSE.HM_MESSAGE_LENGTH=0;
		HARD_DEADLINE_PROCESS_RESPONSE.ERROR_RESPONSE.HM_ACTION=HM_RESPONSE_TYPE::STOP;
		//{PROCESS_RESPONSE_TYPE::HARD_DEADLINE_MISSED,0, false,PROCESS_RESPONSE_TYPE::PARTITIONSCHEDULER,"violation"};
		//PROCESS_RESPONSE_TYPE SOFT_DEADLINE_PROCESS_RESPONSE {PROCESS_RESPONSE_TYPE::SOFT_DEADLINE_MISSED,0, false,PROCESS_RESPONSE_TYPE::PARTITIONSCHEDULER,"violation"};
		APEX* partition_instance = APEX::_pinstance;
		SYSTEM_TIME_TYPE nano = convertSecsToNano(1);
		SYSTEM_TIMESPEC next_sleep_time;
		if (partition_instance ==NULL)
		{
			return;
		}

		//start with a schedulability analysis
		DoSchedulabilityAnalysis(); //dummy

		//set all the thread priorities to the non-working level as the default
		//DoPriorityChange( false );

		//check that process_mutex is unlocked

		assert(partition_instance->PROCESS_TABLE_MUTEX.try_lock_without_locking() == MUTEX::NO_ERROR);

		//lock section
		partition_instance->PROCESS_TABLE_MUTEX.lock();

		//initialize first schedule
#ifdef OPTIMAL

                //wait for scheduling
                if(APEX::moduleManagerIsParent)
                {
                        sendInitializationReadySignalToPartitionController();
                        LOGGER(ACM_USER::DEBUG,"Waiting for partition manager");
                        raise (SIGSTOP);
                        LOGGER(ACM_USER::DEBUG,"Received SIG Continue from partition Manager");
                }


                if(USER_PRE_START_HANDLER!=0)
                {
                        void (*fptr)(void);
                        fptr=(void (*)(void)) (USER_PRE_START_HANDLER);
                        LOGGER(DEBUG,"Calling USER_PRE_START_HANDLER");
                        fptr();
                        LOGGER(DEBUG,"Out of USER_PRE_START_HANDLER");
                }

#endif



		for(unsigned int i = 0;i<partition_instance->PROCESS_DESCRIPTOR_VECTOR.size();i++)
		{
			PROCESS* process = partition_instance->PROCESS_DESCRIPTOR_VECTOR[i]->THE_PROCESS;

			partition_instance->PROCESS_DESCRIPTOR_VECTOR[i]->NEXT_SCHEDULING_INSTANCE= PROCESS_DESCRIPTOR::RELEASE_PROCESS; //set the next schedule to release	

			//process is in state - waiting to schedule
			PROCESS::PROCESS_state status =process->get_state();
			if (status ==PROCESS::WAITING_PARTITION_NOT_NORMAL) 
			{

			if(partition_instance->PROCESS_DESCRIPTOR_VECTOR[i]->INITIAL_DELAY_TIME==0)
			{
				partition_instance->PROCESS_DESCRIPTOR_VECTOR[i]->ABSOLUTE_NEXT_SCHEDULED_TIME = currentTime();
			}
			else
			{
				partition_instance->PROCESS_DESCRIPTOR_VECTOR[i]->ABSOLUTE_NEXT_SCHEDULED_TIME = 
				//currentTime(); // compute next release point
				addTimeNano(currentTime(),partition_instance->PROCESS_DESCRIPTOR_VECTOR[i]->INITIAL_DELAY_TIME);
		    }
				
				partition_instance->PROCESS_DESCRIPTOR_VECTOR[i]->ABSOLUTE_CRITICAL_TIME = partition_instance->PROCESS_DESCRIPTOR_VECTOR[i]->ABSOLUTE_NEXT_SCHEDULED_TIME;


				if (process->isPeriodic()) //set process state based on the nature of the process
					process->set_state(PROCESS::WAITING_ON_PERIOD); //set process state to waiting on period for periodic process.
				else
					process->set_state(PROCESS::READY); //set process state to ready for aperiodic process.

			}

		}

		//		partition_instance->PROCESS_TABLE_MUTEX.unlock();



		//		LOGGER(INFO,"In RunScheduler::after unlocking , the process table mutex status is %ld", partition_instance->PROCESS_TABLE_MUTEX.try_lock_without_locking());




		while (partition_instance->CURRENT_OPERATING_MODE == NORMAL && !partition_instance->flagToTerminate)
		{
			partition_instance->PROCESS_TABLE_MUTEX.lock(); //try if it is not locked
			partition_instance->sortProcessDescriptorVector(); 
			//APEX::logProcessDescriptorVector();
			//check is vector size is 0 then sleep for a default time of 1 second.
			if (
				(partition_instance->PROCESS_SCHEDULING_SORTED_VECTOR.size()==0) || 
				(partition_instance->PROCESS_SCHEDULING_SORTED_VECTOR[0]->THE_PROCESS->get_state()==PROCESS::DORMANT) || 
				(
				(partition_instance->PROCESS_SCHEDULING_SORTED_VECTOR[0]->THE_PROCESS->attribute.TIME_CAPACITY==INFINITE_TIME_VALUE ) 
				&& 
				(partition_instance->PROCESS_SCHEDULING_SORTED_VECTOR[0]->NEXT_SCHEDULING_INSTANCE==PROCESS_DESCRIPTOR::CHECK_DEADLINE)
				)
				)
			{
				LOGGER(ACM_USER::DEBUG,"From  APEX Scheduler: Nothing to do" );
				next_sleep_time =currentTime();
				next_sleep_time = addTimeNano(next_sleep_time,nano);
				APEX_SCHEDULER_WAIT_ABSOLUTE_TIME(next_sleep_time); // May be wait forever and get triggered if there is something to do.
				continue;
			}

			//partition_instance->PROCESS_TABLE_MUTEX.try_lock(); //try if it is not locked
			//partition_instance->sortProcessDescriptorVector(); //locks and sorts the vector

			//Do the scheduling loop		
			//SYSTEM_TIMESPEC actualtime=currentTime();

			//LOGGER( DEBUG, "Size of PROCESS_SCHEDULING_SORTED_VECTOR is  %d",partition_instance->PROCESS_SCHEDULING_SORTED_VECTOR.size());

			//APEX::logProcessDescriptorVector();

			//take the head of the vector
			next_sleep_time= currentTime();
			next_sleep_time = addTimeNano(next_sleep_time,nano);

			if (partition_instance->PROCESS_SCHEDULING_SORTED_VECTOR.size()>0)
			{
				next_sleep_time=partition_instance->PROCESS_SCHEDULING_SORTED_VECTOR[0]->ABSOLUTE_CRITICAL_TIME;					
			}

			if (!APEX_SCHEDULER_WAIT_ABSOLUTE_TIME(next_sleep_time))
				continue;
				
			////printf ("out of sleep \n");

			SYSTEM_TIMESPEC current_time= currentTime();

			for (unsigned int count =0 ; count <partition_instance->PROCESS_SCHEDULING_SORTED_VECTOR.size(); count++)
			{


				//if absolute critical time is less than or equal to current time
				if (compareTime(partition_instance->PROCESS_SCHEDULING_SORTED_VECTOR[count]->ABSOLUTE_CRITICAL_TIME, current_time))
				{
					//printf ("in the main loop %d \n",__LINE__);
					PROCESS_DESCRIPTOR* process_descriptor =partition_instance->PROCESS_SCHEDULING_SORTED_VECTOR[count];
					switch (process_descriptor->NEXT_SCHEDULING_INSTANCE)
					{
					case PROCESS_DESCRIPTOR::RELEASE_PROCESS:
						{								
							//setting state to Ready and Releasing it;

							//Check that the current state should be DORMANT or waiting on pERIOD or WAITING on Partition not Normal
							PROCESS::PROCESS_state current_process_state = process_descriptor->THE_PROCESS->get_state();

							//current_process_state == PROCESS::DORMANT ||
							if (current_process_state == PROCESS::WAITING_ON_PERIOD || current_process_state == PROCESS::READY)
							{
								process_descriptor->THE_PROCESS->set_priority(process_descriptor->THE_PROCESS->attribute.BASE_PRIORITY);
								process_descriptor->CURRENT_PRIORITY=process_descriptor->THE_PROCESS->attribute.BASE_PRIORITY;

								//LOGGER( DEBUG, "State of process %s is %d",process_descriptor->THE_PROCESS->attribute.NAME,current_process_state);

								process_descriptor->THE_PROCESS->INITIALIZE_PROCESS_RESPONSE(); //!<This will initialize the process response back to default
								process_descriptor->THE_PROCESS->set_state(PROCESS::READY);
								LOGGER( DEBUG, "Releasing process %s",process_descriptor->THE_PROCESS->attribute.NAME);
								LOGGER(DEBUG," ABSOLUTE_CRITICAL_TIME of %s is %ld.%ld",process_descriptor->THE_PROCESS->attribute.NAME,process_descriptor->ABSOLUTE_CRITICAL_TIME.tv_sec,process_descriptor->ABSOLUTE_CRITICAL_TIME.tv_nsec);
								process_descriptor->THE_PROCESS->start();

								process_descriptor->ABSOLUTE_PREV_START_TIME=current_time; //set previous schedule time

								//set next schedule time for periodic processes
								if (process_descriptor->THE_PROCESS->isPeriodic())

								{
									//process_descriptor->ABSOLUTE_NEXT_SCHEDULED_TIME=addTimeNano(current_time,process_descriptor->THE_PROCESS->attribute.PERIOD);
									process_descriptor->ABSOLUTE_NEXT_SCHEDULED_TIME=addTimeNano(process_descriptor->ABSOLUTE_CRITICAL_TIME,process_descriptor->THE_PROCESS->attribute.PERIOD);
									//std::cout<<"tick\n";
								}

								//process_descriptor->DEADLINE_TIME = computeDeadlineTimeSpecs(current_time,process_descriptor->THE_PROCESS->attribute.TIME_CAPACITY);
								process_descriptor->DEADLINE_TIME = computeDeadlineTimeSpecs(process_descriptor->ABSOLUTE_CRITICAL_TIME,process_descriptor->THE_PROCESS->attribute.TIME_CAPACITY);
								process_descriptor->ABSOLUTE_CRITICAL_TIME=process_descriptor->DEADLINE_TIME;//set absolute critical time
								//process_descriptor->CRITICAL_TIME=process_descriptor->THE_PROCESS->attribute.TIME_CAPACITY; //set critical time
								process_descriptor->NEXT_SCHEDULING_INSTANCE= PROCESS_DESCRIPTOR::CHECK_DEADLINE;  //set next scheduling action

							}
						}
						break;

					case PROCESS_DESCRIPTOR::CHECK_DEADLINE:
						{


							LOGGER(ACM_USER::DEBUG, "Checking Deadline for %s",process_descriptor->THE_PROCESS->attribute.NAME);

							PROCESS::PROCESS_state current_process_state = process_descriptor->THE_PROCESS->get_state();
							if (current_process_state == PROCESS::DORMANT ||current_process_state == PROCESS::WAITING_ON_PERIOD)
							{

								//Deadline not violated
								LOGGER(ACM_USER::DEBUG, "No Deadline Violation for %s",process_descriptor->THE_PROCESS->attribute.NAME);

								if (current_process_state == PROCESS::WAITING_ON_PERIOD) //set the next release time for periodic process
									process_descriptor->ABSOLUTE_CRITICAL_TIME=process_descriptor->ABSOLUTE_NEXT_SCHEDULED_TIME;

								process_descriptor->NEXT_SCHEDULING_INSTANCE= PROCESS_DESCRIPTOR::RELEASE_PROCESS;	//set the next action to release

							}
							else
							{		
								//APEX::logProcessDescriptorVector();
								if ( process_descriptor->THE_PROCESS->attribute.DEADLINE == HARD ){
									//Stopping the process and setting its state to DORMANT.
									LOGGER( HMEVENT, "DETECT HARD DEADLINE VIOLATION for %s:%s ",process_descriptor->THE_PROCESS->attribute.NAME,partition_instance->PARTITION_NAME.c_str());	
									process_descriptor->THE_PROCESS->set_priority(WORKER_THREAD_RESTART_PRIORITY);
									process_descriptor->THE_PROCESS->stop(true);
									process_descriptor->THE_PROCESS->set_priority(process_descriptor->THE_PROCESS->attribute.BASE_PRIORITY);
									process_descriptor->NEXT_SCHEDULING_INSTANCE= PROCESS_DESCRIPTOR::RELEASE_PROCESS;	

									//Inform the health Manager of this process.
									LOGGER(HMEVENT,"RAISE_ERROR for %s: %s", process_descriptor->THE_PROCESS->get_name().c_str(),partition_instance->PARTITION_NAME.c_str());
									RETURN_CODE_TYPE RETURN_CODE;
									APEX::NOTIFY_PROCESS_FINISHED_STATUS(process_descriptor->THE_PROCESS->process_id,&HARD_DEADLINE_PROCESS_RESPONSE,&RETURN_CODE );
									APEX::RAISE_APPLICATION_ERROR(process_descriptor->THE_PROCESS->process_id,DEADLINE_MISSED,0,0, &RETURN_CODE);



								}
								else{
									//If ever we get into the situation where anaperiodic process with has soft deadline and
									// it is suspended, the scheduler will go into running at every second to monitor the deadline of the aperiodic process.
									//we will still continue to monitor this for deadline violation. Setting a default critical time of 1 second
									LOGGER(  HMEVENT, "WARNING, Soft Deadline Violation for %s: %s ",process_descriptor->THE_PROCESS->attribute.NAME,partition_instance->PARTITION_NAME.c_str());
									//we will still continue to monitor this for deadline violation
									SYSTEM_TIMESPEC c = currentTime();
									process_descriptor->ABSOLUTE_CRITICAL_TIME= addTimeNano(c,nano);

									//!!! TEST Added by abhishek to get rid of the soft deadline violation bug

									//set next schedule time for periodic processes
									if (process_descriptor->THE_PROCESS->isPeriodic())

									{
										//! Required to ensure that the process starts in phase even when it misses the deadline.
										while(compareTime(process_descriptor->ABSOLUTE_NEXT_SCHEDULED_TIME,process_descriptor->ABSOLUTE_CRITICAL_TIME))
											//process_descriptor->ABSOLUTE_NEXT_SCHEDULED_TIME=addTimeNano(current_time,process_descriptor->THE_PROCESS->attribute.PERIOD);
											process_descriptor->ABSOLUTE_NEXT_SCHEDULED_TIME=addTimeNano(process_descriptor->ABSOLUTE_NEXT_SCHEDULED_TIME,process_descriptor->THE_PROCESS->attribute.PERIOD);
										//std::cout<<"tick\n";
									}
									//!!! TEST Added by abhishek to get rid of the soft deadline violation bug
									//Inform the health Manager of this process.
									LOGGER(HMEVENT,"Informing HM of %ld of deadline violation", process_descriptor->THE_PROCESS->process_id);
									RETURN_CODE_TYPE RETURN_CODE;
									//APEX::NOTIFY_PROCESS_FINISHED_STATUS(process_descriptor->THE_PROCESS->process_id,&SOFT_DEADLINE_PROCESS_RESPONSE,&RETURN_CODE );
									APEX::RAISE_APPLICATION_ERROR(process_descriptor->THE_PROCESS->process_id,DEADLINE_MISSED,0,0, &RETURN_CODE);

								}
							}

						}//  end case



					} // end switch
				} //end if
				else
					break; // @nag : This loop probably should not continue if there is more time for the processes to launch

			}  //end for periodic process vector


		} //end while

		LOGGER(ACM_USER::DEBUG,"Terminate Flag is Set. Getting out of Run Partition Scheduler");

	}

	//void APEX::DoPriorityChange(  bool setControllerThread  )
	//{
	//	//int i = 0;

	//	APEX* partition_instance = APEX::_pinstance;
	//	if (partition_instance ==NULL)
	//	{
	//		return;
	//	}
	//	//check the priority status
	//	if ( partition_instance->PRIORITY_LEVEL == LOW_LEVEL ) // THis is a flag that indicates wether the partition is waking up or going to sleep
	//	{
	//		//the priorities are already low, we have to raise them back = this partition is running
	//		//log it
	//		//LOGGER( msg, "doPriorityChange UP" );
	//		//logMessageToFile( msg );

	//		//if we have to set the controllers priority too
	//		if ( setControllerThread )
	//		{
	//			//printf( "doPriorityChange UP CONTRLLER\n" );
	//			//finally raise controller thread's priority
	//			struct sched_param sp_Thread;
	//			sp_Thread.sched_priority = CONTROLLER_PROCESS_PRIORITY;
	//			int schedRetValue = 99;
	//			//checking which thread's priority to change
	//			schedRetValue = pthread_setschedparam( pthread_self(), ACM_SCHED_POLICY, &sp_Thread);
	//		}

	//		//AUTO_LOCK lock (partition_instance->PROCESS_TABLE_MUTEX);

	//		//raise each thread priority to the original value (only which is still in use)
	//		for(unsigned int i = 0;i<partition_instance->PROCESS_DESCRIPTOR_VECTOR.size();i++)
	//		{
	//			PROCESS* process = partition_instance->PROCESS_DESCRIPTOR_VECTOR[i]->THE_PROCESS;
	//			if (process->get_state()==PROCESS::RESTARTING)
	//			{
	//				process->set_priority( WORKER_THREAD_RESTART_PRIORITY );
	//			}
	//			else
	//			{
	//				process->set_priority(partition_instance->PROCESS_DESCRIPTOR_VECTOR[i]->CURRENT_PRIORITY);
	//			}
	//		}
	//	}
	//	else
	//	{
	//		//the priorities are already high, we have to lower them back = this partition is not running
	//		//lower each thread priority to the LOW_PRIORITY VALUE
	//		{
	//			//AUTO_LOCK lock (partition_instance->PROCESS_TABLE_MUTEX);

	//			//lower each thread priority to the original value (only which is still in use)
	//			for(unsigned int i = 0;i<partition_instance->PROCESS_DESCRIPTOR_VECTOR.size();i++)
	//			{
	//				PROCESS* process = partition_instance->PROCESS_DESCRIPTOR_VECTOR[i]->THE_PROCESS;
	//				process->set_priority(PARTITION_PROCESS_LOWPRIORITY);
	//			}

	//		}

	//		//if we have to set the controllers priority too
	//		if ( setControllerThread )
	//		{
	//			//printf( "doPriorityChange DOWN CONTRLLER\n" );
	//			//finally lower the controller thread's priority
	//			struct sched_param sp_Thread;
	//			sp_Thread.sched_priority = PARTITION_PROCESS_LOWPRIORITY;
	//			int schedRetValue = 99;
	//			//checking which thread's priority to change
	//			schedRetValue = pthread_setschedparam( pthread_self(), ACM_SCHED_POLICY, &sp_Thread);
	//		}

	//	}

	//	//set the priority status to opposite
	//	partition_instance->PRIORITY_LEVEL =(partition_instance->PRIORITY_LEVEL == LOW_LEVEL) ? HIGH_LEVEL : LOW_LEVEL ;


	//}

}

#endif//_APEX_SCHEDULING_cc
