#ifndef GLOBALS_H
#define GLOBALS_H

#include <ACM/USER/ACM_LOGGER.h>
#include <fstream>
#include <string>

class LogStream : public std::ofstream {
public:
  LogStream();
  ~LogStream();
  void flushit();

  // friend put-to operators
  friend LogStream &operator<<(LogStream &s, const char *r);
  friend LogStream &operator<<(LogStream &s, const std::string &r);
  friend LogStream &operator<<(LogStream &s, const int i);
  friend LogStream &operator<<(LogStream &s, const double i);
  friend LogStream &operator<<(LogStream &s, const unsigned long l);
  std::string m_buff;
};

class Globals {
private:
  Globals() : outputName() {}
  static Globals *_pinstance;
  ~Globals() {}

public:
  static void TerminateInstance();
  static Globals *Instance();
  std::string outputName;
  LogStream reg;
};

#endif