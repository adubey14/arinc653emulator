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

#ifndef _Arinc_Emulated_Process_cc
#define _Arinc_Emulated_Process_cc
#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>

#ifdef IS_MODULE_MANAGER
#undef USE_CORBA
#undef USE_MICO
#undef USE_CIAO
#endif

#ifdef USE_CIAO
#include <ace/SString.h>
#include <tao/Exception.h>
#endif

#ifdef USE_MICO
#include <CORBA.h>
#include <mico/throw.h>
#endif

namespace ACM_INTERNAL {

PROCESS::PROCESS_state PROCESS::get_state() const { return state; }
std::string PROCESS::get_name() const {
  return std::string(this->attribute.NAME);
}
/*
ACM_Base_Exception_ptr PROCESS::getException(){
// Create a copy of the exception here

ACM_Base_Exception_ptr toBeReturned = my_exception_ptr->createCopy();
this->clearException();
return toBeReturned;
}
void PROCESS::clearException(){
if(this->state!=PROCESS::READY_RUNNING)
{
AUTO_LOCK lock (this->mutex_op_critical);
my_exception_ptr=0;
}
}
void PROCESS::setException(ACM_Base_Exception_ptr except){
AUTO_LOCK lock (this->mutex_op_critical);
my_exception_ptr=except;

}*/
// bool PROCESS::waitOnNotificationCondition(){
//	if(this->state!=PROCESS::RUNNING &&this->state!=PROCESS::READY){
//	return true;
//	}
//	this->notificationMUTEX.lock();
//	bool ret=this->notificationCOND_VAR.wait();
//	this->notificationMUTEX.unlock();
//	return ret;
//}

/*
void PROCESS::setRequest_argument_ptr(void* request){
AUTO_LOCK lock (this->mutex_op_critical);
this->request_argument_ptr=request;

}
void* PROCESS::getRequest_argument_ptr(){
return this->request_argument_ptr;
}*/

void PROCESS::waiting_on_period(
    /*out*/ ACM_USER::RETURN_CODE_TYPE *RETURN_CODE) {
  if (!this->isPeriodic()) {
    *RETURN_CODE = INVALID_MODE;
    return;
  }

  this->set_state(PROCESS::WAITING_ON_PERIOD);

  LOGGER(ACM_USER::TRACE, "Blocking on Sem: %s", this->attribute.NAME);
  LOGGER(ACM_USER::TRACE, "END_PROCESS: %s", this->attribute.NAME);

  // Block on The Sem
  sem_state.wait(); //! Waiting for State to Change
  LOGGER(ACM_USER::TRACE, "Released on Sem: %s", this->attribute.NAME);
  LOGGER(ACM_USER::TRACE, "START_PROCESS: %s", this->attribute.NAME);
}

void PROCESS::set_state(PROCESS_state s) { state = s; }
void PROCESS::post_state_change() { sem_state.post(); }

void PROCESS::INITIALIZE_PROCESS_RESPONSE() {

  PROCESS_RESPONSE.NO_ERROR = true;
  PROCESS_RESPONSE.RESPONDING_ENTITY = ACM_USER::PROCESS;
  //	HARD_DEADLINE_PROCESS_RESPONSE.ERROR_RESPONSE.ERROR_CODE=ACM_USER::NO_ERROR;
  PROCESS_RESPONSE.ERROR_RESPONSE.HM_MESSAGE_LENGTH = 0;

  // PROCESS_RESPONSE.RESPONSE_STATUS=PROCESS_RESPONSE_TYPE::NO_ERROR;
  // PROCESS_RESPONSE.PROCESSING_COMPLETED=true;
  // strcpy((char*)&(PROCESS_RESPONSE.RESPONSE_MESSAGE[0]),"finished");
  // PROCESS_RESPONSE.RESPONDING_ENTITY=PROCESS_RESPONSE_TYPE::PROCESS;
}

/*! We have to get rid of all the new statements and replace it with an own
 * memory manager
*/
PROCESS::PROCESS(PROCESS_ATTRIBUTE_TYPE &attr,
                 PROCESS_ID_TYPE unique_process_id)
    : process_id(unique_process_id), attribute(attr),
      thread_detached(PROCESS::NOT_DETACHED), state(DORMANT),
      mutex_op_critical(false, MUTEX::RECURSIVE),
      // notificationMUTEX(false,MUTEX::RECURSIVE),
      //  notificationCOND_VAR(&notificationMUTEX),
      delayed_start_flag(false)

{

  this->absolute_delay_start_time.tv_sec = 0;
  this->absolute_delay_start_time.tv_nsec = 0;

  LOGGER(INFO, "Inside constructor of process %s", attribute.NAME);

  //! Attribute ptr cannot be null

  /*if (attribute_ptr==0 || attribute_ptr==NULL){
  //We are in a bad situation. Throw an execption
  std::string Message("From PROCESS's constructor: A null PROCESS attribute
  pointer passed to the constructor");

  ACM_BASE_EXCEPTION_TYPE ex (Message);
  throw (ex);

  }*/
  //! Attributes process wrapper cannot be null

  if (attribute.ENTRY_POINT == 0 || attribute.ENTRY_POINT == NULL) {
    // We are in a bad situation. Throw an execption
    std::string Message("From PROCESS's constructor: NULL ENTRY_POINT in "
                        "PROCESS_attribute is Illegal");

    ACM_BASE_EXCEPTION_TYPE ex(Message);
    throw(ex);
  }
  // Do the Construction here.
  PROCESS::ERROR_TYPE result = this->create_thread();
  if (result != NO_ERROR) {
    char buffer[100];
    snprintf(buffer, 100, "Problem: Return From create_thread: code is %d",
             result);
    LOGGER(ERROR, "Problem: Return From create_thread: code is %d", result);
    ACM_BASE_EXCEPTION_TYPE exception(buffer);
    throw(exception);
  }
}

PROCESS::~PROCESS() {

  // std::string Name(this->attribute.NAME);
  this->terminate(); //! Wait for completion
  this->wait();
  // LOGGER(ACM_USER::DEBUG,"Waiting for thread to destruct");
  // delete(& (this->attribute)); //delete the attribute
  // LOGGER(ACM_USER::DEBUG,"Inside PROCESS %s Destructor. Thread has died. Wait
  // Finished.",Name.c_str());
}

void PROCESS::restart(bool doMitigation) {
  // LOGGER (ACM_USER::DEBUG,"Initiating restart on ArincEmulatorThread");
  set_state(PROCESS::RESTARTING);
  this->terminate();
  // LOGGER (ACM_USER::DEBUG,"Waiting for the ArincEmulatorThread to
  // terminate");
  void *status;
  this->wait(&status);
  // LOGGER(ACM_USER::DEBUG,"Setting Arinc Emulator Thread to idle");
  this->set_state(PROCESS::DORMANT);
  PROCESS::ERROR_TYPE result = this->create_thread();

  if (result != PROCESS::NO_ERROR) {
    LOGGER(ERROR,
           "Problem while restarting %s: Return From create_thread: code is %d",
           this->attribute.NAME, result);
    return;
  }

  if (this->isPeriodic() && doMitigation) {
    ACM_USER::RETURN_CODE_TYPE RETURN_CODE;
    ACM_INTERNAL::APEX::START(this->process_id, &RETURN_CODE);
    if (RETURN_CODE != ACM_USER::NO_ERROR) {

      LOGGER(ERROR, "APEX::START failed while restarting process %s:And the "
                    "error code is %d",
             this->attribute.NAME, RETURN_CODE);

      return;
    }
  }

  LOGGER(HMEVENT, "%s STOPPED", this->attribute.NAME);
}

/*! partition controller will call start to release a thread
*/
void PROCESS::start() {
  if (this->get_state() == PROCESS::READY) {
    this->set_state(PROCESS::RUNNING);
    // RESET Their Event
    RETURN_CODE_TYPE RETURN_CODE;
    APEX_HELPER_RESET_EVENT(this->attribute.NAME, &RETURN_CODE);
    this->sem_state.post(); // Post the state Change Message
  }
}

void PROCESS::delayed_start(SYSTEM_TIME_TYPE timeout) {
  if (this->get_state() == PROCESS::READY) {
    if (this->isPeriodic()) {
      SYSTEM_TIMESPEC actualtime = currentTime();
      this->absolute_delay_start_time = addTimeNano(actualtime, timeout);
      this->delayed_start_flag = true;
    }
    this->set_state(PROCESS::RUNNING);
    // RESET Their Event
    RETURN_CODE_TYPE RETURN_CODE;
    APEX_HELPER_RESET_EVENT(
        this->attribute.NAME,
        &RETURN_CODE);      //<- change this to write to blackboard.....
    this->sem_state.post(); // Post the state Change Message
  }
}

// create a start and wait for response...

// void PROCESS::suspend(); //probably we won't implement this
void PROCESS::stop(bool restart) { // Will cause a restart of the thread
  this->set_state(PROCESS::DORMANT);

  // this->setException(new ACM::ACM_TIMING_EXCEPTION_TYPE(std::string("Timing
  // Failure in Process Named")+this->attribute_ptr->NAME));
  //! Inform Waiting Threads that this process has been stopped
  // this->notificationMUTEX.lock();
  // this->notificationMUTEX.unlock();
  // this->notificationCOND_VAR.broadcast();

  // Restart the Thread
  if (restart)
    this->restart();
}
void PROCESS::stop_self() { this->set_state(PROCESS::DORMANT); }

// void PROCESS::suspend_self();
// void PROCESS::resume(); //probably we won't implement this

long PROCESS::get_priority() {
  int policy = -1;
  struct sched_param sp_Thread;
  // int retVal = -99;
  /*if (this->thread_id()<0){
  return retVal;
  }*/
  pthread_getschedparam(this->id, &policy, &sp_Thread);
  return sp_Thread.sched_priority;
}
void PROCESS::set_priority(long priority) {
  // if (this->thread_id()<0){
  //	return;
  //	}
  // create the sruct
  // this->CURRENT_PRIORITY=priority;
  struct sched_param sp_Thread;
  sp_Thread.sched_priority = priority;
  int schedRetValue = 99;
  // checking which thread's priority to change
  schedRetValue = pthread_setschedparam(this->id, ACM_SCHED_POLICY, &sp_Thread);
  if (schedRetValue != 0) {
    LOGGER(ERROR,
           "pthread_setschedparam at %d for thread_id %d. return code is %d",
           __LINE__, this->id, schedRetValue);
  }
}

void PROCESS::create_key(Thread_key &key, void (*__cleanup)(void *)) {
  int result = pthread_key_create(&key, __cleanup);
  assert(result == 0);
}
void PROCESS::delete_key(Thread_key &key) {
  int result = pthread_key_delete(key);
  assert(result == 0);
}
void *PROCESS::get_specific(const Thread_key &key) {
  return pthread_getspecific(key);
}

void PROCESS::set_specific(const Thread_key &key, void *value) {
  int result = pthread_setspecific(key, value);
  assert(result == 0);
}

PROCESS::ThreadID PROCESS::self() { return pthread_self(); }
bool PROCESS::operator==(const PROCESS &aep) const {
  return (bool)(aep.process_id == this->process_id);
}

void *PROCESS::Thread_wrapper(void *arg) {
  static_cast<PROCESS *>(arg)->run();
  return NULL;
}

PROCESS::ERROR_TYPE PROCESS::create_thread() {
  if (pthread_create(&id, NULL, Thread_wrapper, this) != 0) {
    return PROCESS::THREAD_FAILURE;
  }
  if (thread_detached == PROCESS::DETACHED) {
    pthread_detach(id);
  }
  return PROCESS::NO_ERROR;
}

void PROCESS::Thread_cleanup_Handler(void *data) {
  // This is a cleanup Handler
  PROCESS *current = static_cast<PROCESS *>(data);
  // PROCESS_ID_TYPE my_id;
  RETURN_CODE_TYPE RETURN_CODE;
  std::vector<SEMAPHORE_ID_TYPE> SEM_ID_VEC;

  for (std::list<SEMAPHORE_ID_TYPE>::iterator iter = current->READLOCKS.begin();
       iter != current->READLOCKS.end(); iter++) {
    LOGGER(HMEVENT, "PROCESS %s stopped. But it still holds READ LOCK to "
                    "sempahore id %ld. Unlocking it.",
           current->attribute.NAME, *iter);
    SEMAPHORE_ID_TYPE SEM_ID = *iter;
    // iter++;
    // APEX::READUNLOCK_RW_SEMAPHORE(SEM_ID,&RETURN_CODE);
    // assert(RETURN_CODE==ACM_USER::NO_ERROR);
    SEM_ID_VEC.push_back(SEM_ID);
  }

  for (unsigned int i = 0; i < SEM_ID_VEC.size(); i++) {
    APEX::READUNLOCK_RW_SEMAPHORE(SEM_ID_VEC[i], &RETURN_CODE);
    assert(RETURN_CODE == ACM_USER::NO_ERROR);
  }

  SEM_ID_VEC.clear();

  for (std::list<SEMAPHORE_ID_TYPE>::iterator iter =
           current->WRITELOCKS.begin();
       iter != current->WRITELOCKS.end(); iter++) {
    LOGGER(HMEVENT, "PROCESS %s stopped. But it still holds WRITE LOCK to "
                    "sempahore id %ld. Unlocking it.",
           current->attribute.NAME, *iter);
    SEMAPHORE_ID_TYPE SEM_ID = *iter;
    // iter++;
    SEM_ID_VEC.push_back(SEM_ID);
    // APEX::WRITEUNLOCK_RW_SEMAPHORE(SEM_ID,&RETURN_CODE);
    // assert(RETURN_CODE==ACM_USER::NO_ERROR);
  }
  for (unsigned int i = 0; i < SEM_ID_VEC.size(); i++) {
    APEX::WRITEUNLOCK_RW_SEMAPHORE(SEM_ID_VEC[i], &RETURN_CODE);
    assert(RETURN_CODE == ACM_USER::NO_ERROR);
  }
  SEM_ID_VEC.clear();

  //	ACM_INTERNAL::APEX::GET_MY_ID(&my_id,&RETURN_CODE);
  //	LOGGER(APP,"pthread Cleanp for process %s : id
  //%ld",current->attribute.NAME,my_id);
  //std::cout<<PROCESS::self()<<std::endl;
  // std::cout<<"MY ID IS "<<my_id<<std::endl;

  // This is where we will call the unlock if required.
  //	LOGGER(ACM_USER::DEBUG,"pthread Cleanp for process"pthread Cleanp for
  //process  %s. Specific cleanup code can can be called
  //here.",current->attribute.NAME);
}

void PROCESS::run() {
  RETURN_CODE_TYPE RETURN_CODE;
  // BLACKBOARD_ID_TYPE MY_BOARD_ID=-1;
  // APEX::GET_PROCESS_NOTIFICATION_BOARD_ID(this->process_id,&MY_BOARD_ID,&RETURN_CODE);
  BLACKBOARD_ID_TYPE MY_HM_BOARD_ID = 1000;
  HM_RESPONSE_TYPE HM_RESPONSE;
  SYSTEM_TIME_TYPE TIME_OF_START;
  pthread_cleanup_push(Thread_cleanup_Handler, (void *)this);
  do {
    sem_state.wait(); //! Waiting for State to Change
    if (get_state() ==
        PROCESS::RUNNING) { // State is Ready_Running. Do Some Work

      if (this->delayed_start_flag) {
        int clockresult = 1;
        // you can spuriously get out of nanosleep
        struct timespec rem = {0, 0};
        while (clockresult != 0) {
          clockresult =
              clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME,
                              &(this->absolute_delay_start_time), &rem);
        }
      }
      this->delayed_start_flag = false;
      LOGGER(ACM_USER::TRACE, "START_PROCESS: %s", this->attribute.NAME);
      TIME_OF_START = currentTimeInNanoSeconds();
      try {
        void (*fptr)(void);

        fptr = (void (*)(void))(this->attribute.ENTRY_POINT);
        fptr(); // Call real user code.
      } catch (ACM_BASE_EXCEPTION_TYPE &ex) {
        LOGGER(ACM_USER::ERROR, "Exception occured while doing the Run");
        // PROCESS_RESPONSE.RESPONSE_STATUS=PROCESS_RESPONSE_TYPE::OTHER_ERROR;

        PROCESS_RESPONSE.NO_ERROR = false;
        PROCESS_RESPONSE.ERROR_RESPONSE.HM_MESSAGE_DISCRIMINATOR =
            ACM_USER::STRING;
        strncpy((char *)&(PROCESS_RESPONSE.ERROR_RESPONSE.HM_MESSAGE[0]),
                ex.getMessage().c_str(), ERROR_RESPONSE_TYPE_MESSAGE_SIZE);
        PROCESS_RESPONSE.ERROR_RESPONSE.HM_MESSAGE_LENGTH =
            ERROR_RESPONSE_TYPE_MESSAGE_SIZE; // strlen(ex.getMessage().c_str());

        //	this->setException(&ex);
      }
#ifdef USE_MICO
      catch (CORBA::Exception &exception) {
        LOGGER(ACM_USER::ERROR, "CORBA Exception occured while doing the Run");
        std::stringstream st;
        exception._print(st);
        std::string ststring(st.str());
        PROCESS_RESPONSE.NO_ERROR = false;
        PROCESS_RESPONSE.ERROR_RESPONSE.HM_MESSAGE_DISCRIMINATOR =
            ACM_USER::STRING;
        strncpy((char *)&(PROCESS_RESPONSE.ERROR_RESPONSE.HM_MESSAGE[0]),
                ststring.c_str(), ERROR_RESPONSE_TYPE_MESSAGE_SIZE);
        PROCESS_RESPONSE.ERROR_RESPONSE.HM_MESSAGE_LENGTH =
            ERROR_RESPONSE_TYPE_MESSAGE_SIZE; // strlen(ex.getMessage().c_str());

      }
#endif

#ifdef USE_CIAO
      catch (CORBA::Exception &exception) {

        if (!APEX::checkTerminateStatus()) {
          LOGGER(ACM_USER::ERROR,
                 "CORBA Exception occured while doing the Run");
          std::string ststring = exception._info().c_str();
          PROCESS_RESPONSE.NO_ERROR = false;
          PROCESS_RESPONSE.ERROR_RESPONSE.HM_MESSAGE_DISCRIMINATOR =
              ACM_USER::STRING;
          strncpy((char *)&(PROCESS_RESPONSE.ERROR_RESPONSE.HM_MESSAGE[0]),
                  ststring.c_str(), ERROR_RESPONSE_TYPE_MESSAGE_SIZE);
          PROCESS_RESPONSE.ERROR_RESPONSE.HM_MESSAGE_LENGTH =
              ERROR_RESPONSE_TYPE_MESSAGE_SIZE; // strlen(ex.getMessage().c_str());
        }
      }
#endif

      // catch(...)
      // {
      // LOGGER(ACM_USER::ERROR,"Unknown Exception occured while doing the
      // Run");
      // PROCESS_RESPONSE.NO_ERROR=false;
      // strncpy((char*)&(PROCESS_RESPONSE.ERROR_RESPONSE.HM_MESSAGE[0]),"Unknown
      // Exception occured while doing the Run"
      // ,ERROR_RESPONSE_TYPE_MESSAGE_SIZE);
      // PROCESS_RESPONSE.ERROR_RESPONSE.HM_MESSAGE_LENGTH=ERROR_RESPONSE_TYPE_MESSAGE_SIZE;

      // }

      if (this->isPeriodic()) {
        // periodic
        this->set_state(PROCESS::WAITING_ON_PERIOD);
      } else {
        // aperiodic
        // this->notificationMUTEX.lock();
        // this->notificationCOND_VAR.broadcast();
        // this->notificationMUTEX.unlock();
        RETURN_CODE_TYPE RETURN_CODE;
        APEX_HELPER_SET_EVENT(this->attribute.NAME,
                              &RETURN_CODE); // Left for backward compatibility.
                                             // We should use notification board
                                             // now.
        this->set_state(PROCESS::DORMANT);
      }

      // check if its own blackboard is empty...
      // if it is not empty. Then no process can be blocked on it
      // APEX::GET_BLACKBOARD_STATUS(MY_BOARD_ID,&MY_BOARD_STATUS,&RETURN_CODE);
      // if(MY_BOARD_STATUS.EMPTY_INDICATOR==ACM_USER::EMPTY)
      //{
      // std::cout<<"MYboard is empty \n";

      // Check and update process response if there is an error_code in the HM
      // blackboard.

      // Read the HM Blackboard

      if (!APEX::checkTerminateStatus()) {

#ifndef OPTIMAL
        APEX::GET_PROCESS_HM_BOARD_ID(this->process_id, &MY_HM_BOARD_ID,
                                      &RETURN_CODE);
        if (RETURN_CODE == ACM_USER::NO_ERROR) {
          // HM_BOARD is available
          BLACKBOARD_STATUS_TYPE MY_BOARD_STATUS;
          APEX::GET_BLACKBOARD_STATUS(MY_HM_BOARD_ID, &MY_BOARD_STATUS,
                                      &RETURN_CODE);
          if (MY_BOARD_STATUS.EMPTY_INDICATOR !=
              ACM_USER::EMPTY) { // Read the board
            MESSAGE_SIZE_TYPE HM_RESPONSE_LENGTH;
            APEX::READ_BLACKBOARD(MY_HM_BOARD_ID, 0,
                                  (MESSAGE_ADDR_TYPE)&HM_RESPONSE,
                                  &HM_RESPONSE_LENGTH, &RETURN_CODE);
            if (RETURN_CODE == ACM_USER::NO_ERROR && HM_RESPONSE_LENGTH > 0 &&
                TIME_OF_START < HM_RESPONSE.TIMEOFMESSAGE) {
              memcpy(&(PROCESS_RESPONSE.ERROR_RESPONSE), &HM_RESPONSE,
                     sizeof(HM_RESPONSE_TYPE));
              PROCESS_RESPONSE.NO_ERROR = false;
              PROCESS_RESPONSE.RESPONDING_ENTITY = ACM_USER::HM;
            }
          }
        }

        APEX::NOTIFY_PROCESS_FINISHED_STATUS(
            /*!in */ this->process_id,
            /*!out*/ &PROCESS_RESPONSE,
            /*!out*/ &RETURN_CODE);
#endif
      }

      //}
      // else
      //{
      // ACM_USER::PROCESS_RESPONSE_TYPE response;
      // MESSAGE_SIZE_TYPE PROCESS_RESPONSE_LENGTH;
      // APEX::GET_PROCESS_FINISHED_STATUS(
      //					   /*!in */ my_id,
      //					   /*!in */ 0,
      //					   /*!out*/ (MESSAGE_ADDR_TYPE)
      //(&response),
      //					   /*!in */ (MESSAGE_SIZE_TYPE)
      //sizeof(response),
      //					   /*!out*/
      //&PROCESS_RESPONSE_LENGTH,
      //					   /*!out*/&RETURN_CODE);
      // if(PROCESS_RESPONSE_LENGTH >0){
      //	std::cout<<"Received Response \n";
      //	std::cout<<response.RESPONSE_MESSAGE<<std::endl;
      //	std::cout<<"RESPONDING
      //ENTITY"<<response.RESPONDING_ENTITY<<std::endl;
      //	std::cout<<"TIME OF MESSAGE"<<response.TIMEOFMESSAGE<<std::endl;
      //}
      //}

      LOGGER(ACM_USER::TRACE, "END_PROCESS: %s", this->attribute.NAME);
    }
  } while (true);

  pthread_cleanup_pop(0);
}
void PROCESS::terminate() {
  LOGGER(ACM_USER::DEBUG, "Terminating PROCESS: %s", this->attribute.NAME);
  pthread_cancel(this->thread_id());
  // assert(!result);
}
void PROCESS::wait(void **exitval) {

  pthread_join(this->thread_id(), exitval);
  LOGGER(ACM_USER::DEBUG, "Terminated PROCESS: %s", this->attribute.NAME);
}

} // ACM

#endif //_Arinc_Emulated_Process_cc
