#ifndef _STATE_VARIABLE_HELPER_H
#define _STATE_VARIABLE_HELPER_H
#include <ACM/USER/ACM_UTILITIES.h>
#include <stdlib.h>
#include <deque>
namespace ACM_USER
{

 // template<typename T> class STATE_VARIABLE;  // pre-declare the template class itself
 // template<typename T> T operator+ (const STATE_VARIABLE<T>& lhs, const STATE_VARIABLE<T>& rhs);
 // template<typename T> T operator> (const STATE_VARIABLE<T>& lhs, const STATE_VARIABLE<T>& rhs);
 // template<typename T> T operator>= (const STATE_VARIABLE<T>& lhs, const STATE_VARIABLE<T>& rhs);
 // template<typename T> T operator!= (const STATE_VARIABLE<T>& lhs, const STATE_VARIABLE<T>& rhs);
 
 // template<typename T> std::ostream& operator<< (std::ostream& o, const Foo<T>& x); 
 
 

	template <class T>
	class STATE_VARIABLE 
	{
	private:
	struct VALUE_ITEM{
	T VALUE;
	SYSTEM_TIME_TYPE TIME_OF_CREATION;	  
	VALUE_ITEM(T& DATA):VALUE(DATA),TIME_OF_CREATION(currentTimeInNanoSeconds()) {}			
	VALUE_ITEM(T& DATA,SYSTEM_TIME_TYPE& now):VALUE(DATA),TIME_OF_CREATION(now) {}	
	};
		unsigned int WINDOW_SIZE;
		std::deque< VALUE_ITEM > VALUES;
	public:
		STATE_VARIABLE(int HISTORY_DEPTH=2):WINDOW_SIZE(HISTORY_DEPTH)
		{
		}
		//!returns the size of the internal queue
		
		int RESET()
		{
			VALUES.clear();
			return VALUES.size();
		}
		
		void SET(T input)
		{
			if (VALUES.size()==WINDOW_SIZE)
				VALUES.pop_front();
			VALUE_ITEM item (input);
			VALUES.push_back(input);
			
			return;
		}
		
		
		
		
		 const T& operator=(const T &rhs)
		{
			this->SET(rhs);
			return rhs;
		}


		
		void SET(T input,SYSTEM_TIME_TYPE now)
		{
			if (VALUES.size()==WINDOW_SIZE)
				VALUES.pop_front();
			VALUE_ITEM item (input,now);			
			VALUES.push_back(input);
			
			return;
		}
		
		void UNSET_LAST_SAMPLE(RETURN_CODE_TYPE* RETURN_CODE)
		{
			*RETURN_CODE=NO_ERROR;
			if (VALUES.size() ==0)
			{
				*RETURN_CODE=NOT_AVAILABLE;
				return;
			}
			VALUES.pop_back();			
		}
		
		/*! TIME_INDEX should always be <=0. 0 implies current sample, -1 imples 1 previous sample and so on
			Will return INVALID_PARAM if TIME_INDEX >0
		
		*/
		void GET(T* outData, RETURN_CODE_TYPE* RETURN_CODE, int TIME_INDEX=0) const
		{
			*RETURN_CODE=NO_ERROR;
			
			if (TIME_INDEX >0)
			{
				*RETURN_CODE=INVALID_PARAM;
				return ;
			}			
			if (VALUES.size() ==0)
			{
				*RETURN_CODE=NOT_AVAILABLE;
				return ;
			}			
			if ( abs(TIME_INDEX) +1 > VALUES.size())
			{
				*RETURN_CODE=NOT_AVAILABLE;
				return;			
			}
			
			int index_to_return= (VALUES.size() -1) - abs(TIME_INDEX);
			
			*outData = VALUES[index_to_return].VALUE;				
		}
		
