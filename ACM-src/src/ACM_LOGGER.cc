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
#ifndef _ACM_LOGGER_CC
#define _ACM_LOGGER_CC

#include <ACM/INTERNAL/ACM_THREAD_SUPPORT.h>
#include <ACM/INTERNAL/UDPSocket.h>
#include <ACM/USER/ACM_EXCEPTION.h>
#include <ACM/USER/ACM_LOGGER.h>
#include <fstream>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <vector>
#ifdef ISNETWORK_PARTITION
#include <ACM/USER/ACM_APEX_EXTENSIONS.h>
#include <ACM/USER/APEX_INTERFACE.h>
#else
#ifdef IS_MODULE_MANAGER
inline std::string GET_PARTITION_NAME() { return "MM"; }

#else
#include <ACM/USER/ACM_APEX_EXTENSIONS.h>
#include <ACM/USER/APEX_INTERFACE.h>
#endif
#endif

namespace ACM_USER {

const unsigned long LOGGER_BUFFER::LOG_ENTRY_SIZE = 255;
unsigned long LOGGER_BUFFER::MAX_LOG_ENTRIES = 0;
char **LOGGER_BUFFER::log_buffer = 0;
static ACM_INTERNAL::MUTEX LOG_MUTEX;
static unsigned int log_index = 0;
static bool log_overflow = false;
static int FLUSH_OR_NOT = 0;
#ifndef F6ACM
class REPORTER {
public:
  UDPSocket sendSocket;
  std::string hostname;
  static REPORTER *my_instance;
  unsigned int portnum;
  REPORTER(std::string &name, unsigned int &port);
  char buffer1[LOGGER_BUFFER::LOG_ENTRY_SIZE];
  std::string Partition_Name;
  static ACM_INTERNAL::MUTEX mymutex;

public:
  ~REPORTER() { LOGGER(APP, "Reported Terminated"); }
};

ACM_INTERNAL::MUTEX REPORTER::mymutex;
// Reporter Code

REPORTER *REPORTER::my_instance = 0;

REPORTER::REPORTER(std::string &name, unsigned int &port)
    : sendSocket(), hostname(name), portnum(port) {
  Partition_Name = GET_PARTITION_NAME();
  LOGGER(APP, "Reporter Created");
}
#endif

void REPORTER_INIT(std::string hostname, unsigned int portnumber,
                   ACM_USER::RETURN_CODE_TYPE *RETURN_CODE) {

#ifndef F6ACM
  ACM_INTERNAL::AUTO_LOCK lock(REPORTER::mymutex);
  *RETURN_CODE = NO_ERROR;

  if (REPORTER::my_instance != 0) {
    //*RETURN_CODE=NOT_AVAILABLE;
    return;
  }
  REPORTER::my_instance = new REPORTER(hostname, portnumber);
#endif
}
void REPORT(const char *message, ACM_USER::RETURN_CODE_TYPE *RETURN_CODE) {

#ifndef F6ACM
  SYSTEM_TIMESPEC actualtime = currentTime();
  long double sec = (actualtime.tv_sec);
  long double nsecs = (actualtime.tv_nsec);
  sec = sec + (nsecs / (long double)NANO);

  if (REPORTER::my_instance == 0) {
    *RETURN_CODE = NOT_AVAILABLE;
    LOGGER(ERROR, "Call Reporter Init first");

    return;
  }
  ACM_INTERNAL::AUTO_LOCK lock(REPORTER::mymutex);
  snprintf(REPORTER::my_instance->buffer1, LOGGER_BUFFER::LOG_ENTRY_SIZE,
           "%s|%0.9Lf|APP|%s", REPORTER::my_instance->Partition_Name.c_str(),
           sec, message);
  try {

    REPORTER::my_instance->sendSocket.sendTo(
        REPORTER::my_instance->buffer1,
        strlen(REPORTER::my_instance->buffer1) + 1,
        REPORTER::my_instance->hostname, REPORTER::my_instance->portnum);
  } catch (SocketException &e) {
    LOGGER(ERROR, "%s", e.what());
    *RETURN_CODE = NOT_AVAILABLE;
  }
#endif
}
void REPORTER_TERMINATE(ACM_USER::RETURN_CODE_TYPE *RETURN_CODE) {

#ifndef F6ACM
  *RETURN_CODE = NO_ERROR;
  ACM_INTERNAL::AUTO_LOCK lock(REPORTER::mymutex);
  if (REPORTER::my_instance == 0) {
    //*RETURN_CODE=NOT_AVAILABLE;

    return;
  }
  delete (REPORTER::my_instance);
  REPORTER::my_instance = 0;
#endif
}

#ifdef ENABLE_LOGGING_INFO
#ifndef ENABLE_LOGGING
#define ENABLE_LOGGING
static int CURRENT_LOGLEVEL = INFO;
#endif
#endif
#ifdef ENABLE_LOGGING_WARNING
#ifndef ENABLE_LOGGING
#define ENABLE_LOGGING
static int CURRENT_LOGLEVEL = WARNING;
#endif
#endif
#ifdef ENABLE_LOGGING_DEBUG
#ifndef ENABLE_LOGGING
#define ENABLE_LOGGING
static int CURRENT_LOGLEVEL = DEBUG;
#endif
#endif
#ifdef ENABLE_LOGGING_TRACE
#ifndef ENABLE_LOGGING
#define ENABLE_LOGGING
static int CURRENT_LOGLEVEL = TRACE;
#endif
#endif
#ifdef ENABLE_LOGGING_HMEVENT
#ifndef ENABLE_LOGGING
#define ENABLE_LOGGING
static int CURRENT_LOGLEVEL = HMEVENT;
#endif
#endif
#ifdef ENABLE_LOGGING_APP
#ifndef ENABLE_LOGGING
#define ENABLE_LOGGING
static int CURRENT_LOGLEVEL = APP;
#endif
#endif
#ifdef ENABLE_LOGGING_ERROR
#ifndef ENABLE_LOGGING
#define ENABLE_LOGGING
static int CURRENT_LOGLEVEL = ERROR;
#endif
#endif
#ifdef ENABLE_LOGGING_FATAL
#ifndef ENABLE_LOGGING
#define ENABLE_LOGGING
static int CURRENT_LOGLEVEL = FATAL;
#endif
#endif

void CONFIGLOG(LOG_LEVEL OUTPUT) {
#ifdef ENABLE_LOGGING
  CURRENT_LOGLEVEL = OUTPUT;
#endif
}

void LOG_INIT() {
#ifdef ENABLE_LOGGING
  if (LOGGER_BUFFER::log_buffer != 0) {
    printf("Logger entries already configured, ignoring the environment "
           "parameter\n");
    return;
  }
  char *entries = getenv("ACM_LOG_ENTRIES");
  if (entries != NULL) {
    long i = atol(entries);
    if (i > 1) {
      LOGGER_BUFFER::MAX_LOG_ENTRIES = i;
      LOGGER_BUFFER::log_buffer = (char **)malloc((sizeof(char *)) * i);
      for (long j = 0; j < i; j++) {
        LOGGER_BUFFER::log_buffer[j] = (char *)malloc(
            ((sizeof(char)) * LOGGER_BUFFER::LOG_ENTRY_SIZE) + 1);
      }
      printf("ACM_LOG_ENTRIES set  to %ld \n", i);
    } else {
      printf("ACM_LOG_ENTRIES set incorrectly to %s \n", entries);
    }
  } else {
    printf("ACM_LOG_ENTRIES not set \n");
  }

#endif
}

void FLUSHSTDOUT(bool input) {
  if (input)
    FLUSH_OR_NOT = 1;
}

void LOG2FILE() {
#ifdef ENABLE_LOGGING

  pid_t pid_data = syscall(SYS_gettid);

  if (LOGGER_BUFFER::MAX_LOG_ENTRIES < 1 || LOGGER_BUFFER::log_buffer == 0)
    return;

  char filename[60];
  snprintf(filename, 60, "%s-%d.log", GET_PARTITION_NAME().c_str(), pid_data);
  // std::string filename =toString(pid_data);
  // filename+=".log";
  std::ofstream myfile(filename);
  if (!myfile)
    return;

  /*printf("------------------------------------------------------------------------------------------------------------\n");
          printf("%11s|%20s|%7s|%s\n","PID:NAME","SECOND.NANO
     ","LEVEL","MESSAGE");
          printf("------------------------------------------------------------------------------------------------------------\n");*/

  unsigned int num_iterations =
      (log_overflow) ? LOGGER_BUFFER::MAX_LOG_ENTRIES : log_index;
  if (log_overflow)
    myfile << " Warning : log_overflow occured" << std::endl;
  for (unsigned int i = 0; i < num_iterations; i++) {
    myfile << LOGGER_BUFFER::log_buffer[i];
  }

  // log_buffer.clear();

  myfile.close();
  return;

#endif
}

void LOGGER_IDENT(char *NAME, LOG_LEVEL LOGLEVEL, const char *format, ...) {
#ifdef ENABLE_LOGGING
  // int level=LOGLEVEL;
  static pid_t pid_data = syscall(SYS_gettid);
  static std::string Partition_Name = GET_PARTITION_NAME();
  char buffer1[LOGGER_BUFFER::LOG_ENTRY_SIZE];
  char buffer2[LOGGER_BUFFER::LOG_ENTRY_SIZE];
  SYSTEM_TIMESPEC actualtime = currentTime();
  long double sec = (actualtime.tv_sec);
  long double nsecs = (actualtime.tv_nsec);
  sec = sec + (nsecs / (long double)NANO);
  switch (LOGLEVEL) {
  case INFO: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE,
             "%d:%s|%0.9Lf|INF|[From %s]", pid_data, Partition_Name.c_str(),
             sec, NAME);
  } break;

