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
#ifndef _ACM_INTERNAL_INCLUDE_H
#define _ACM_INTERNAL_INCLUDE_H

#ifdef SCHEDRR
#define ACM_SCHED_POLICY SCHED_RR
#else
#define ACM_SCHED_POLICY SCHED_FIFO
#endif

#include <ACM/INTERNAL/ACM_THREAD_SUPPORT.h>
#include <ACM/USER/ACM_HM_TYPES.h>
#include <ACM/USER/APEX_TYPES.h>
#include <config.h>

#include <ACM/INTERNAL/APEX.h>
#include <ACM/INTERNAL/BLACKBOARD.h>
#include <ACM/INTERNAL/BUFFER.h>
#include <ACM/INTERNAL/EVENT.h>
#include <ACM/INTERNAL/PROCESS.h>

#include <ACM/INTERNAL/QUEUE_CHANNEL.h>
#include <ACM/INTERNAL/QUEUE_PORT.h>
#include <ACM/INTERNAL/SAMPLING_CHANNEL.h>
#include <ACM/INTERNAL/SAMPLING_PORT.h>

#include <ACM/USER/ACM_COMPONENT.h>
#include <ACM/USER/ACM_EXCEPTION.h>
#include <ACM/USER/ACM_LOGGER.h>
#include <ACM/USER/ACM_UTILITIES.h>
#include <ACM/USER/APEX_INTERFACE.h>
#include <stdio.h>

#endif
