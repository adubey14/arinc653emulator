#ifndef ACM_TIMER_H
#define ACM_TIMER_H
#include <set>
#include <functional>
#include <ACM/USER/ACM_UTILITIES.h>
#include <ACM/INTERNAL/ACM_THREAD_SUPPORT.h>
#include <ACM/INTERNAL/EVENT.h>

 
namespace ACM_HELPERS
{

	struct SYSTEM_TIMESPEC_COMP : public std::binary_function <SYSTEM_TIMESPEC, SYSTEM_TIMESPEC, bool> 
	{
	public:
		bool operator()(SYSTEM_TIMESPEC d1, SYSTEM_TIMESPEC d2) const
		{
			return ACM_USER::compareTimeLess(d1,d2) ;
		}

	};

	class ACM_TIMER
	{

		typedef   void (*callback_type)(void);
		enum TIMER_STATE
		{
			WORKING,
			STOPPED
		};
	private:

		
		std::string name;
		ACM_INTERNAL::APEX_EVENT_TYPE internalevent_; //Intial state of This is locked;
		ACM_INTERNAL::SEMAPHORE internalsem_;
		ACM_INTERNAL::MUTEX internalMUTEX;
		pthread_t id;		
		callback_type ENTRY_POINT;
		TIMER_STATE my_state;		
		std::set<SYSTEM_TIMESPEC,SYSTEM_TIMESPEC_COMP> TimeValues;
		
		


	public:
		void addTimeValue(SYSTEM_TIMESPEC myspec,RETURN_CODE_TYPE* return_code);
		void removeTimeValue(SYSTEM_TIMESPEC myspec,RETURN_CODE_TYPE* return_code);
		void removeAllTimeValues(RETURN_CODE_TYPE* return_code);
		void stopTimer(RETURN_CODE_TYPE* return_code);
		void startTimer(RETURN_CODE_TYPE* return_code);
		long get_priority();
		void set_priority(long priority );
		~ACM_TIMER();


	private:
		void run();
		static void*  Thread_wrapper(void* arg);
		int create_thread();	
		void wait(void** exitval = NULL); 
		void terminate();
		
	
	private://Stop copy construction //stop assigment operator

		ACM_TIMER(const ACM_TIMER &sock) ;
		void operator=(const ACM_TIMER &sock);

	public:
		ACM_TIMER(std::string name,callback_type callback=NULL);

	};


}
#endif
