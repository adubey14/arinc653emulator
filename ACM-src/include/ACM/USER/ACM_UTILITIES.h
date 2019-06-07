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

#ifndef ACM_UTILITIES_H
#define ACM_UTILITIES_H

#ifndef WIN32
#include <sys/time.h>
#else
#include <SYS\Timeb.h>
#endif
#include <pthread.h>

#ifndef WIN32
#include <sys/syscall.h>
#endif

#include <assert.h>
#include <iostream>
#include <sstream>
//#include </usr/include/asm-generic/errno.h>
#include <list>

#include <ACM/USER/ACM_LOGGER.h>
#include <ACM/USER/APEX_TYPES.h>
#include <algorithm>
#include <errno.h>
#include <map>
#include <math.h>
#include <stdio.h>
#include <string>
#include <vector>
#ifndef WIN32
#include <signal.h>
#include <unistd.h>
#endif

namespace ACM_USER {
#define POSIX_Q_MESSAGE_DEFAULT_PRIORITY 0 // for POSIX message queues
#define SYSTEM_TIMESPEC struct timespec
/**********************************/
/* Time Support Utilities		  */
/**********************************/
inline SYSTEM_TIMESPEC currentTime() {
  SYSTEM_TIMESPEC tv;
#ifndef WIN32
  clock_gettime(CLOCK_REALTIME, &tv);
#else
  struct _timeb currSysTime;
  long long nanosecs, secs;

  const long long NANOSEC_PER_MILLISEC = 1000000;
  const long long NANOSEC_PER_SEC = 1000000000;

  /* get current system time and add millisecs */
  _ftime(&currSysTime);
  secs = (long long)(currSysTime.time);
  nanosecs = ((long long)(currSysTime.millitm)) * NANOSEC_PER_MILLISEC;
  if (nanosecs >= NANOSEC_PER_SEC) {
    secs++;
    nanosecs -= NANOSEC_PER_SEC;
  } else if (nanosecs < 0) {
    secs--;
    nanosecs += NANOSEC_PER_SEC;
  }

  tv.tv_nsec = (long)nanosecs;
  tv.tv_sec = (long)secs;
#endif
  return tv;
}

#ifndef WIN32
inline void ACM_EXIT() {
  LOGGER(ACM_USER::DEBUG, "CALLING EXIT");
  raise(SIGTERM);
}

#define ACM_ASSERT(x)                                                          \
  if (!(x)) {                                                                  \
    ACM_USER::LOGGER(ACM_USER::FATAL,                                          \
                     "Error in program %s at line %d. Exiting Now", __FILE__,  \
                     __LINE__);                                                \
    ACM_EXIT();                                                                \
  }
#endif

inline void set_self_priority(long priority) {
  struct sched_param sp_Thread;
  int policy;
  pthread_t id = pthread_self();
  pthread_getschedparam(id, &policy, &sp_Thread);
  LOGGER(DEBUG, "Scheduling Policy of thread %d is %s", id,
         (policy == SCHED_FIFO) ? "SCHED_FIFO" : (policy == SCHED_RR)
                                                     ? "SCHED_RR"
                                                     : (policy == SCHED_OTHER)
                                                           ? "SCHED_OTHER"
                                                           : "???");

  sp_Thread.sched_priority = priority;
  //		int schedRetValue = 99;
  // checking which thread's priority to change
  // schedRetValue =
  pthread_setschedparam(id, policy, &sp_Thread);
}
inline SYSTEM_TIME_TYPE convertTimeSpecToNano(const SYSTEM_TIMESPEC &tv) {
  SYSTEM_TIME_TYPE time =
      (SYSTEM_TIME_TYPE)tv.tv_sec * NANO + (SYSTEM_TIME_TYPE)tv.tv_nsec;
  return time;
}

/*	inline long long Unique_Integer(){
static long long initial_value=0;
static MUTEX myMUTEX;
ACM::AUTO_LOCK lock (myMUTEX);
return(++initial_value);
}
*/

inline long double currentTimeInSeconds() {
  SYSTEM_TIMESPEC tv = currentTime();
  // clock_gettime(CLOCK_REALTIME , &tv);
  long double time = (long double)tv.tv_sec + (long double)tv.tv_nsec / NANO;
  return time;
}

inline SYSTEM_TIME_TYPE currentTimeInNanoSeconds() {
  SYSTEM_TIMESPEC tv = currentTime();
  // clock_gettime(CLOCK_REALTIME , &tv);
  SYSTEM_TIME_TYPE time =
      (SYSTEM_TIME_TYPE)tv.tv_sec * NANO + (SYSTEM_TIME_TYPE)tv.tv_nsec;
  return time;
}

inline SYSTEM_TIMESPEC addTime(const SYSTEM_TIMESPEC &first,
                               const SYSTEM_TIMESPEC &second) {
  SYSTEM_TIMESPEC tv;
  long long nsecs = first.tv_nsec + second.tv_nsec;
  long long secs = first.tv_sec + second.tv_sec;
  while (nsecs >= NANO) {
    secs = secs + 1;
    nsecs = nsecs - NANO;
  }
  tv.tv_nsec = nsecs;
  tv.tv_sec = secs;
  return tv;
}
inline SYSTEM_TIMESPEC operator+(const SYSTEM_TIMESPEC &first,
                                 const SYSTEM_TIMESPEC &second) {
  return addTime(first, second);
}

inline SYSTEM_TIMESPEC addTimeNano(const SYSTEM_TIMESPEC &first,
                                   const SYSTEM_TIME_TYPE &nanoseconds) {

#ifndef WIN32
  // printf("%lld nsec, %lld max time out\n",nanoseconds,
  // (SYSTEM_TIME_TYPE)MAX_TIME_OUT);
  bool value;
  if (nanoseconds <= MAX_TIME_OUT)
    value = true;
  else
    value = false;

  ACM_ASSERT(value);
// ACM_ASSERT(nanoseconds<=(SYSTEM_TIME_TYPE)MAX_TIME_OUT);
//	assert(nanoseconds<=(SYSTEM_TIME_TYPE)MAX_TIME_OUT);
#else
  assert(nanoseconds <= (SYSTEM_TIME_TYPE)MAX_TIME_OUT);
#endif
  if (nanoseconds == INFINITE_TIME_VALUE) {
    SYSTEM_TIMESPEC tv;
    long long nsecs = first.tv_nsec + (SYSTEM_TIME_TYPE)MAX_TIME_OUT;
    long long secs = nsecs / (long long)NANO;
    secs += first.tv_sec;
    nsecs = nsecs % (long long)NANO;
    tv.tv_sec = secs;
    tv.tv_nsec = nsecs;
    return tv;

  } else {
    SYSTEM_TIMESPEC tv;
    long long nsecs = first.tv_nsec + nanoseconds;
    long long secs = nsecs / (long long)NANO;
    secs += first.tv_sec;
    nsecs = nsecs % (long long)NANO;
    tv.tv_sec = secs;
    tv.tv_nsec = nsecs;
    return tv;
  }
}
inline SYSTEM_TIMESPEC operator+(const SYSTEM_TIMESPEC &first,
                                 const SYSTEM_TIME_TYPE &nanoseconds) {
  return addTimeNano(first, nanoseconds);
}

#ifndef WIN32
inline void HELPER_CLOCK_NANOSLEEP(SYSTEM_TIME_TYPE TIME_OUT) {
  SYSTEM_TIMESPEC actualtime = currentTime();

  SYSTEM_TIMESPEC absolute_time = addTimeNano(actualtime, TIME_OUT);

  int clockresult = 1;
  // you can spuriously get out of nanosleep
  struct timespec rem = {0, 0};
  while (clockresult != 0) {
    // printf("inthe sleep loop\n");
    clockresult =
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &absolute_time, &rem);
  }
  // SYSTEM_TIMESPEC actualtime1 =currentTime();

