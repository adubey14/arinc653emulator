#ifndef _MODULE_MANAGER_CC
#define _MODULE_MANAGER_CC
#include <ACM/INTERNAL/ModuleManager.h>
#include <cctype>
#include <iostream>
#include <iterator>
#include <sstream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

//#include "ModuleConfigurationDefinitions.cc"

#include <algorithm>
#ifdef ISNETWORK_PARTITION
#include <ACM/INTERNAL/NetworkPartition.h>
extern NETWORK_PARTITION *net_partition;
#endif

namespace ACM_INTERNAL {
MODULE_MANAGER *MODULE_MANAGER::_myInstance = 0;
std::string MODULE_MANAGER::fileName = "file.cfg";

bool fexists(const char *filename) {
  ifstream ifile;
  ifile.open(filename);
  bool return_flag = ifile.is_open();
  if (return_flag)
    ifile.close();
  return return_flag;
}
};

#include "ModuleInitialization.cc"
#include "ModulePartitionCreation.cc"
#include "ModuleScheduler.cc"
#include "ModuleSignalHandlers.cc"
#include "MultiModuleRelatedConfig.cc"

namespace ACM_INTERNAL {

MODULE_MANAGER::MODULE_MANAGER()
    : DISCOVERY_PROTOCOL(IOR), NS_PORT_NUMBER(0), cpunumber(0),
      DUMMY_PROCESS_ID(0), MAX_NUMBER_OF_FRAMES(-1),
      isMAX_NUMBER_OF_FRAMES_SET(false), MIN_WAIT_TIME(500),
      isMIN_WAIT_TIME_SET(false), NumberOfInitStages(1), PARENT_NAME(""),
      PARENT_HOST(""), PARENT_PORT(0),
      CONFIGURATION_PORT_NUM(CONFIGURATION_PORT), isSystemModuleManager(false),
      isBroadcast(false), isMulticast(false),
      isrunningUnderSystemModuleManager(false),

