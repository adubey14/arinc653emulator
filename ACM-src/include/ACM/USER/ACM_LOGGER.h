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

#ifndef _ACM_LOGGER_H
#define _ACM_LOGGER_H
#include <ACM/USER/APEX_TYPES.h>
#include <string>
namespace ACM_USER{
	enum LOG_LEVEL {
		INFO=0,			
		WARNING=1,
		DEBUG=2,
		TRACE=3,
		HMEVENT=4,			
		APP=5,
		ERROR=6,
		FATAL=7,
		USER=8 // USER Level is to be used to printing business logic statements to the screen
	};

	// INFO < WARNING < DEBUG <  TRACE < HMEVENT< APP< ERROR<FATAL
	void LOGGER(LOG_LEVEL LOGLEVEL, const char * format,...);	
	void LOGGER_IDENT(char* NAME,LOG_LEVEL LOGLEVEL, const char * format,...);
	void CONFIGLOG(LOG_LEVEL OUTPUT);
	void FLUSHSTDOUT(bool input);
	void LOG2FILE();
 void REPORTER_INIT(std::string hostname, unsigned int portnumber,ACM_USER::RETURN_CODE_TYPE *RETURN_CODE);
 void REPORT(const char* message, ACM_USER::RETURN_CODE_TYPE *RETURN_CODE);
 void REPORTER_TERMINATE(ACM_USER::RETURN_CODE_TYPE *RETURN_CODE);
 void LOG_INIT();

 struct LOGGER_BUFFER
 {
   static const unsigned long LOG_ENTRY_SIZE;
   static unsigned long MAX_LOG_ENTRIES;
   static char ** log_buffer;
 };

}//end namespace ACM
#endif //_ACM_LOGGER_H

