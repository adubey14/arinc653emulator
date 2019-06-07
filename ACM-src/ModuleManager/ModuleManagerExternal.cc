#ifndef _MODULE_MANAGER_EXTERNAL_CC
#define _MODULE_MANAGER_EXTERNAL_CC

#include <ACM/INTERNAL/ModuleManager.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <sys/wait.h>
#include <vector>

namespace ACM_INTERNAL {
void MODULE_MANAGER::SORTALLLOGS() {
  pid_t pid_data = syscall(SYS_gettid);
  char MMfilename[30];
  sprintf(MMfilename, "%d.log", pid_data);
  std::vector<std::string> BUFFER;
  std::ifstream myfile(MMfilename);
  if (!myfile)
    return;
  std::string line;
  if (myfile.is_open()) {
    while (!myfile.eof()) {
      getline(myfile, line);
      if (line.find("HME") == std::string::npos &&
          line.find("TRC") == std::string::npos)
        continue;
      BUFFER.push_back(line);
      // cout << line << endl;
      // check if line contains TRC or HME
    }
    myfile.close();
  }

  for (unsigned int i = 0; i < this->PARTITION_VECTOR.size(); i++)

  {

    //	std::cout<<this->PARTITION_VECTOR[i].PID<<std::endl;

    char partfilename[30];
    sprintf(partfilename, "%d.log", this->PARTITION_VECTOR[i].PID);

    std::ifstream partfile(partfilename);
    if (!partfile)
      return;
    if (partfile.is_open()) {
      while (!partfile.eof()) {
        getline(partfile, line);
        if (line.find("HME") == std::string::npos &&
            line.find("TRC") == std::string::npos)
          continue;
        BUFFER.push_back(line);
        // cout << line << endl;
        // check if line contains TRC or HME
      }
      partfile.close();
    }
  }

  char Combinedfilename[30];
  sprintf(Combinedfilename, "%d_comb.log", pid_data);

  std::ofstream myfileout(Combinedfilename);
  if (!myfileout)
    return;

  for (unsigned int i = 0; i < BUFFER.size(); i++) {
    myfileout << BUFFER[i] << std::endl;
  }
  myfileout.close();

  char system_string[100] = "sort -t '|' -n -k 2  ";
  strcat(system_string, Combinedfilename);
  strcat(system_string, " > ");
  // printf("%s\n",system_string);

  char pid_str[30];
  sprintf(pid_str, "%d_sorted.log", pid_data);
  // printf("%s\n",pid_str);
  strcat(system_string, pid_str);

  // printf("%s\n",system_string);

  // int result =
  int result = system(system_string);
  if (result != 0) {
    LOGGER(ERROR, "Output of system call at line %d is %d", __LINE__, result);
  }

  //		printf("%s\n",system_string);

  strcpy(system_string, "rm ");
  //	printf("%s\n",system_string);
  strcat(system_string, Combinedfilename);
  // printf("%s\n",system_string);
  result = 0;
  result = system(system_string);
  if (result != 0) {
    LOGGER(ERROR, "Output of system call at line %d is %d", __LINE__, result);
  }
}
}
#endif
