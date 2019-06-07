/*# Copyright (c) Vanderbilt University, 2010
# ALL RIGHTS RESERVED
# Vanderbilt University disclaims all warranties with regard to this
# software, including all implied warranties of merchantability
# and fitness.  In no event shall Vanderbilt University be liable for
# any special, indirect or consequential damages or any damages
# whatsoever resulting from loss of use, data or profits, whether
# in an action of contract, negligence or other tortious action,
# arising out of or in connection with the use or performance of
# this software.
*/

#ifndef _APEX_h
#define _APEX_h

#include <ACM/INTERNAL/ACM_THREAD_SUPPORT.h>
#include <ACM/INTERNAL/BLACKBOARD.h>
#include <ACM/INTERNAL/BUFFER.h>
#include <ACM/INTERNAL/EVENT.h>
#include <ACM/INTERNAL/PROCESS.h>
#include <ACM/INTERNAL/QUEUE_PORT.h>
#include <ACM/INTERNAL/SAMPLING_PORT.h>
#include <ACM/USER/ACM_COMPONENT.h>
#include <ACM/USER/ACM_EXCEPTION.h>
#include <ACM/USER/ACM_HM_TYPES.h>
#include <ACM/USER/ACM_LOGGER.h>
#include <ACM/USER/APEX_TYPES.h>

#include <algorithm>
#include <functional>
#include <string.h>
#include <vector>

using namespace ACM_USER;

#define __GET_SINGLETON_INSTANCE                                               \
  APEX *partition_instance = APEX::_pinstance;                                 \
  if (partition_instance == NULL) {                                            \
    *RETURN_CODE = INVALID_CONFIG;                                             \
    return;                                                                    \
  }

#define __GET_SINGLETON_INSTANCE_AND_CREATE_IF_DOES_NOT_EXIST                  \
  APEX *partition_instance =                                                   \
      (APEX::_pinstance == 0) ? APEX::getInstance() : APEX::_pinstance;        \
  if (partition_instance == NULL) {                                            \
    *RETURN_CODE = INVALID_CONFIG;                                             \
    return;                                                                    \
  }

//! ACM_INTERNAL namespace is never exposed to the USERS.
namespace ACM_INTERNAL {
//! the Partition class. All methods and members of this class are static. No
//! Instance of this class is ever created or destroyed.
class APEX {
private:
  //! An internal struct. It holds necessary information for process scheduling.
  struct PROCESS_DESCRIPTOR {
    SYSTEM_TIMESPEC
        ABSOLUTE_CRITICAL_TIME; //!< The ansolute time stamp used in scheduling
    ACM_INTERNAL::APEX_EVENT_TYPE SUSPENSION_EVENT; //!< It is used if the
                                                    //!process calls
                                                    //!suspend_self. Suspend
                                                    //!call (not self) is
                                                    //!implemented using
                                                    //!priorities
    ACM_INTERNAL::PROCESS *THE_PROCESS;             //!< The real process.

    enum NEXT_SCHEDULING_INSTANCE_TYPE {
      RELEASE_PROCESS = 0,
      CHECK_DEADLINE = 1
    };
    NEXT_SCHEDULING_INSTANCE_TYPE NEXT_SCHEDULING_INSTANCE; //!<This tells
                                                            //!wether the next
                                                            //!critical
                                                            //!scheduling
                                                            //!instance is a
                                                            //!release kind or
                                                            //!check deadline
                                                            //!kind
    SYSTEM_TIMESPEC ABSOLUTE_PREV_START_TIME; //!< Used to record previous start
                                              //!time. Only for history
    SYSTEM_TIMESPEC ABSOLUTE_NEXT_SCHEDULED_TIME; //!< Used to record next
                                                  //!release time. Only used for
                                                  //!periodic processes. For
                                                  //!aperiodic processes. Next
                                                  //!scheduled time= Next
                                                  //!critical time
    SYSTEM_TIMESPEC DEADLINE_TIME; //!< This stores the next deadline
    SYSTEM_TIME_TYPE INITIAL_DELAY_TIME;
    PRIORITY_TYPE CURRENT_PRIORITY; //!< Used to store current Priority.
    HM_DESCRIPTOR_ID_TYPE
        HM_DESCRIPTOR_ID;   //!< Used to store the descriptor ID for the HM
    bool IS_HEALTH_MANAGER; //!< This flag is set to true for health managers.
    BLACKBOARD_ID_TYPE HM_RESPONSE_BOARD_ID; //!< Used to store the blackboard
                                             //!id for getting HM response back

    BLACKBOARD_ID_TYPE PROCESS_NOTIFICATION_RESPONSE_BOARD_ID; //!< Used to
                                                               //!store the
                                                               //!blackboard id
                                                               //!for getting
                                                               //!process
                                                               //!response back

