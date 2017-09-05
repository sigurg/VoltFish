#include <unistd.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "util.h"

//---------------------------------------------------------------------------
std::mutex StdOutExclusiveAccess::mtx;
//---------------------------------------------------------------------------
void hexdump(std::ostream& os, const void* p, int n, const char* prefix,
  int color)
{
  auto src = reinterpret_cast<const unsigned char*>(p);
  if (!n)
  {
    return;
  }
  auto f(os.flags());
  if (color && !isatty(1))
  {
    color = 0;
  }
  if (color)
  {
    os << "\x1b[" << color << "m";
  }
  if (prefix)
  {
    os << prefix;
  }
  os << "[" << std::hex << std::setfill('0');
  for (auto i = 0; i < n; ++i)
  {
    if (i)
    {
      os << " ";
    }
    os << std::setw(2) << int(*src++);
  }
  os << "]";
  if (color)
  {
    os << "\x1b[0m";
  }
  os.flags(f);
}
//---------------------------------------------------------------------------