  // printf("start time %ld.%ld\n",actualtime.tv_sec,actualtime.tv_nsec);
  // printf("supposed finish time
  // %ld.%ld\n",absolute_time.tv_sec,absolute_time.tv_nsec);
  // printf("actual finish time
  // %ld.%ld\n",actualtime1.tv_sec,actualtime1.tv_nsec);
}
#endif
inline SYSTEM_TIMESPEC computeDeadlineTimeSpecs(const SYSTEM_TIMESPEC &first,
                                                SYSTEM_TIME_TYPE &nanoseconds) {
#ifndef WIN32

  bool value;
  if (nanoseconds <= MAX_TIME_OUT)
    value = true;
  else
    value = false;

  ACM_ASSERT(value);

// ACM_ASSERT(nanoseconds<=(SYSTEM_TIME_TYPE)MAX_TIME_OUT);
#else
  assert(nanoseconds <= (SYSTEM_TIME_TYPE)MAX_TIME_OUT);
#endif
  if (nanoseconds == INFINITE_TIME_VALUE) {
    SYSTEM_TIME_TYPE t = (SYSTEM_TIME_TYPE)MAX_TIME_OUT;
    return addTimeNano(first, t);
  }

  return addTimeNano(first, nanoseconds);
}

inline SYSTEM_TIMESPEC addTimeMilli(const SYSTEM_TIMESPEC &first,
                                    long &milliseconds) {
  // it will call the ont with long long nanoseconds, but it's cleaner than
  // calling something from the code in form wcet * 1000000;
  long long nanoseconds = milliseconds * MICRO;
  return addTimeNano(first, nanoseconds);
}