		//!Will return value and time.		
		void GET(T* outData, SYSTEM_TIME_TYPE* outTime, RETURN_CODE_TYPE* RETURN_CODE, int TIME_INDEX=0) const
		{
			*RETURN_CODE=NO_ERROR;
			
			if (TIME_INDEX >0)
			{
				*RETURN_CODE=INVALID_PARAM;
				return ;
			}			
			if (VALUES.size() ==0)
			{
				*RETURN_CODE=NOT_AVAILABLE;
				return ;
			}			
			if ( abs(TIME_INDEX) +1 > VALUES.size())
			{
				*RETURN_CODE=NOT_AVAILABLE;
				return;			
			}
			
			int index_to_return= (VALUES.size() -1) - abs(TIME_INDEX);
			
			*outData = VALUES[index_to_return].VALUE;	
			*outTime =VALUES[index_to_return].TIME_OF_CREATION;			
		}
		

		T VALUE(int TIME_INDEX=0)
		{
			
			T temp;
			if (TIME_INDEX >0)
			{
			
				return temp ;
			}			
			if (VALUES.size() ==0)
			{
				return temp;
			}			
			if ( abs(TIME_INDEX) +1 > VALUES.size())
			{
				return temp;			
			}
			
			int index_to_return= (VALUES.size() -1) - abs(TIME_INDEX);
			
			return (VALUES[index_to_return].VALUE);				
		}
		
		T GET()
		{
			return this->VALUE();
		}
		
		operator T()
		{
			return this->VALUE();
		}
		
		
				
		// friend bool operator> (const T& rhs)
		// {
			// return (this->VALUE() > rhs);
		// }
		
		// bool operator>(const T& rhs)
		// {
			// return (this->VALUE() > rhs);
		// }
		
		void GET_TIMESTAMP(SYSTEM_TIME_TYPE* outData, RETURN_CODE_TYPE* RETURN_CODE, int TIME_INDEX=0) const
		{
			*RETURN_CODE=NO_ERROR;
			
			if (TIME_INDEX >0)
			{
				*RETURN_CODE=INVALID_PARAM;
				return ;
			}			
			if (VALUES.size() ==0)
			{
				*RETURN_CODE=NOT_AVAILABLE;
				return ;
			}			
			if ( abs(TIME_INDEX) +1 > VALUES.size())
			{
				*RETURN_CODE=NOT_AVAILABLE;
				return;			
			}
			
			int index_to_return= (VALUES.size() -1) - abs(TIME_INDEX);
			
			*outData = VALUES[index_to_return].TIME_OF_CREATION;				
		}
		
		SYSTEM_TIME_TYPE GET_TIMESTAMP()
		{
			return this->VALUE().TIME_OF_CREATION;
		}
		
		
		double RATE_OF_CHANGE(RETURN_CODE_TYPE* RETURN_CODE) const
		{
			*RETURN_CODE=NO_ERROR;
			if (VALUES.size() <=1)
			{
				*RETURN_CODE=NOT_AVAILABLE;
				return 0;
			}
			
			int size = VALUES.size();
			double timedelta=(double)(VALUES[size-1].TIME_OF_CREATION - VALUES[size-2].TIME_OF_CREATION);
			double timedeltainseconds=timedelta/NANO;
			
			double rate = ((double)(VALUES[size-1].VALUE - VALUES[size-2].VALUE))/(timedeltainseconds);
			return rate;	
		}
		
		double RATE_OF_CHANGE(T Second, SYSTEM_TIME_TYPE now,RETURN_CODE_TYPE* RETURN_CODE) const
		{
			*RETURN_CODE=NO_ERROR;
			if (VALUES.size() ==0)
			{
				*RETURN_CODE=NOT_AVAILABLE;
				return 0;
			}
			
			int size = VALUES.size();
			double timedelta=(double)(now - VALUES[size-1].TIME_OF_CREATION);
			double timedeltainseconds=timedelta/NANO;
			double rate = ((double)(Second - VALUES[size-1].VALUE))/(timedeltainseconds);
			return rate;	
		}
		
		double RATE_OF_CHANGE(T Second, RETURN_CODE_TYPE* RETURN_CODE) const
		{
			return 	RATE_OF_CHANGE(Second,currentTimeInNanoSeconds(),RETURN_CODE);
		}
		
		double DELTA(RETURN_CODE_TYPE* RETURN_CODE) const
		{
			*RETURN_CODE=NO_ERROR;
			if (VALUES.size() <=1)
			{
				*RETURN_CODE=NOT_AVAILABLE;
				return 0;
			}
			
			int size = VALUES.size();
			double rate = VALUES[size-1].VALUE - VALUES[size-2].VALUE;
			return rate;	
		}
		
