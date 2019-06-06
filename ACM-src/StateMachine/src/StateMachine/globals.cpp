#ifdef WIN32
#pragma warning(disable:4018)
#pragma warning(disable:4996)
#endif

#include <StateMachine/globals.h>
#include <iostream>
using namespace std;
#ifndef WIN32
#define _snprintf snprintf
#endif
 Globals* Globals::_pinstance=0;
 Globals* Globals::Instance()
 {
	 if (_pinstance==0)
		 _pinstance= new Globals();

	 return _pinstance; 
 }

 void Globals::TerminateInstance()
 {
	  if (_pinstance!=0)
		  delete (_pinstance);
	  _pinstance=0;
 }


 
 LogStream::LogStream(): m_buff()
{ }

LogStream::~LogStream() 
{
	
	m_buff = "";
}

void LogStream::flushit()
{
	*this << "\n"; // flush
}


LogStream& operator<<( LogStream& stream, const char * r)
{
	return stream << std::string( r);
}

LogStream& operator<<( LogStream& stream, const std::string& r)
{
	stream.write( r.c_str(), r.length());
	
	stream.m_buff += r;
	if( r.find('\n') != std::string::npos )

	{
		//if( Globals::Instance()->ConsoleMode) {
			try { ACM_USER::LOGGER(ACM_USER::INFO,"%s",stream.m_buff.c_str()); }
			catch( ... ) { } // will throw if invoked without the Gui up&running
		//}
		stream.m_buff = "";
	}
	
	return stream;
}


LogStream& operator<<( LogStream& stream, const int i)
{
	char t[32];
	_snprintf( t, 32,"%i", i);
	return stream << std::string( t);
}

 LogStream& operator<<( LogStream& stream, const double i)
{
	char t[100];
	_snprintf( t,100, "%f", i);
	return stream << std::string( t);

}

 LogStream& operator<<( LogStream& stream, const unsigned long l)
 {
	 char t[100];
	_snprintf( t,100, "%lu", l);
	return stream << std::string( t);
 }