/*!
This function subtracts second from first
*/
inline SYSTEM_TIMESPEC subtractTime(const SYSTEM_TIMESPEC &first,
                                    const SYSTEM_TIMESPEC &second) {
  SYSTEM_TIMESPEC tv;
  long nsecs = first.tv_nsec - second.tv_nsec;
  long secs = first.tv_sec - second.tv_sec;
  while (nsecs < 0) {
    secs--;
    nsecs = nsecs + NANO;
  }
  tv.tv_nsec = nsecs;
  tv.tv_sec = secs;
  return tv;
}

inline long long subtractTime2(const SYSTEM_TIMESPEC &first,
                               const SYSTEM_TIMESPEC &second) {
  SYSTEM_TIMESPEC tv = subtractTime(first, second);
  long long ret = tv.tv_nsec;
  ret += (long long)tv.tv_sec * NANO;
  return ret;
}

inline SYSTEM_TIMESPEC subtractTimeNano(const SYSTEM_TIMESPEC &first,
                                        long long &nanoseconds) {

#ifndef WIN32
  bool value;
  if (nanoseconds <= MAX_TIME_OUT)
    value = true;
  else
    value = false;

  ACM_ASSERT(value);

// ACM_ASSERT(nanoseconds<=(SYSTEM_TIME_TYPE)MAX_TIME_OUT);
#else
  assert(nanoseconds <= (SYSTEM_TIME_TYPE)MAX_TIME_OUT);
#endif

  SYSTEM_TIMESPEC tv;
  long nsecs = first.tv_nsec - nanoseconds;
  long secs = first.tv_sec;
  while (nsecs < 0) {
    secs--;
    nsecs = nsecs + NANO;
  }
  tv.tv_nsec = nsecs;
  tv.tv_sec = secs;
  return tv;
}
inline SYSTEM_TIMESPEC subtractTimeMilli(const SYSTEM_TIMESPEC &first,
                                         long &milliseconds) {
  long long nanoseconds = milliseconds * MICRO;
  return subtractTimeNano(first, nanoseconds);
}

inline bool compareTime(const SYSTEM_TIMESPEC &first,
                        const SYSTEM_TIMESPEC &second) {
  if (first.tv_sec < second.tv_sec)
    return true;

  if (first.tv_sec > second.tv_sec)
    return false;

  if (first.tv_nsec <= second.tv_nsec)
    return true;

  return false;
}

inline bool compareTimeLess(const SYSTEM_TIMESPEC &first,
                            const SYSTEM_TIMESPEC &second) {
  if (first.tv_sec < second.tv_sec)
    return true;

  if (first.tv_sec > second.tv_sec)
    return false;

  if (first.tv_nsec < second.tv_nsec)
    return true;

  return false;
}

/*! Conversion toString
*/
// template <class T>
// std::string toString(T message){
//	 //std::cout<<"Here is "<<message<<"\n";
//	std::stringstream temp;
//	temp<<message;
//	//std::cout<<temp.str()<<"\n";
//	return temp.str();
//}

template <class T>
inline bool from_string(T &t, const std::string &s,
                        std::ios_base &(*f)(std::ios_base &)) {
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}

inline void stringTrim(std::string &str, std::string delims) {
  std::string::size_type pos = str.find_last_not_of(delims);
  if (pos != std::string::npos) {
    str.erase(pos + 1);
    pos = str.find_first_not_of(delims);
    if (pos != std::string::npos)
      str.erase(0, pos);
  } else
    str.erase(str.begin(), str.end());
}

