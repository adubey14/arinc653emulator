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

#ifndef NETWORK_PARTITION_H
#define NETWORK_PARTITION_H

#include <ACM/INTERNAL/ACM_INTERNAL_INCLUDE.h>
using namespace ACM_USER;
using namespace ACM_INTERNAL;



struct NETWORK_PARTITION {
	std::string ConfigurationFile;
	std::string name;
	
	std::vector<std::string> QUEUEING_CHANNELS;
	std::vector<std::string> SAMPLING_CHANNELS;
	  

};


#endif