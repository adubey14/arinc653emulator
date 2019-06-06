#ifndef ISNETWORK_PARTITION
#ifndef _MODULE_MAIN_CC
#define _MODULE_MAIN_CC

#include <ACM/INTERNAL/ModuleManager.h>
using namespace std;

int main (int argc, char** argv)
{
	//ACM_INTERNAL::SocketSynchronizationMessageType SocketSynchronizationMessage(ACM_INTERNAL::SocketSynchronizationMessageType::START,"localhost","durip02",10);
	//SocketSynchronizationMessage.increment();

	//SocketSynchronizationMessage.printout();

	//ACM_INTERNAL::SocketSynchronizationMessageType::SERIAL_BUFFER_TYPE SERIALBUFFER;

	//ACM_INTERNAL::SocketSynchronizationMessageType::TO_SERIAL_BUFFER(SERIALBUFFER,SocketSynchronizationMessage);

	//ACM_INTERNAL::SocketSynchronizationMessageType SocketSynchronizationMessage2;

	//ACM_INTERNAL::SocketSynchronizationMessageType::FROM_SERIAL_BUFFER(SocketSynchronizationMessage2,SERIALBUFFER);

	//SocketSynchronizationMessage2.printout();

	//return 0;
	LOGGER(TRACE,"START: MM");
	ACM_INTERNAL::MODULE_MANAGER* main_instance =0;
	try{

		if (argc==1) LOGGER(WARNING,"Using Default configuration file %s",ACM_INTERNAL::MODULE_MANAGER::fileName.c_str());
		
		
		
		if (argc>1){
			std::string secondargument= argv[1];
			if (secondargument.compare("-?")==0 ||secondargument.compare("-h")==0 ||secondargument.compare("--help")==0||secondargument.compare("-help")==0) {
				cout<<argv[0]<<" <Partition Configuration File Name>\n or, -s for printing configurations schema \n";
				return 0;
			}
			if (secondargument.compare("-s")==0 ||secondargument.compare("--schema")==0||secondargument.compare("-schema")==0) {

				cout<<argv[0]<<"Here are the keys that can be used in a configuration file\n";
				ACM_INTERNAL::print_key_values();
				return 0;
			}

			ACM_INTERNAL::MODULE_MANAGER::fileName=argv[1];
			if(argc >2)
			{
				cout <<"Ignoring additional commandline arguments. Only using "<<argv [1]<<" \n";

			}
		}
		signal(SIGINT, &ACM_INTERNAL::MODULE_MANAGER::SIGINT_HANDLER );
		signal(SIGUSR1, &ACM_INTERNAL::MODULE_MANAGER::SIGUSR_HANDLER );
		signal(SIGTERM, &ACM_INTERNAL::MODULE_MANAGER::SIGTERM_HANDLER );


		//ACM_INTERNAL::MODULE_MANAGER::Instance()->CREATE_DUMMY_PROCESS();
		//ACM_INTERNAL::MODULE_MANAGER::Instance()->RunPartitionManager();

		//RETURN_CODE_TYPE RETURN_CODE;
		main_instance = ACM_INTERNAL::MODULE_MANAGER::Instance(); //create partition vectors


		LOGGER(APP,"STARTING MODULE MANAGER");
		ACM_INTERNAL::MODULE_MANAGER::Instance()->RUN_MODULE_MANAGER();
	}
	catch (int i){
		LOGGER(INFO,"Exit code %d",i);


		LOGGER(APP,"FINISHED MODULE MANAGER");
		//std::cout<<"**********Experiment Finished ****************\n";
		//main_instance->SORTALLLOGS();
		if(main_instance!=0)
			delete(main_instance);
		LOG2FILE();
		return i;
	}
	catch (...){
		LOGGER( ERROR,"reached here. Exception Received");
		//ACM_INTERNAL::MODULE_MANAGER::Instance()->DOSHUTDOWN(true);
	}
	if(main_instance!=0)
		delete(main_instance);



	LOGGER(APP,"FINISHED MODULE MANAGER");
	LOG2FILE();


	return 0;

}
#endif
#endif