    PROCESS_DESCRIPTOR(ACM_INTERNAL::PROCESS *aep, PROCESS_NAME_TYPE Name)
        : SUSPENSION_EVENT(std::string(Name) + "SuspensionEvent"),
          THE_PROCESS(aep), NEXT_SCHEDULING_INSTANCE(RELEASE_PROCESS),
          INITIAL_DELAY_TIME(0), HM_DESCRIPTOR_ID(-1), IS_HEALTH_MANAGER(false),
          HM_RESPONSE_BOARD_ID(1000) // this board does not exist
    {
      // Create a Descriptor
    }

    inline std::string toString() {
      std::stringstream temp;
      temp << "PROCESS_ID:" << this->THE_PROCESS->get_process_id();
      temp << "\t:NAME:" << this->THE_PROCESS->get_name()
           << "\t:PROCESS_STATE:" << this->THE_PROCESS->get_string_state();
      temp << "\t:Priority:" << this->THE_PROCESS->get_priority()
           << "\t:ABSOLUTE_CRITICAL_TIME:"
           << convertTimeSpecToNano(ABSOLUTE_CRITICAL_TIME);
      temp << "\t:ABSOLUTE_PREV_START_TIME:"
           << convertTimeSpecToNano(ABSOLUTE_PREV_START_TIME);
      temp << "\t:NEXT_SCHEDULING_INSTANCE:"
           << ((NEXT_SCHEDULING_INSTANCE == 0) ? "RELEASE_PROCESS"
                                               : "CHECK_DEADLINE");
      return temp.str();
    }

    //! This deletion happens only when the partition is shutting down.
    ~PROCESS_DESCRIPTOR() {

      if (THE_PROCESS != 0)
        delete (THE_PROCESS);
    }

  }; // End: PROCESS_DESCRIPTOR

public:
  static bool checkTerminateStatus();

private:
  static std::string PARTITION_NAME;
  //! Used to store the current state of partition

  APEX(); //!<Nobody is allowed to construct an instance of APEX
  static APEX *_pinstance;
  static ACM_INTERNAL::MUTEX
      singletonInstanceMutex; //!<Used to protect the singleton instance
  ACM_INTERNAL::MUTEX PROCESS_TABLE_MUTEX; //!< Used to protect the access to
                                           //!process table while scheduling
  ACM_INTERNAL::APEX_EVENT_TYPE
      *APEX_SCHEDULING_EVENT; //!<Used by the scheduler