inline void stringTokenize(const std::string &str,
                           std::vector<std::string> &tokens,
                           std::string delimiters) {
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos) {
    // Found a token, add it to the vector.
    std::string substring = str.substr(lastPos, pos - lastPos);
    // LOGGER("SUBSTRING is %s",substring.c_str());
    stringTrim(substring, " \t\r\n\0");
    // std::cout<<substring;
    tokens.push_back(substring);
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

/*
template <>
inline bool from_string<double>(double& t,
        const std::string& s,
        std::ios_base& (*f)(std::ios_base&))
{
        //std::cout<<"in double"<<std::endl;
        bool retval =true;
        {
                std::istringstream iss(s);
                retval = !(iss >> f >> t).fail();
        }

        if(!retval)
                return retval;

        std::vector<std::string>parts;
        stringTokenize(s,parts,".");

        if(parts.size()==1)
        {
                std::string temp=parts[0];
                parts[0]="0";
                parts.push_back(temp);
        }

        if(parts.size()!=2   ) //an integer without decimal place was passed
                return false;

        int length=parts[1].size();

        if(length<=9)
        {
                int numzeros=9-length;
                for (int j=0;j<numzeros;j++)
                        parts[1].append("0");

                parts[0].append(parts[1]);
        }
        else
        {
                for (int j=0;j<9;j++)
                {
                        std::string t_1 =parts[1].substr(j,1);
                        parts[0].append(t_1);
                }
        }

        //std::cout<<parts[0]<<std::endl;

        long long tempnanoval;
        {
                retval=true;
                std::istringstream iss(parts[0]);
                retval = !(iss >> f >> tempnanoval).fail();
                if (!retval) return retval;
        }

        long double tempdouble =tempnanoval;
        tempdouble=tempdouble/1e9;
        t=(double)tempdouble;
        //std::cout<<"converted using nano\n";
        return retval;
}
*/
#ifndef WIN32
inline void SET_CPU_AFFINITY(int cpunumber) {
  cpu_set_t mask;
  CPU_ZERO(&mask);           /* clear mask */
  CPU_SET(cpunumber, &mask); /* processors 0 */

  unsigned int len = sizeof(mask);
  if (sched_setaffinity(0, len, &mask) < 0) {
    LOGGER(FATAL, "Error when trying to set CPU affinity: %s", strerror(errno));
    throw(-1);
  }
}

inline void Send_Synchronization_To_MM() {
  struct sched_param sp_Process;
  // after setting FIFO scheduling, get controllers ABSOLUTE priority
  if (sched_getparam(getppid(), &sp_Process) ==
      0) { // PartitionManager has 99 ACM_SCHED_POLICY priority
    if (sp_Process.sched_priority == PARTITION_MANAGER_PRIORITY) {
      // parent seems to be PartitionManager
      // set senddignal to true
      kill(getppid(), SIGUSR1);
      LOGGER(ACM_USER::DEBUG, "Sent sigusr1 to MM");
      raise(SIGSTOP);
      LOGGER(ACM_USER::DEBUG, "Received SIG Continue from MM");
    }
  }
}

#endif

inline void median(std::vector<double> &data, double &output) {
  if (data.size() == 0)
    return;
  std::sort(data.begin(), data.end());
  int len = data.size();
  int index = len / 2;
  if (len % 2 == 1) {
    output = data[index];
    return;
  } else {
    output = 0.5 * (data[index] + data[index - 1]);
    return;
  }
}

inline void median(std::vector<int> &data, double &output) {
  if (data.size() == 0)
    return;
  std::sort(data.begin(), data.end());
  int len = data.size();
  int index = len / 2;
  if (len % 2 == 1) {
    output = data[index];
    return;
  } else {
    output = 0.5 * (double)(data[index] + data[index - 1]);
    return;
  }
}

#ifdef OLD
inline SYSTEM_TIME_TYPE convertSecsToNano(double secs) {
  // std::cout<<secs<<"\n";
  SYSTEM_TIME_TYPE result = 0;
  result = NANO;
  result = (SYSTEM_TIME_TYPE)(NANO * secs);
  return result;
}
#else
inline SYSTEM_TIME_TYPE convertSecsToNano(double DURATION) {
  long double DURATION_d = DURATION;
  DURATION_d = DURATION_d * 1e9; /// Converting to NanoSeconds
  LOGGER(DEBUG, "In convertSecsToNano Input %Lf:\n", DURATION_d);
  char buffer[100];
  snprintf(buffer, 100, "%Lf", DURATION_d);
  std::string bufferstr(buffer);
  // LOGGER(DEBUG,"buffer %s\n",bufferstr.c_str());
  std::string outputval;
  std::vector<std::string> tempbufferparts;
  stringTokenize(bufferstr, tempbufferparts, ".");
  if (tempbufferparts.size() == 0)
    outputval = bufferstr;
  else
    outputval = tempbufferparts[0];
  // printf("%s\n",outputval.c_str());
  SYSTEM_TIME_TYPE temp_dur = atoll(outputval.c_str());
  LOGGER(DEBUG, "From convert to nsec returning %lld\n", temp_dur);
  return temp_dur;
}
#endif
}

#endif // ACM_UTILITIES_H
