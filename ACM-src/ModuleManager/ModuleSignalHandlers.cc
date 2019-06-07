namespace ACM_INTERNAL {

void MODULE_MANAGER::SIGTERM_HANDLER(int sig) {
  ACM_INTERNAL::MODULE_MANAGER *partManager =
      ACM_INTERNAL::MODULE_MANAGER::Instance();
  if (partManager->isSystemModuleManager)
    partManager->SEND_HYPER_PERIOD_ECHO(true);

  if (partManager->DUMMY_PROCESS_ID != 0) {
    LOGGER(
        INFO,
        "Module Manager, Received SigTerm. SENDING SIGTERM to dummy Process");
    kill(partManager->DUMMY_PROCESS_ID, SIGTERM);
  } else {
    LOGGER(INFO, "Module Manager, RECEIVED SIGTERM");
  }

  // should stop at next frame
  for (unsigned int i = 0; i < partManager->PARTITION_VECTOR.size(); i++) {
    if (partManager->PARTITION_VECTOR[i].PID == 0)
      continue;
    LOGGER(INFO, "SENDING MESSAGE TO %d", partManager->PARTITION_VECTOR[i].PID);
    kill(partManager->PARTITION_VECTOR[i].PID, SIGSTOP);
    kill(partManager->PARTITION_VECTOR[i].PID, SIGTERM);
  }
  for (unsigned int i = 0; i < partManager->PARTITION_VECTOR.size(); i++) {
    if (partManager->PARTITION_VECTOR[i].PID == 0)
      continue;
    LOGGER(INFO, "SENDING MESSAGE TO %d", partManager->PARTITION_VECTOR[i].PID);
    kill(partManager->PARTITION_VECTOR[i].PID, SIGCONT);
    waitpid(partManager->PARTITION_VECTOR[i].PID, NULL, 0);
  }

  partManager->PARTITION_VECTOR.clear();
#ifndef ISNETWORK_PARTITION
  LOGGER(APP, "FINISHED Module Manager");
#endif
  LOG2FILE();

  // throw (-1);

  exit(0);
}

void MODULE_MANAGER::SIGINT_HANDLER(int sig) {
  ACM_INTERNAL::MODULE_MANAGER *partManager =
      ACM_INTERNAL::MODULE_MANAGER::Instance();
  if (partManager->isSystemModuleManager)
    partManager->SEND_HYPER_PERIOD_ECHO(true);

  if (partManager->DUMMY_PROCESS_ID != 0) {
    LOGGER(INFO,
           "Module Manager, Received Sigint. SENDING SIGINT to dummy Process");
    kill(partManager->DUMMY_PROCESS_ID, SIGINT);
  } else {
    LOGGER(INFO, "Module Manager, RECEIVED SIGINT");
  }

  for (unsigned int i = 0; i < partManager->PARTITION_VECTOR.size(); i++) {
    if (partManager->PARTITION_VECTOR[i].PID == 0)
      continue;
    LOGGER(INFO, "SENDING MESSAGE SIGSTOP TO %d",
           partManager->PARTITION_VECTOR[i].PID);
    kill(partManager->PARTITION_VECTOR[i].PID, SIGSTOP);
  }

  for (unsigned int i = 0; i < partManager->PARTITION_VECTOR.size(); i++) {
    if (partManager->PARTITION_VECTOR[i].PID == 0)
      continue;
    LOGGER(INFO, "SENDING MESSAGE SIGCONT and SIGINT TO %d",
           partManager->PARTITION_VECTOR[i].PID);
    kill(partManager->PARTITION_VECTOR[i].PID, SIGCONT);
    kill(partManager->PARTITION_VECTOR[i].PID, SIGINT);
    // HELPER_CLOCK_NANOSLEEP(convertSecsToNano(10));
    waitpid(partManager->PARTITION_VECTOR[i].PID, NULL, 0);
    /*for (unsigned int count=0;count <
    partManager->PARTITION_VECTOR[i].QUEUE_CHANNEL_SOURCE_VECTOR.size();count++)
    {
    if (partManager->PARTITION_VECTOR[i].QUEUE_CHANNEL_SOURCE_VECTOR[count]!=0)
    delete(partManager->PARTITION_VECTOR[i].QUEUE_CHANNEL_SOURCE_VECTOR[count]);
    }*/
    // partManager->PARTITION_VECTOR[i].QUEUE_CHANNEL_SOURCE_VECTOR.clear();
    /*for (unsigned int
    count=0;count<partManager->PARTITION_VECTOR[i].SAMPLING_CHANNEL_SOURCE_VECTOR.size();count++)
    {
    if
    (partManager->PARTITION_VECTOR[i].SAMPLING_CHANNEL_SOURCE_VECTOR[count]!=0)
    delete(partManager->PARTITION_VECTOR[i].SAMPLING_CHANNEL_SOURCE_VECTOR[count]);
    }*/
    // partManager->PARTITION_VECTOR[i].SAMPLING_CHANNEL_SOURCE_VECTOR.clear();
  }

  partManager->PARTITION_VECTOR.clear();
#ifndef ISNETWORK_PARTITION
  LOGGER(APP, "FINISHED Module Manager");
#endif
  LOG2FILE();

  // throw (-1);

  exit(0);
}

void MODULE_MANAGER::SIGUSR_HANDLER(int sig) {
  LOGGER(INFO, "RECEIVED SIGUSR");
  ACM_INTERNAL::MODULE_MANAGER::Instance()->my_mutex.unlock();
}
};