      Recv_Socket(0), Send_Socket(0), AVE_NETWORK_LATENCY_FROM_PARENT(0),
      SocketsInitialized(false)

{

  INIT_PARTITION_PROPERTY_MAP(MODULE_MANAGER::fileName);
#ifndef ISNETWORK_PARTITION

  pid_t pid_data = syscall(SYS_gettid);
  LOG_INIT();

  snprintf(killfilename, 30, "cleanupMM-%d.sh", pid_data);
  killfile.open(killfilename);
  // chmod(killfilename,S_IRWXU);

  if (killfile.good() && killfile.is_open()) {
    pid_t pid_data = syscall(SYS_gettid);

    killfile << "ps -ef |grep ModuleManager | grep " << pid_data
             << " && echo \"kill -9 " << pid_data << " \" &&  kill -9 "
             << pid_data << std::endl;
    killfile << "mkdir -p /dev/mqueue" << std::endl;
    killfile << "(! mount |grep mqueue > /dev/null) && mount -t mqueue none "
                "/dev/mqueue"
             << std::endl;
    killfile.flush();
  }

  SET_MAJOR_FRAME_SIZE();
  SET_MAX_NUMBER_OF_FRAMES();
  READ_NUMBER_OF_INIT_STAGES();
  SET_MIN_WAIT_TIME();
  this->PREPARE_PARENT_CHILD_INFORMATION();
  SET_DISCOVERY_PROTOCOL();
  SET_CPU_NUMBER();
  MODULE_MANAGER::SET_FIFO_POLICY();
  CREATE_PARTITION_VECTOR();

  if (PARTITION_VECTOR.size() > MAX_PARTITION_NUM) {
    LOGGER(FATAL, "PARTITION_VECTOR.size()>MAX_PARTITION_NUM %d",
           MAX_PARTITION_NUM);
    throw(-6);
  }
  std::sort(SCHEDULING_VECTOR.begin(), SCHEDULING_VECTOR.end(),
            MODULE_MANAGER::SCHEDULE_COMP);

  // check schedule
  for (unsigned int i = 0; i < SCHEDULING_VECTOR.size() - 1; i++) {
    if (SCHEDULING_VECTOR[i].OFFSET + SCHEDULING_VECTOR[i].DURATION >
        SCHEDULING_VECTOR[i + 1].OFFSET) {
      LOGGER(
          FATAL, "WRONG SCHEDULE: Partition %s Offset %lld is greater than "
                 "previous Partition %s's Offset %lld + Duration %lld",
          PARTITION_VECTOR[SCHEDULING_VECTOR[i + 1].PARTITION_INDEX]
              .Name.c_str(),
          SCHEDULING_VECTOR[i + 1].OFFSET,
          PARTITION_VECTOR[SCHEDULING_VECTOR[i].PARTITION_INDEX].Name.c_str(),
          SCHEDULING_VECTOR[i].OFFSET, SCHEDULING_VECTOR[i].DURATION);

      throw(-5);
    }
  }

  if (SCHEDULING_VECTOR[SCHEDULING_VECTOR.size() - 1].OFFSET +
          SCHEDULING_VECTOR[SCHEDULING_VECTOR.size() - 1].DURATION >
      this->MAJOR_FRAME_SIZE) {
    LOGGER(FATAL, "WRONG SCHEDULE: Hyperperiod %lld is smaller  than last "
                  "Partition %s's Offset %lld + Duration %lld",
           this->MAJOR_FRAME_SIZE,
           PARTITION_VECTOR[SCHEDULING_VECTOR[SCHEDULING_VECTOR.size() - 1]
                                .PARTITION_INDEX]
               .Name.c_str(),
           SCHEDULING_VECTOR[SCHEDULING_VECTOR.size() - 1].OFFSET,
           SCHEDULING_VECTOR[SCHEDULING_VECTOR.size() - 1].DURATION);
    throw(-5);
  }

  LOGGER(INFO, "CREATED %d partitions", this->PARTITION_VECTOR.size());
  for (unsigned int i = 0; i < PARTITION_VECTOR.size(); i++)
    PARTITION_VECTOR[i].printDescriptor();
  for (unsigned int i = 0; i < SCHEDULING_VECTOR.size(); i++)
    SCHEDULING_VECTOR[i].printDescriptor();

#endif
}

MODULE_MANAGER::~MODULE_MANAGER() {
#ifndef ISNETWORK_PARTITION
  if (killfile.good() && killfile.is_open()) {
    LOGGER(APP, "Cleanup File Closed");
    killfile.close();
    // chmod(killfilename,S_IRWXU);
  }
#endif
  LOGGER(INFO, "STOPPING Module Manager");
  PARTITION_VECTOR.clear();

  for (std::map<std::string, QUEUE_CHANNEL *>::iterator iter =
           MAP_OF_QUEUEING_CHANNELS.begin();
       iter != MAP_OF_QUEUEING_CHANNELS.end(); iter++) {
    if (iter->second != 0)
      delete (iter->second);
  }

  MAP_OF_QUEUEING_CHANNELS.clear();

  for (std::map<std::string, SAMPLING_CHANNEL *>::iterator iter =
           MAP_OF_SAMPLING_CHANNELS.begin();
       iter != MAP_OF_SAMPLING_CHANNELS.end(); iter++) {
    if (iter->second != 0)
      delete (iter->second);
  }
  MAP_OF_SAMPLING_CHANNELS.clear();

  for (std::map<std::string, MESSAGE_QUEUE_TYPE *>::iterator iter =
           MAP_OF_CONFIGURATION_QUEUE.begin();
       iter != MAP_OF_CONFIGURATION_QUEUE.end(); iter++) {
    if (iter->second != 0)
      delete (iter->second);
  }

  if (Recv_Socket != 0)
    delete (Recv_Socket);
  if (Send_Socket != 0)
    delete (Send_Socket);
}
MODULE_MANAGER *MODULE_MANAGER::Instance() {
  if (_myInstance == 0)
    _myInstance = new MODULE_MANAGER();

  return _myInstance;
}
void MODULE_MANAGER::DOSHUTDOWN_BEFORE_INIT() {

  ACM_INTERNAL::MODULE_MANAGER *partManager =
      ACM_INTERNAL::MODULE_MANAGER::Instance();
  if (partManager->DUMMY_PROCESS_ID != 0) {
    LOGGER(HMEVENT, "Terminating Dummy Process");
    kill(partManager->DUMMY_PROCESS_ID, SIGTERM);
  }

  for (unsigned int i = 0; i < partManager->PARTITION_VECTOR.size(); i++) {
    kill(partManager->PARTITION_VECTOR[i].PID, SIGSTOP);
  }

  for (unsigned int i = 0; i < partManager->PARTITION_VECTOR.size(); i++) {
    if (partManager->PARTITION_VECTOR[i].PID == 0)
      continue;
    LOGGER(HMEVENT, "SENDING MESSAGE TO %s (%d)",
           partManager->PARTITION_VECTOR[i].Name.c_str(),
           partManager->PARTITION_VECTOR[i].PID);
    // Raise the priority...
    // CONTROLLER_PROCESS_PRIORITY +1;
    struct sched_param sp_Controller;
    sched_getparam(partManager->PARTITION_VECTOR[i].PID, &sp_Controller);
    sp_Controller.sched_priority = CONTROLLER_PROCESS_PRIORITY + 1;
    sched_setparam(partManager->PARTITION_VECTOR[i].PID, &sp_Controller);
    kill(partManager->PARTITION_VECTOR[i].PID, SIGCONT);
    kill(partManager->PARTITION_VECTOR[i].PID, SIGTERM);
    // HELPER_CLOCK_NANOSLEEP(convertSecsToNano(10));
    waitpid(partManager->PARTITION_VECTOR[i].PID, NULL, 0);
    LOGGER(HMEVENT, "%s (%d) killed.",
           partManager->PARTITION_VECTOR[i].Name.c_str(),
           partManager->PARTITION_VECTOR[i].PID);
  }

  LOGGER(HMEVENT, "Flushing Internals");
  partManager->PARTITION_VECTOR.clear();
#ifndef ISNETWORK_PARTITION
  LOGGER(APP, "FINISHED Module Manager");
#endif
  LOG2FILE();

  if (partManager != 0)
    delete (partManager);
  exit(0);
}
void MODULE_MANAGER::DOSHUTDOWN(bool proper) {
  ACM_INTERNAL::MODULE_MANAGER *partManager =
      ACM_INTERNAL::MODULE_MANAGER::Instance();
  if (partManager->DUMMY_PROCESS_ID != 0) {
    LOGGER(INFO,
           "Module Manager, Received Sigint. SENDING SIGINT to dummy Process");
    kill(partManager->DUMMY_PROCESS_ID, SIGINT);
  } else {
    LOGGER(INFO, "Module Manager, RECEIVED SIGINT");
  }

  // should stop at next frame

  for (unsigned int i = 0; i < partManager->PARTITION_VECTOR.size(); i++) {
    if (partManager->PARTITION_VECTOR[i].PID == 0)
      continue;

    kill(partManager->PARTITION_VECTOR[i].PID, SIGSTOP);
  }

  for (unsigned int i = 0; i < partManager->PARTITION_VECTOR.size(); i++) {
    if (partManager->PARTITION_VECTOR[i].PID == 0)
      continue;

    LOGGER(INFO, "SENDING MESSAGE TO %d", partManager->PARTITION_VECTOR[i].PID);
    kill(partManager->PARTITION_VECTOR[i].PID, SIGCONT);
    kill(partManager->PARTITION_VECTOR[i].PID, SIGINT);
    // HELPER_CLOCK_NANOSLEEP(convertSecsToNano(10));
    waitpid(partManager->PARTITION_VECTOR[i].PID, NULL, 0);
  }

  partManager->PARTITION_VECTOR.clear();

#ifndef ISNETWORK_PARTITION
  LOGGER(APP, "FINISHED Module Manager");
#endif
  LOG2FILE();
  if (partManager != 0)
    delete (partManager);
  if (!proper)
    exit(-1);
  else {
    // LOG2FILE();
    exit(0);
  }
}

} // end Namespace

#endif //_MODULE_MANAGER_CC