  case ERROR: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE,
             "%d:%s|%0.9Lf|ERR|[From %s]", pid_data, Partition_Name.c_str(),
             sec, NAME);
  } break;
  case DEBUG: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE,
             "%d:%s|%0.9Lf|DEB|[From %s]", pid_data, Partition_Name.c_str(),
             sec, NAME);
  } break;
  case FATAL: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE,
             "%d:%s|%0.9Lf|FTL|[From %s]", pid_data, Partition_Name.c_str(),
             sec, NAME);
  } break;
  case TRACE: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE,
             "%d:%s|%0.9Lf|TRC|[From %s]", pid_data, Partition_Name.c_str(),
             sec, NAME);
  } break;
  case HMEVENT: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE,
             "%d:%s|%0.9Lf|HME|[From %s]", pid_data, Partition_Name.c_str(),
             sec, NAME);
  } break;
  case APP: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE,
             "%d:%s|%0.9Lf|APP|[From %s]", pid_data, Partition_Name.c_str(),
             sec, NAME);
  } break;
  case USER: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE,
             "%d:%s|%0.9Lf|USR|[From %s]", pid_data, Partition_Name.c_str(),
             sec, NAME);
  } break;
  case WARNING: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE,
             "%d:%s|%0.9Lf|WRN|[From %s]", pid_data, Partition_Name.c_str(),
             sec, NAME);
  }
  }
  va_list args;
  va_start(args, format);
  vsnprintf(buffer1, LOGGER_BUFFER::LOG_ENTRY_SIZE, format, args);
  va_end(args);
  strcat(buffer2, buffer1);
  strcat(buffer2, "\n");

  if (LOGGER_BUFFER::MAX_LOG_ENTRIES > 1 && LOGGER_BUFFER::log_buffer != 0) {
    ACM_INTERNAL::AUTO_LOCK lock(LOG_MUTEX);
    strncpy(LOGGER_BUFFER::log_buffer[log_index++], buffer2,
            LOGGER_BUFFER::LOG_ENTRY_SIZE);
    if (log_index >= LOGGER_BUFFER::MAX_LOG_ENTRIES) {
      log_overflow = true;
      log_index = 0;
    }
  }

  if (int(LOGLEVEL) < CURRENT_LOGLEVEL)
    return;
  std::cout << buffer2;
  return;