		double DELTA(T Second,RETURN_CODE_TYPE* RETURN_CODE) const
		{
			*RETURN_CODE=NO_ERROR;
			if (VALUES.size() ==0)
			{
				*RETURN_CODE=NOT_AVAILABLE;
				return 0;
			}
			
			int size = VALUES.size();
			double rate = Second - VALUES[size-1].VALUE;
			return rate;	
		}	


		//SERIALIZE_TO_BUFFER
		void CHECKPOINT (MESSAGE_ADDR_TYPE MESSAGE_ADDR,MESSAGE_SIZE_TYPE MAX_LENGTH,MESSAGE_SIZE_TYPE *LENGTH_WRITTEN,RETURN_CODE_TYPE* RETURN_CODE)
		{
			*RETURN_CODE=NO_ERROR;
			T outData;
			SYSTEM_TIME_TYPE timeval;
			*LENGTH_WRITTEN=0;
			//check MAX_LENGTH >= size(T)+size(SYSTEM_TIME_TYPE)+size(APEX_INTEGER)
			if(MAX_LENGTH < sizeof(T)+sizeof(SYSTEM_TIME_TYPE)+sizeof(APEX_INTEGER))
			{
				*RETURN_CODE=INVALID_CONFIG;
				return;
			}			
			GET(&outData, &timeval, RETURN_CODE);
			if (*RETURN_CODE!=NO_ERROR)
			{
				return;				
			}		
			//Written in the format
			//TotalSize+T+TIME
			APEX_INTEGER length=sizeof(T)+sizeof(SYSTEM_TIME_TYPE);
			memcpy(MESSAGE_ADDR,(void*)&length,sizeof(APEX_INTEGER));
			*LENGTH_WRITTEN=sizeof(APEX_INTEGER);
			memcpy(MESSAGE_ADDR+(*LENGTH_WRITTEN),(void*)&(outData),sizeof(T));
			*LENGTH_WRITTEN=*LENGTH_WRITTEN+sizeof(T);
			memcpy(MESSAGE_ADDR+(*LENGTH_WRITTEN),(void*)&(timeval),sizeof(SYSTEM_TIME_TYPE));
			*LENGTH_WRITTEN=*LENGTH_WRITTEN+sizeof(SYSTEM_TIME_TYPE);
			return;			
		}		
		
		void RESTORE(MESSAGE_ADDR_TYPE MESSAGE_ADDR,MESSAGE_SIZE_TYPE LENGTH,RETURN_CODE_TYPE* RETURN_CODE)
		{
			this->RESET();
			*RETURN_CODE=NO_ERROR;
			//The first entry it reads should be same as length
			APEX_INTEGER lengthr;
			memcpy(&lengthr,MESSAGE_ADDR,sizeof(APEX_INTEGER));
			if(lengthr!=LENGTH)
			{
				//Problem
				LOGGER(ERROR,"Header Length read from buffer not same as length passed in the restore API");
				*RETURN_CODE=INVALID_CONFIG;
				return;
			}
			if(lengthr!=sizeof(T)+sizeof(SYSTEM_TIME_TYPE)+sizeof(APEX_INTEGER))
			{
				LOGGER(ERROR,"Header Length != sizeof(T)+sizeof(SYSTEM_TIME_TYPE)+sizeof(APEX_INTEGER)");
				*RETURN_CODE=INVALID_CONFIG;
				return;				
			}
			
			T indata;
			SYSTEM_TIME_TYPE timeval;
			//Check Length == size(T)+size(SYSTEM_TIME_TYPE)+size(APEX_INTEGER)
			memcpy((void*)&indata,MESSAGE_ADDR+(sizeof(APEX_INTEGER)),sizeof(T));
			memcpy((void*)&timeval,MESSAGE_ADDR+(sizeof(APEX_INTEGER)+sizeof(T)),sizeof(SYSTEM_TIME_TYPE));
			this->SET(indata,timeval);
			return;						
		}
	};


}
#endif


