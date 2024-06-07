#include "stdio.h"
#include <sstream>

#ifndef LOG_H
#define LOG_H

// Logging stuff
extern std::stringstream debugLogStream;

class Log {
public:
  Log(const std::string &funcName) { debugLogStream << funcName << ": "; }

  template <class T> Log &operator<<(const T &v) {
    debugLogStream << v;
    return *this;
  }

  ~Log() {
    debugLogStream << std::endl;

    auto str = debugLogStream.str();

    FILE *f = fopen("/dev/stdout", "w");
    if (f != NULL) {
      fwrite(str.c_str(), 1, str.size(), f);
      fclose(f);
    }

    // Clear the stream
    debugLogStream.str("");
  }
};

#define DEBUGLOG Log(__FUNCTION__)

#endif