#endif
}

void LOGGER(LOG_LEVEL LOGLEVEL, const char *format, ...) {
#ifdef ENABLE_LOGGING
  // int level=LOGLEVEL;
  static pid_t pid_data = syscall(SYS_gettid);
  static std::string Partition_Name = GET_PARTITION_NAME();
  char buffer1[LOGGER_BUFFER::LOG_ENTRY_SIZE];
  char buffer2[LOGGER_BUFFER::LOG_ENTRY_SIZE];
  SYSTEM_TIMESPEC actualtime = currentTime();
  long double sec = (actualtime.tv_sec);
  long double nsecs = (actualtime.tv_nsec);
  sec = sec + (nsecs / (long double)NANO);
  switch (LOGLEVEL) {
  case INFO: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE, "%d:%s|%0.9Lf|INF|",
             pid_data, Partition_Name.c_str(), sec);
  } break;

  case ERROR: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE, "%d:%s|%0.9Lf|ERR|",
             pid_data, Partition_Name.c_str(), sec);
  } break;
  case DEBUG: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE, "%d:%s|%0.9Lf|DEB|",
             pid_data, Partition_Name.c_str(), sec);
  } break;
  case FATAL: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE, "%d:%s|%0.9Lf|FTL|",
             pid_data, Partition_Name.c_str(), sec);
  } break;
  case TRACE: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE, "%d:%s|%0.9Lf|TRC|",
             pid_data, Partition_Name.c_str(), sec);
  } break;
  case HMEVENT: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE, "%d:%s|%0.9Lf|HME|",
             pid_data, Partition_Name.c_str(), sec);
  } break;
  case APP: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE, "%d:%s|%0.9Lf|APP|",
             pid_data, Partition_Name.c_str(), sec);
  } break;
  case USER: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE, "%d:%s|%0.9Lf|USR|",
             pid_data, Partition_Name.c_str(), sec);
  } break;
  case WARNING: {
    snprintf(buffer2, LOGGER_BUFFER::LOG_ENTRY_SIZE, "%d:%s|%0.9Lf|WRN|",
             pid_data, Partition_Name.c_str(), sec);
  }
  }
  va_list args;
  va_start(args, format);
  vsnprintf(buffer1, LOGGER_BUFFER::LOG_ENTRY_SIZE, format, args);
  va_end(args);
  strcat(buffer2, buffer1);
  strcat(buffer2, "\n");

  if (LOGGER_BUFFER::MAX_LOG_ENTRIES > 1 && LOGGER_BUFFER::log_buffer != 0) {
    ACM_INTERNAL::AUTO_LOCK lock(LOG_MUTEX);
    strncpy(LOGGER_BUFFER::log_buffer[log_index++], buffer2,
            LOGGER_BUFFER::LOGGER_BUFFER::LOG_ENTRY_SIZE);
    if (log_index >= LOGGER_BUFFER::MAX_LOG_ENTRIES) {
      log_overflow = true;
      log_index = 0;
    }
  }

  if (int(LOGLEVEL) < CURRENT_LOGLEVEL)
    return;
  std::cout << buffer2;
  if (FLUSH_OR_NOT >= 1)
    std::cout.flush();
  return;
#endif
}
}

#endif // _ACM_LOGGER_CC
