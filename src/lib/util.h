#ifndef __UTIL_H
#define __UTIL_H

#include <iostream>
#include <mutex>
#include "measurement.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Mooshimeter object creates three additional threads to perform its tasks.
// To avoid data races when printing to output or error streams the following
// class is used.
class StdOutExclusiveAccess : public std::lock_guard<std::mutex>
{
 private:
  static std::mutex mtx;
 public:
  StdOutExclusiveAccess() : lock_guard(mtx)
  {
  }
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void hexdump(std::ostream& os, const void* p, int n, const char* prefix=0,
  int color=0);
//---------------------------------------------------------------------------

#endif // __UTIL_H
