#ifndef ACM_TIMER_CC
#define ACM_TIMER_CC

#include <ACM/INTERNAL/ACM_THREAD_SUPPORT.h>
#include <ACM/INTERNAL/EVENT.h>
#include <ACM/USER/ACM_TIMER.h>

using namespace ACM_INTERNAL;
namespace ACM_HELPERS {
void ACM_TIMER::run() {
  LOGGER(TRACE, "Inside run of thread ********");
  // printf("Event state from inside thread is
  // %d\n",internalevent_.EVENT_STATE);
  // Need a Semaphore
  while (true) {
    LOGGER(TRACE, "waiting for semaphore Inside run of thread ********");
    internalsem_.wait();
    {
      my_state = WORKING;
      // bool invoke = false;
      LOGGER(TRACE, "Running Timer");
      // printf("I am here in the run **********\n");
      RETURN_CODE_TYPE return_code = NO_ERROR;
      internalMUTEX.lock();
      while (true) {
        if (TimeValues.size() == 0) {
          internalevent_.WAIT_EVENT(&return_code);
        } else {
          std::set<SYSTEM_TIMESPEC, SYSTEM_TIMESPEC_COMP>::iterator iter =
              TimeValues.begin();
          SYSTEM_TIMESPEC current = *iter;
          TimeValues.erase(iter);
          internalevent_.WAIT_EVENT(current, &return_code);
        }
        if (return_code != TIMED_OUT) {
          LOGGER(TRACE, "Out of Wait Event Without A time out");
          break;
        }
        LOGGER(TRACE, "TIMED OUT");
        if (ENTRY_POINT != NULL) {
          (*ENTRY_POINT)();
        }
      }
      internalMUTEX.unlock();
      my_state = STOPPED;
    }
  }
}

void ACM_TIMER::addTimeValue(SYSTEM_TIMESPEC myspec,
                             RETURN_CODE_TYPE *return_code) {
  *return_code = NO_ERROR;
  if (my_state == WORKING) {
    LOGGER(TRACE, "Timer is working.");
    *return_code = NOT_AVAILABLE;
    return;
  }
  internalevent_.RESET_EVENT(return_code);

  if (*return_code == NO_ERROR) {
    LOGGER(TRACE, "reset event successful in addtimevalue");

  } else {
    LOGGER(TRACE, "reset event NOT successful in addtimevalue");
    *return_code = NOT_AVAILABLE;
    return;
  }
  AUTO_LOCK my(internalMUTEX);
  // printf("Event state from inside addTimeValue is
  // %d\n",internalevent_.EVENT_STATE);
  if (internalevent_.EVENT_STATE != DOWN) {
    LOGGER(TRACE, "internalevent_.EVENT_STATE!=DOWN");
    *return_code = NOT_AVAILABLE;
    // internalMUTEX.unlock();
    return;
  }
  TimeValues.insert(myspec);
  LOGGER(TRACE, "Timer: Size of set is %d", TimeValues.size());
  // internalMUTEX.unlock();
}

void ACM_TIMER::removeTimeValue(SYSTEM_TIMESPEC myspec,
                                RETURN_CODE_TYPE *return_code) {
  *return_code = NO_ERROR;
  if (my_state == WORKING) {
    LOGGER(TRACE, "Timer is working.");
    *return_code = NOT_AVAILABLE;
    return;
  }
  AUTO_LOCK my(internalMUTEX);
}

void ACM_TIMER::removeAllTimeValues(RETURN_CODE_TYPE *return_code) {
  *return_code = NO_ERROR;
  if (my_state == WORKING) {
    LOGGER(TRACE, "Timer is working.");
    *return_code = NOT_AVAILABLE;
    return;
  }
  AUTO_LOCK my(internalMUTEX);

  this->TimeValues.clear();
}

void ACM_TIMER::stopTimer(RETURN_CODE_TYPE *return_code) {
  *return_code = NO_ERROR;

  internalevent_.SET_EVENT(return_code);

  AUTO_LOCK my(internalMUTEX);

  LOGGER(TRACE, "Timer Stop Successful");
}

void ACM_TIMER::startTimer(RETURN_CODE_TYPE *return_code) {
  LOGGER(TRACE, "**********startTimer Called");

  if (my_state == WORKING) {
    LOGGER(TRACE, "Timer is working.");
    *return_code = NOT_AVAILABLE;
    return;
  }

  internalMUTEX.lock();
  *return_code = NO_ERROR;
  internalevent_.RESET_EVENT(return_code);
  // assert(*return_code==NO_ERROR);
  *return_code = internalsem_.post();
  internalMUTEX.unlock();
}

void ACM_TIMER::wait(void **exitval) { pthread_join(this->id, exitval); }
void ACM_TIMER::terminate() {
  int result = pthread_cancel(this->id);
  assert(!result);
}

int ACM_TIMER::create_thread() {
  if (pthread_create(&id, NULL, Thread_wrapper, this) != 0) {
    return -1;
  }
  return 0;
}
void *ACM_TIMER::Thread_wrapper(void *arg) {
  static_cast<ACM_TIMER *>(arg)->run();
  return NULL;
}

long ACM_TIMER::get_priority() {
  int policy = -1;
  struct sched_param sp_Thread;
  // int retVal = -99;
  /*if (this->thread_id()<0){
  return retVal;
  }*/
  pthread_getschedparam(this->id, &policy, &sp_Thread);
  return sp_Thread.sched_priority;
}
void ACM_TIMER::set_priority(long priority) {
  struct sched_param sp_Thread;
  int policy = -1;
  pthread_getschedparam(this->id, &policy, &sp_Thread);
  sp_Thread.sched_priority = priority;
  int schedRetValue = 99;
  schedRetValue = pthread_setschedparam(this->id, policy, &sp_Thread);
}
ACM_TIMER::~ACM_TIMER() {
  this->terminate();
  this->wait();
}

ACM_TIMER::ACM_TIMER(const ACM_TIMER &sock)
    : internalevent_(sock.name),
      internalsem_(SEMAPHORE_VALUE_TYPE(0), SEMAPHORE_VALUE_TYPE(1)),
      my_state(STOPPED) {}
void ACM_TIMER::operator=(const ACM_TIMER &sock) {}

ACM_TIMER::ACM_TIMER(std::string name, callback_type callback)
    : name(name), internalevent_(name),
      internalsem_(SEMAPHORE_VALUE_TYPE(0), SEMAPHORE_VALUE_TYPE(1)),
      ENTRY_POINT(callback), my_state(STOPPED) // will lock this
{

  int result = this->create_thread();
  if (result != 0) {
    char buffer[100];
#ifndef WIN32
    snprintf(buffer, 100, "Problem: Return From create_thread: code is %d",
             result);
#else
    _snprintf(buffer, 100, "Problem: Return From create_thread: code is %d",
              result);
#endif
    LOGGER(ERROR, "Problem: Return From create_thread: code is %d", result);
    assert(result == 0);
  }
}
}

#endif