  enum PRIORITY_LEVEL_TYPE { LOW_LEVEL = 0, HIGH_LEVEL };
  PRIORITY_LEVEL_TYPE PRIORITY_LEVEL;
  static bool moduleManagerIsParent; //!< This flag is true if running inside a
                                     //!module manager. other wise it is false.
  static int sigIntCount;
  OPERATING_MODE_TYPE
      CURRENT_OPERATING_MODE; //!<Current Partition Operating Mode
  bool flagToTerminate; //!< This flag is set after signal SIGINT is received
  //! \name Internal Data Structures for holding the various entities. Id's are
  //! function of the vector index
  typedef std::vector<PROCESS_DESCRIPTOR *> PROCESS_DESCRIPTOR_VECTOR_TYPE;
  PROCESS_DESCRIPTOR_VECTOR_TYPE
      PROCESS_DESCRIPTOR_VECTOR; //!< All process descriptors
  PROCESS_DESCRIPTOR_VECTOR_TYPE PROCESS_SCHEDULING_SORTED_VECTOR; //<! This
                                                                   //vector is
                                                                   //especially
                                                                   //used for
                                                                   //sorting
                                                                   //through
                                                                   //schedules;
  typedef std::vector<ACM_INTERNAL::APEX_EVENT_TYPE *> EVENT_VECTOR_TYPE;
  EVENT_VECTOR_TYPE EVENT_VECTOR;
  typedef std::vector<ACM_INTERNAL::APEX_BLACKBOARD_TYPE *>
      BLACKBOARD_VECTOR_TYPE;
  BLACKBOARD_VECTOR_TYPE BLACKBOARD_VECTOR;
  typedef std::vector<ACM_INTERNAL::SEMAPHORE *> SEMAPHORE_VECTOR_TYPE;
  SEMAPHORE_VECTOR_TYPE SEMAPHORE_VECTOR;
  typedef std::vector<ACM_INTERNAL::RW_LOCK_SEM *> RW_LOCK_SEM_VECTOR_TYPE;
  RW_LOCK_SEM_VECTOR_TYPE RW_LOCK_SEM_VECTOR;
  typedef std::vector<SAMPLING_PORT_HELPER *> SAMPLING_PORT_VECTOR_TYPE;
  typedef std::vector<MESSAGE_QUEUE_TYPE *> QUEUING_PORT_VECTOR_TYPE;
  typedef std::vector<APEX_BUFFER_TYPE *> BUFFER_VECTOR_TYPE;
  SAMPLING_PORT_VECTOR_TYPE SAMPLING_PORT_VECTOR;
  QUEUING_PORT_VECTOR_TYPE QUEUING_PORT_VECTOR;
  BUFFER_VECTOR_TYPE BUFFER_VECTOR;
  typedef std::vector<HM_DESCRIPTOR_TYPE> HM_VECTOR_TYPE;
  HM_VECTOR_TYPE HM_VECTOR;
  typedef std::map<PROCESS_ID_TYPE, ACM_INTERNAL::PROCESS::ThreadID *>
      PROCESS_ID_TO_PTHREAD_ID_MAP_TYPE;
  PROCESS_ID_TO_PTHREAD_ID_MAP_TYPE PROCESS_ID_TO_PTHREAD_ID_MAP; //<!Used to
                                                                  //map internal
                                                                  //process id
                                                                  //to actual
                                                                  //pthread id.
                                                                  //used for
                                                                  //get_my_id

private:
  static void TERMINATE();
  static void CLEAR_VECTORS(); //!<Used to delete all the internal data
                               //!structures except PROCESS
  int InitPartitionScheduler(/*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  void RunPartitionScheduler();
  void DoSchedulabilityAnalysis(); //<! This Method is a placeholder. Will be
                                   //possiblly implemented in future.
  // void  DoPriorityChange(bool setScheduler);
  void ComputeNextScheduleForWaitingToSchedule();
  static void sigIntHandler(int sig);
  static void sigTermHandler(int sig);
  static void sigsegvHandler(int sig);
  /*!
  * This method is used to suspend the execution of this process till a resume
  * is called by someone.
  * It implements a signal handler for SIGUSR2
  */

  static void handle_sigusr2_suspend(int sig);
  static void SET_FIFO_POLICY() throw(ACM_BASE_EXCEPTION_TYPE);
  static void SET_PARTITION_PRIORITY();

  /** AD: 11/25/2011
     Adding new function addresses for user specified signal handlers. These if
     provided will be invoked before the signal handler code implemented by the
     emulator
  */

  static SYSTEM_ADDRESS_TYPE USER_SIGINT_HANDLER;
  static SYSTEM_ADDRESS_TYPE USER_SIGTERM_HANDLER;
  static SYSTEM_ADDRESS_TYPE USER_PRE_START_HANDLER;

public:
  static void SET_USER_SIGINT_HANDLER(SYSTEM_ADDRESS_TYPE entry_point);
  static void SET_USER_SIGTERM_HANDLER(SYSTEM_ADDRESS_TYPE entry_point);
  static void SET_USER_PRE_START_HANDLER(SYSTEM_ADDRESS_TYPE entry_point);

private:
  //! \name APEX INTERNAL FUNCTIONS
  //@{
  /* !
  *
  */
  inline static bool PROCESS_DESCRIPTOR_COMP(const PROCESS_DESCRIPTOR *first,
                                             const PROCESS_DESCRIPTOR *second) {
    assert(first != 0);
    assert(second != 0);
    assert(first->THE_PROCESS != 0);
    assert(second->THE_PROCESS != 0);

    if ((first->THE_PROCESS->get_state() == ACM_INTERNAL::PROCESS::DORMANT) &&
        (second->THE_PROCESS->get_state() == ACM_INTERNAL::PROCESS::DORMANT))
      return false;

    if (second->THE_PROCESS->get_state() == ACM_INTERNAL::PROCESS::DORMANT)
      return true;

    if (first->THE_PROCESS->get_state() == ACM_INTERNAL::PROCESS::DORMANT)
      return false;

    return compareTime(first->ABSOLUTE_CRITICAL_TIME,
                       second->ABSOLUTE_CRITICAL_TIME);
  }

  inline static void APEX_HELPER_CLOCK_NANOSLEEP(SYSTEM_TIME_TYPE TIME_OUT) {
    SYSTEM_TIMESPEC actualtime = currentTime();
    SYSTEM_TIMESPEC absolute_time = addTimeNano(actualtime, TIME_OUT);
    if (_pinstance == 0) {
      int clockresult = 1;
      // you can spuriously get out of nanosleep
      struct timespec rem = {0, 0};
      while (clockresult != 0) {
        clockresult = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME,
                                      &absolute_time, &rem);
      }
    } else {
      // LOGGER(INFO,"***************USING CONDITION VARIABLE INSIDE SCHEDULER
      // FOR WAITING********");
      RETURN_CODE_TYPE returncode;
      _pinstance->APEX_SCHEDULING_EVENT->WAIT_EVENT(TIME_OUT, &returncode);
      // LOGGER(INFO,"***Return Code of of waiting is %d********", returncode);
    }
  }

