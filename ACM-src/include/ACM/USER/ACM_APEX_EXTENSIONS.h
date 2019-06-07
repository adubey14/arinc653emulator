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

#ifndef ACM_APEX_EXTENSIONS_H
#define ACM_APEX_EXTENSIONS_H
#include <ACM/USER/ACM_HM_TYPES.h>
#include <ACM/USER/APEX_TYPES.h>
#include <string>

namespace ACM_USER {
//! \name HELPER FUNCTIONS (Not Part of APEX Specifications). These are added
//! functions which are not part of the regular APEX library. They are
//! implemented as a combination of one or more than one standard ARINC function
//! calls.
//@{
/* !
*
*/
/** AD: 11/25/2011
Adding new function addresses for user specified signal handlers. These if
provided will be invoked before the signal handler code implemented by the
emulator
*/
void SET_USER_SIGINT_HANDLER(/*!in*/ SYSTEM_ADDRESS_TYPE entry_point);
void SET_USER_SIGTERM_HANDLER(/*!in*/ SYSTEM_ADDRESS_TYPE entry_point);
void SET_USER_PRE_START_HANDLER(/*!in*/ SYSTEM_ADDRESS_TYPE entry_point);

void APEX_REATTACH_SIGNAL_HANDLER();
void PRINT_CREATED_OBJECT_LIST(/*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void SET_PARTITION_NAME(/*!in*/ std::string NAME);
std::string GET_PARTITION_NAME();
PROCESS_ID_TYPE APEX_HELPER_CREATE_PROCESS(
    /*!in*/ SYSTEM_TIME_TYPE PERIOD, /*!in*/ SYSTEM_TIME_TYPE TIME_CAPACITY,
    /*!in*/ SYSTEM_ADDRESS_TYPE ENTRY_POINT, /*!in*/ STACK_SIZE_TYPE STACK_SIZE,
    /*!in*/ PRIORITY_TYPE BASE_PRIORITY, /*!in*/ DEADLINE_TYPE DEADLINE,
    /*!in*/ std::string PROCESS_NAME, /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void APEX_HELPER_START_PROCESS(/*!in*/ PROCESS_NAME_TYPE PNAME,
                               /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void APEX_HELPER_START_PROCESS_AND_WAIT_FOR_RESPONSE(
    /*!in */ PROCESS_NAME_TYPE PNAME,
    /*!out*/ MESSAGE_ADDR_TYPE PROCESS_RESPONSE,
    /*!in */ MESSAGE_SIZE_TYPE PROCESS_RESPONSE_CAPACITY,
    /*!out*/ MESSAGE_SIZE_TYPE *PROCESS_RESPONSE_LENGTH,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void APEX_HELPER_DELAYED_START_PROCESS(/*!in*/ PROCESS_NAME_TYPE PNAME,
                                       /*!in*/ SYSTEM_TIME_TYPE DELAY,
                                       /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void APEX_HELPER_STOP_PROCESS(/*!in*/ PROCESS_NAME_TYPE PNAME,
                              /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void APEX_HELPER_SUSPEND_PROCESS(/*!in*/ PROCESS_NAME_TYPE PNAME,
                                 /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void APEX_HELPER_RESUME_PROCESS(/*!in*/ PROCESS_NAME_TYPE PNAME,
                                /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
//! Will create and start a dummy process. Such processes are necessary for
//! process suspension.
void APEX_HELPER_CREATE_AND_START_DUMMY_PROCESS(
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void APEX_HELPER_SET_EVENT(/*!in*/ EVENT_NAME_TYPE EVENT_NAME,
                           /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void APEX_HELPER_RESET_EVENT(/*!in*/ EVENT_NAME_TYPE EVENT_NAME,
                             /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void APEX_HELPER_WAIT_EVENT(/*!in*/ EVENT_NAME_TYPE EVENT_NAME,
                            /*!in*/ SYSTEM_TIME_TYPE TIME_OUT,
                            /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

//! \name PROCESS MANAGEMENT EXTENSIONS (Not Part of APEX Specifications). These
//! are added functions which are not part of the regular APEX library. They are
//! implemented as a combination of one or more than one standard ARINC function
//! calls.
//@{
/* !
*
*/

void GET_MY_NAME(/*!out*/ PROCESS_NAME_TYPE *PROCESS_NAME,
                 /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void GET_PROCESS_NAME(/*!in */ PROCESS_ID_TYPE PROCESS_ID,
                      /*!out*/ PROCESS_NAME_TYPE *PROCESS_NAME,
                      /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void NOTIFY_PROCESS_FINISHED_STATUS(
    /*!in */ PROCESS_ID_TYPE PROCESS_ID,
    /*!out*/ PROCESS_RESPONSE_TYPE *PROCESS_RESPONSE,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void GET_PROCESS_FINISHED_STATUS(
    /*!in */ PROCESS_ID_TYPE PROCESS_ID,
    /*!in */ SYSTEM_TIME_TYPE TIME_OUT,
    /*!out*/ MESSAGE_ADDR_TYPE PROCESS_RESPONSE,
    /*!in */ MESSAGE_SIZE_TYPE PROCESS_RESPONSE_CAPACITY,
    /*!out*/ MESSAGE_SIZE_TYPE *PROCESS_RESPONSE_LENGTH,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

//! \name HM Services. Health Manager Extensions. (Not Part of APEX
//! Specifications). They are implemented as a combination of one or more than
//! one standard ARINC function calls. This Health Management is different than
//! typical ARINC Error Handling Services.

void CREATE_HEALTH_MANAGER_PROCESS(
    /*!in*/ SYSTEM_ADDRESS_TYPE ENTRY_POINT,
    /*!in*/ PROCESS_NAME_TYPE PROCESS_NAME,
    /*!in*/ STACK_SIZE_TYPE STACK_SIZE,
    /*!out*/ HM_DESCRIPTOR_ID_TYPE *HM_DESCRIPTOR_ID,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

void REGISTER_PROCESS_WITH_HEALTH_MANAGER(
    /*!in */ PROCESS_ID_TYPE PROCESS_ID,
    /*!in */ HM_DESCRIPTOR_ID_TYPE HM_DESCRIPTOR_ID,
    /*!out*/ BLACKBOARD_ID_TYPE *BLACKBOARD_ID,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void START_HM(/*!in */ HM_DESCRIPTOR_ID_TYPE HM_DESCRIPTOR_ID,
              /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void HM_GET_MY_BUFFER_ID(/*!out*/ BUFFER_ID_TYPE *BUFFER_ID,
                         /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void RAISE_APPLICATION_ERROR(/*!in */ ERROR_CODE_TYPE ERROR_CODE,
                             /*!in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
                             /*!in */ ERROR_MESSAGE_SIZE_TYPE LENGTH,
                             /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

void RAISE_APPLICATION_ERROR_AND_GET_RESPONSE(
    /*!in */ ERROR_CODE_TYPE ERROR_CODE,
    /*!in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
    /*!in */ ERROR_MESSAGE_SIZE_TYPE LENGTH,
    /*!in */ SYSTEM_TIME_TYPE TIME_OUT,
    /*!out*/ HM_RESPONSE_TYPE *HM_RESPONSE,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

// void RAISE_APPLICATION_ERROR_AND_GET_RESPONSE (	 ERROR_CODE_TYPE
// ERROR_CODE,
//	MESSAGE_ADDR_TYPE MESSAGE_ADDR,
//	 ERROR_MESSAGE_SIZE_TYPE LENGTH,
//	 MESSAGE_ADDR_TYPE HM_RESPONSE,
//	 MESSAGE_SIZE_TYPE HM_RESPONSE_CAPACITY,
//	SYSTEM_TIME_TYPE TIME_OUT, //Added Timeout in response to the discussion
//with Gabor
//	 MESSAGE_SIZE_TYPE *HM_RESPONSE_LENGTH,
// RETURN_CODE_TYPE *RETURN_CODE );
void RAISE_APPLICATION_ERROR(
    /*!in*/ PROCESS_ID_TYPE FAULTY_PROCESS_ID,
    /*!in */ ERROR_CODE_TYPE ERROR_CODE,
    /*!in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
    /*!in */ ERROR_MESSAGE_SIZE_TYPE LENGTH,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void HM_GET_PROCESS_ERROR_MESSAGE(/*!out*/ ERROR_STATUS_TYPE *ERROR_STATUS,
                                  /*!out*/ BLACKBOARD_ID_TYPE *TARGET_BBOARD_ID,
                                  /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void HM_NOTIFY_PROCESS(/*!in*/ PROCESS_ID_TYPE PROCESS_ID,
                       /*!in*/ HM_RESPONSE_TYPE *HM_RESPONSE,
                       /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

//!\name RW_LOCK_SEMAPHORE management services (Not Part of APEX
//!Specifications).
void CREATE_RW_SEMAPHORE(
    /*!in */ SEMAPHORE_NAME_TYPE SEMAPHORE_NAME,
    /*!in */ SEMAPHORE_VALUE_TYPE MAXIMUM_READERS,
    /*!out*/ SEMAPHORE_ID_TYPE *SEMAPHORE_ID,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
void GET_RW_SEMAPHORE_ID(
    /*!in */ SEMAPHORE_NAME_TYPE SEMAPHORE_NAME,
    /*!out*/ SEMAPHORE_ID_TYPE *SEMAPHORE_ID,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

//! Currently ignore Time_Out
void READLOCK_RW_SEMAPHORE(
    /*!in */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,
    /*!in */ SYSTEM_TIME_TYPE TIME_OUT,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

void READUNLOCK_RW_SEMAPHORE(
    /*!in */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,

    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

void WRITELOCK_RW_SEMAPHORE(
    /*!in */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,
    /*!in */ SYSTEM_TIME_TYPE TIME_OUT,
    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);

void WRITEUNLOCK_RW_SEMAPHORE(
    /*!in */ SEMAPHORE_ID_TYPE SEMAPHORE_ID,

    /*!out*/ RETURN_CODE_TYPE *RETURN_CODE);
}

#endif