  //<! Will return true if it has timed out. otherwise if somebody has set the
  //event it will return false.
  inline static bool
  APEX_SCHEDULER_WAIT_ABSOLUTE_TIME(SYSTEM_TIMESPEC TIME_OUT) {
    // LOGGER(INFO,"***************USING CONDITION VARIABLE INSIDE SCHEDULER FOR
    // WAITING********");
    RETURN_CODE_TYPE returncode;
    _pinstance->APEX_SCHEDULING_EVENT->WAIT_AND_LOCK_EVENT(TIME_OUT,
                                                           &returncode);
    if (returncode == ACM_USER::TIMED_OUT)
      return true;
    _pinstance->APEX_SCHEDULING_EVENT->RESET_EVENT(&returncode);
    return false;
  }

  //<! This method should be called with a lock to this->PROCESS_TABLE_MUTEX
  inline void sortProcessDescriptorVector() {

    // ACM_INTERNAL::AUTO_LOCK lock(this->PROCESS_TABLE_MUTEX);

    std::sort(this->PROCESS_SCHEDULING_SORTED_VECTOR.begin(),
              this->PROCESS_SCHEDULING_SORTED_VECTOR.end(),
              APEX::PROCESS_DESCRIPTOR_COMP);
  }

  static void logProcessDescriptorVector(bool sortfirst = false) {
    if (_pinstance == 0)
      return;
    //	ACM_INTERNAL::AUTO_LOCK lock (_pinstance->PROCESS_TABLE_MUTEX);

    _pinstance->PROCESS_TABLE_MUTEX.lock();
    if (sortfirst)
      _pinstance->sortProcessDescriptorVector();
    std::stringstream temp;
    temp << std::endl;
    for (unsigned int i = 0;
         i < _pinstance->PROCESS_SCHEDULING_SORTED_VECTOR.size(); i++) {
      temp << i << ") "
           << _pinstance->PROCESS_SCHEDULING_SORTED_VECTOR[i]->toString();
      temp << std::endl;
    }
    LOGGER(INFO, "Descriptor %s", temp.str().c_str());
  }

  static void sendInitializationReadySignalToPartitionController();

  //! Used to get Parent Info and discover if a manager is present or not.
  static void getParentInfo();

public:
  static std::string GET_PARTITION_NAME();
  static void SET_PARTITION_NAME(const std::string &name);
  static APEX *getInstance();

  static void PRINT_CREATED_OBJECT_LIST(/*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  //! Destructor
  ~APEX();

  //////////////////////All Apex interfaces
public:
  static void REATTACH_SIGNAL_HANDLERS();

  //! \name Event Management Services
  //@{
  /* !
  *
  */

  static void CREATE_EVENT(
      /*!in */ EVENT_NAME_TYPE EVENT_NAME,
      /*!out*/ EVENT_ID_TYPE *EVENT_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void SET_EVENT(
      /*!in */ EVENT_ID_TYPE EVENT_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void RESET_EVENT(
      /*!in */ EVENT_ID_TYPE EVENT_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void WAIT_EVENT(
      /*!in */ EVENT_ID_TYPE EVENT_ID,
      /*!in */ SYSTEM_TIME_TYPE TIME_OUT,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_EVENT_ID(
      /*!in */ EVENT_NAME_TYPE EVENT_NAME,
      /*!out*/ EVENT_ID_TYPE *EVENT_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_EVENT_STATUS(
      /*!in */ EVENT_ID_TYPE EVENT_ID,
      /*!out*/ EVENT_STATUS_TYPE *EVENT_STATUS,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

  //!\name RW_LOCK_SEMAPHORE management services
  static void CREATE_RW_SEMAPHORE(
      /*!in */ SEMAPHORE_NAME_TYPE SEMAPHORE_NAME,
      /*!in */ SEMAPHORE_VALUE_TYPE MAXIMUM_READERS,
      /*!out*/ SEMAPHORE_ID_TYPE *SEMAPHORE_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_RW_SEMAPHORE_ID(
      /*!in */ SEMAPHORE_NAME_TYPE SEMAPHORE_NAME,
      /*!out*/ SEMAPHORE_ID_TYPE *SEMAPHORE_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

  //! Currently ignore Time_Out
  static void READLOCK_RW_SEMAPHORE(
      /*!in */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,
      /*!in */ SYSTEM_TIME_TYPE TIME_OUT,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

  static void READUNLOCK_RW_SEMAPHORE(
      /*!in */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

  static void WRITELOCK_RW_SEMAPHORE(
      /*!in */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,
      /*!in */ SYSTEM_TIME_TYPE TIME_OUT,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

  static void WRITEUNLOCK_RW_SEMAPHORE(
      /*!in */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

  //! \name SEMAPHORE   management services
  //@{
  /* !
  *
  */
  //! We currently ignore QUEUING_DISCIPLINE
  static void CREATE_SEMAPHORE(
      /*!in */ SEMAPHORE_NAME_TYPE SEMAPHORE_NAME,
      /*!in */ SEMAPHORE_VALUE_TYPE CURRENT_VALUE,
      /*!in */ SEMAPHORE_VALUE_TYPE MAXIMUM_VALUE,
      /*!in */ QUEUING_DISCIPLINE_TYPE QUEUING_DISCIPLINE,
      /*!out*/ SEMAPHORE_ID_TYPE *SEMAPHORE_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  //! Currently ignore Time_Out
  static void WAIT_SEMAPHORE(
      /*!in */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,
      /*!in */ SYSTEM_TIME_TYPE TIME_OUT,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void SIGNAL_SEMAPHORE(
      /*!in */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_SEMAPHORE_ID(
      /*!in */ SEMAPHORE_NAME_TYPE SEMAPHORE_NAME,
      /*!out*/ SEMAPHORE_ID_TYPE *SEMAPHORE_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_SEMAPHORE_STATUS(
      /*!in */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,
      /*!out*/ SEMAPHORE_STATUS_TYPE *SEMAPHORE_STATUS,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

  //! \name PARTITION management services
  //@{
  /* !
  *
  */

  static void GET_PARTITION_STATUS(
      /*!out*/ PARTITION_STATUS_TYPE *PARTITION_STATUS,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void SET_PARTITION_MODE(
      /*!in */ OPERATING_MODE_TYPE OPERATING_MODE,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

  //! \name Process management services
  //@{
  /* !
  *
  */
  static void CREATE_PROCESS(
      /*!in */ PROCESS_ATTRIBUTE_TYPE *ATTRIBUTES,
      /*!out*/ PROCESS_ID_TYPE *PROCESS_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void SET_PRIORITY(
      /*!in */ PROCESS_ID_TYPE PROCESS_ID,
      /*!in */ PRIORITY_TYPE PRIORITY,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void SUSPEND_SELF(
      /*!in */ SYSTEM_TIME_TYPE TIME_OUT,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void SUSPEND(
      /*!in */ PROCESS_ID_TYPE PROCESS_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void RESUME(
      /*!in */ PROCESS_ID_TYPE PROCESS_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void STOP_SELF();
  static void STOP(
      /*!in */ PROCESS_ID_TYPE PROCESS_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void START(
      /*!in */ PROCESS_ID_TYPE PROCESS_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void DELAYED_START(
      /*!in */ PROCESS_ID_TYPE PROCESS_ID,
      /*!in */ SYSTEM_TIME_TYPE DELAY_TIME,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_MY_ID(
      /*!out*/ PROCESS_ID_TYPE *PROCESS_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_PROCESS_ID(
      /*!in */ PROCESS_NAME_TYPE PROCESS_NAME,
      /*!out*/ PROCESS_ID_TYPE *PROCESS_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_PROCESS_STATUS(
      /*!in */ PROCESS_ID_TYPE PROCESS_ID,
      /*!out*/ PROCESS_STATUS_TYPE *PROCESS_STATUS,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_MY_NAME(/*!out*/ PROCESS_NAME_TYPE *PROCESS_NAME,
                          /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_PROCESS_NAME(/*!in */ PROCESS_ID_TYPE PROCESS_ID,
                               /*!out*/ PROCESS_NAME_TYPE *PROCESS_NAME,
                               /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  //! Not Implemented
  static void LOCK_PREEMPTION(
      /*!out*/ LOCK_LEVEL_TYPE *LOCK_LEVEL,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  //! Not Implemented
  static void UNLOCK_PREEMPTION(
      /*!out*/ LOCK_LEVEL_TYPE *LOCK_LEVEL,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void NOTIFY_PROCESS_FINISHED_STATUS(
      /*!in */ PROCESS_ID_TYPE PROCESS_ID,
      /*!out*/ PROCESS_RESPONSE_TYPE *PROCESS_RESPONSE,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

  static void GET_PROCESS_FINISHED_STATUS(
      /*!in */ PROCESS_ID_TYPE PROCESS_ID,
      /*!in */ SYSTEM_TIME_TYPE TIME_OUT,
      /*!out*/ MESSAGE_ADDR_TYPE PROCESS_RESPONSE,
      /*!in */ MESSAGE_SIZE_TYPE PROCESS_RESPONSE_CAPACITY,
      /*!out*/ MESSAGE_SIZE_TYPE *PROCESS_RESPONSE_LENGTH,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

  static void GET_PROCESS_NOTIFICATION_BOARD_ID(
      /*!in */ PROCESS_ID_TYPE PROCESS_ID,
      /*!out*/ BLACKBOARD_ID_TYPE *TARGET_BOARD_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

  //! \name Time management services
  //@{
  /* !
  *
  */
  static void TIMED_WAIT(
      /*!in */ SYSTEM_TIME_TYPE DELAY_TIME,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void PERIODIC_WAIT(
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_TIME(
      /*!out*/ SYSTEM_TIME_TYPE *SYSTEM_TIME,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  /*! Replenish is a misnomer. According to arinc spec.
   * new_deadline=current_time+budget_time; effectively, new deadline can be
   * less than current deadline
  */
  static void REPLENISH(
      /*!in */ SYSTEM_TIME_TYPE BUDGET_TIME,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

  //! \name ERROR Management Services
  //@{
  /* !
  *
  */
  static void REPORT_APPLICATION_MESSAGE(
      /*!in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
      /*!in */ MESSAGE_SIZE_TYPE LENGTH,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void CREATE_ERROR_HANDLER(
      /*!in */ SYSTEM_ADDRESS_TYPE ENTRY_POINT,
      /*!in */ STACK_SIZE_TYPE STACK_SIZE,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_ERROR_STATUS(
      /*!out*/ ERROR_STATUS_TYPE *ERROR_STATUS,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

  static void RAISE_APPLICATION_ERROR_AND_GET_RESPONSE(
      /*!in */ ERROR_CODE_TYPE ERROR_CODE,
      /*!in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
      /*!in */ ERROR_MESSAGE_SIZE_TYPE LENGTH,
      /*!in */ SYSTEM_TIME_TYPE
          TIME_OUT, // Added Timeout in response to the discussion with Gabor
      /*!out*/ HM_RESPONSE_TYPE *HM_RESPONSE,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void RAISE_APPLICATION_ERROR(
      /*!in */ ERROR_CODE_TYPE ERROR_CODE,
      /*!in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
      /*!in */ ERROR_MESSAGE_SIZE_TYPE LENGTH,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void RAISE_APPLICATION_ERROR_AND_GET_RESPONSE(
      /*!in */ ERROR_CODE_TYPE ERROR_CODE,
      /*!in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
      /*!in */ ERROR_MESSAGE_SIZE_TYPE LENGTH,
      /*!out*/ MESSAGE_ADDR_TYPE HM_RESPONSE,
      /*!in */ MESSAGE_SIZE_TYPE HM_RESPONSE_CAPACITY,
      /*!in */ SYSTEM_TIME_TYPE
          TIME_OUT, // Added Timeout in response to the discussion with Gabor
      /*!out*/ MESSAGE_SIZE_TYPE *HM_RESPONSE_LENGTH,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void RAISE_APPLICATION_ERROR(
      /*!in*/ PROCESS_ID_TYPE FAULTY_PROCESS_ID,
      /*!in */ ERROR_CODE_TYPE ERROR_CODE,
      /*!in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
      /*!in */ ERROR_MESSAGE_SIZE_TYPE LENGTH,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

  //! \name Added HM Services

  //! HEALTH Manager process is aperiodic. It runs with the highest prority in
  //! the partition. Waits on the buffer.
  static void CREATE_HEALTH_MANAGER_PROCESS(
      /*!in*/ SYSTEM_ADDRESS_TYPE ENTRY_POINT,
      /*!in*/ PROCESS_NAME_TYPE PROCESS_NAME,
      /*!in*/ STACK_SIZE_TYPE STACK_SIZE,
      /*!out*/ HM_DESCRIPTOR_ID_TYPE *HM_DESCRIPTOR_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void START_HM(
      /*!in */ HM_DESCRIPTOR_ID_TYPE HM_DESCRIPTOR_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void
  HM_GET_PROCESS_ERROR_MESSAGE(/*!out*/ ERROR_STATUS_TYPE *ERROR_STATUS,
                               /*!out*/ BLACKBOARD_ID_TYPE *TARGET_BBOARD_ID,
                               /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void
  HM_GET_MY_DESCRIPTOR_ID(/*!OUT*/ HM_DESCRIPTOR_ID_TYPE *HM_DESCRIPTOR_ID,
                          /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void REGISTER_PROCESS_WITH_HEALTH_MANAGER(
      /*!in*/ PROCESS_ID_TYPE PROCESS_ID,
      /*!in*/ HM_DESCRIPTOR_ID_TYPE HM_DESCRIPTOR_ID,
      /*!out*/ BLACKBOARD_ID_TYPE *BLACKBOARD_ID,
      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void HM_GET_MY_BUFFER_ID(/*!OUT*/ BUFFER_ID_TYPE *BUFFER_ID,
                                  /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void HM_NOTIFY_PROCESS(/*!in*/ PROCESS_ID_TYPE PROCESS_ID,
                                /*!in*/ HM_RESPONSE_TYPE *HM_RESPONSE,
                                /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_PROCESS_HM_BOARD_ID(/*!in */ PROCESS_ID_TYPE PROCESS_ID,
                                      /*!out*/ BLACKBOARD_ID_TYPE *HM_BOARD_ID,
                                      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

  //! \name BUFFER Management Services
  static void CREATE_BUFFER(
      /*in */ BUFFER_NAME_TYPE BUFFER_NAME,
      /*in */ MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
      /*in */ MESSAGE_RANGE_TYPE MAX_NB_MESSAGE,
      /*in */ QUEUING_DISCIPLINE_TYPE QUEUING_DISCIPLINE,
      /*out*/ BUFFER_ID_TYPE *BUFFER_ID,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void SEND_BUFFER(
      /*in */ BUFFER_ID_TYPE BUFFER_ID,
      /*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR, /* by reference */
      /*in */ MESSAGE_SIZE_TYPE LENGTH,
      /*in */ SYSTEM_TIME_TYPE TIME_OUT,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void RECEIVE_BUFFER(
      /*in */ BUFFER_ID_TYPE BUFFER_ID,
      /*in */ SYSTEM_TIME_TYPE TIME_OUT,
      /*out*/ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
      /*out*/ MESSAGE_SIZE_TYPE *LENGTH,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_BUFFER_ID(
      /*in */ BUFFER_NAME_TYPE BUFFER_NAME,
      /*out*/ BUFFER_ID_TYPE *BUFFER_ID,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_BUFFER_STATUS(
      /*in */ BUFFER_ID_TYPE BUFFER_ID,
      /*out*/ BUFFER_STATUS_TYPE *BUFFER_STATUS,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);

  //! \name BLACKBOARD Management Services
  static void CREATE_BLACKBOARD(
      /*in */ BLACKBOARD_NAME_TYPE BLACKBOARD_NAME,
      /*in */ MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
      /*out*/ BLACKBOARD_ID_TYPE *BLACKBOARD_ID,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void DISPLAY_BLACKBOARD(
      /*in */ BLACKBOARD_ID_TYPE BLACKBOARD_ID,
      /*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR, /* by reference */
      /*in */ MESSAGE_SIZE_TYPE LENGTH,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void READ_BLACKBOARD(
      /*in */ BLACKBOARD_ID_TYPE BLACKBOARD_ID,
      /*in */ SYSTEM_TIME_TYPE TIME_OUT,
      /*out*/ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
      /*out*/ MESSAGE_SIZE_TYPE *LENGTH,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void CLEAR_BLACKBOARD(
      /*in */ BLACKBOARD_ID_TYPE BLACKBOARD_ID,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_BLACKBOARD_ID(
      /*in */ BLACKBOARD_NAME_TYPE BLACKBOARD_NAME,
      /*out*/ BLACKBOARD_ID_TYPE *BLACKBOARD_ID,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_BLACKBOARD_STATUS(
      /*in */ BLACKBOARD_ID_TYPE BLACKBOARD_ID,
      /*out*/ BLACKBOARD_STATUS_TYPE *BLACKBOARD_STATUS,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);

  //! \name SAMPLING PORT Management Services
  static void CREATE_SAMPLING_PORT(
      /*in */ SAMPLING_PORT_NAME_TYPE SAMPLING_PORT_NAME,
      /*in */ MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
      /*in */ PORT_DIRECTION_TYPE PORT_DIRECTION,
      /*in */ SYSTEM_TIME_TYPE REFRESH_PERIOD,
      /*out*/ SAMPLING_PORT_ID_TYPE *SAMPLING_PORT_ID,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void WRITE_SAMPLING_MESSAGE(
      /*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
      /*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR, /* by reference */
      /*in */ MESSAGE_SIZE_TYPE LENGTH,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void READ_SAMPLING_MESSAGE(
      /*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
      /*out*/ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
      /*out*/ MESSAGE_SIZE_TYPE *LENGTH,
      /*out*/ VALIDITY_TYPE *VALIDITY,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_SAMPLING_PORT_ID(
      /*in */ SAMPLING_PORT_NAME_TYPE SAMPLING_PORT_NAME,
      /*out*/ SAMPLING_PORT_ID_TYPE *SAMPLING_PORT_ID,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_SAMPLING_PORT_STATUS(
      /*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
      /*out*/ SAMPLING_PORT_STATUS_TYPE *SAMPLING_PORT_STATUS,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);

  static void READ_CONTENTS_TO_DATA_BUFFER(
      SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID, MESSAGE_ADDR_TYPE msg_ptr,
      MESSAGE_SIZE_TYPE MAX_LENGTH, MESSAGE_SIZE_TYPE *READ_LENGTH,
      RETURN_CODE_TYPE *RETURN_CODE);
  static void WRITE_CONTENTS_FROM_DATA_BUFFER(
      SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID, MESSAGE_ADDR_TYPE msg_ptr,
      MESSAGE_SIZE_TYPE LENGTH, RETURN_CODE_TYPE *RETURN_CODE);

  //! \name QUEUING PORT Management Services
  static void CREATE_QUEUING_PORT(
      /*in */ QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME,
      /*in */ MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
      /*in */ MESSAGE_RANGE_TYPE MAX_NB_MESSAGE,
      /*in */ PORT_DIRECTION_TYPE PORT_DIRECTION,
      /*in */ QUEUING_DISCIPLINE_TYPE QUEUING_DISCIPLINE,
      /*out*/ QUEUING_PORT_ID_TYPE *QUEUING_PORT_ID,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void SEND_QUEUING_MESSAGE(
      /*in */ QUEUING_PORT_ID_TYPE QUEUING_PORT_ID,
      /*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR, /* by reference */
      /*in */ MESSAGE_SIZE_TYPE LENGTH,
      /*in */ SYSTEM_TIME_TYPE TIME_OUT,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void MESSAGE_QUEUE_SEND_MESSAGE_FROM_BUFFER(
      QUEUING_PORT_ID_TYPE QUEUING_PORT_ID, MESSAGE_ADDR_TYPE msg_ptr,
      MESSAGE_SIZE_TYPE LENGTH, SYSTEM_TIME_TYPE TIME_OUT,
      RETURN_CODE_TYPE *RETURN_CODE);
  static void RECEIVE_QUEUING_MESSAGE(
      /*in */ QUEUING_PORT_ID_TYPE QUEUING_PORT_ID,
      /*in */ SYSTEM_TIME_TYPE TIME_OUT,
      /*out*/ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
      /*out*/ MESSAGE_SIZE_TYPE *LENGTH,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_QUEUING_PORT_ID(
      /*in */ QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME,
      /*out*/ QUEUING_PORT_ID_TYPE *QUEUING_PORT_ID,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);
  static void GET_QUEUING_PORT_STATUS(
      /*in */ QUEUING_PORT_ID_TYPE QUEUING_PORT_ID,
      /*out*/ QUEUING_PORT_STATUS_TYPE *QUEUING_PORT_STATUS,
      /*out*/ RETURN_CODE_TYPE *RETURN_CODE);

  /// @cond IGNORE_DOXY

  ///*!Set Process Argument. This will set the corba request to the process
  ///identified by the id.
  // This request should return an INVALID_MODE if the process in the
  // READY_RUNNING state. This request is only valid for aperiodic processes.*/
  // static void SET_PROCESS_SERVER_REQUEST(
  //	/*!in */ PROCESS_ID_TYPE* id,
  //	/*!in */  void* arg,
  //	/*!out */ RETURN_CODE_TYPE* returncode);
  // static void GET_PROCESS_SERVER_REQUEST(
  //	/*!in */ PROCESS_ID_TYPE* id,
  //	/*!out */void** arg,
  //	/*!out */RETURN_CODE_TYPE* returncode);

  ///*!Set Parent Component.*/
  // static void SET_PROCESS_PARENT_COMPONENT(
  //	/*!in */ PROCESS_ID_TYPE* id,
  //	/*!in */ ACM_COMPONENT_TYPE* arg,
  //	/*!out */ RETURN_CODE_TYPE* returncode);

  // static void GET_PROCESS_PARENT_COMPONENT(
  //	/*!in */ PROCESS_ID_TYPE* id,
  //	/*!out */ ACM_COMPONENT_TYPE** arg,
  //	/*!out */ RETURN_CODE_TYPE* returncode);

  // static void SET_PROCESS_EXCEPTION(
  //	/*!in */ PROCESS_ID_TYPE* id,
  //	/*!in */ ACM_BASE_EXCEPTION_TYPE* arg,
  //	/*!out */ RETURN_CODE_TYPE* returncode);
  // static void GET_PROCESS_EXCEPTION(
  //	/*!in */ PROCESS_ID_TYPE* id,
  //	/*!out */ACM_BASE_EXCEPTION_TYPE** arg,
  //	/*!out */RETURN_CODE_TYPE* returncode);

  /// @endcond

}; // end of Apex class

} // ACM_USER
#endif //_APEX_h
