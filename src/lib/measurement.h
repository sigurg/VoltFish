#ifndef __MEASUREMENT_H
#define __MEASUREMENT_H

#include <iostream>
#include <ctime>
#include <cmath>
#include <vector>
#include <queue>
#include <condition_variable>
#include "response.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class Measurement
{
 private:
  int state;
 public:
  class Time
  {
   public:
    time_t s;
    unsigned ms;
    Time() : s(0), ms(0)
    {
    }
    void clear()
    {
      s = 0;
      ms = 0;
    }
  };
  class Channel
  {
   public:
    double value;
    int bps;
    double lsb2native;
    std::vector<double> buf;
    Channel() : value(NAN), bps(0), lsb2native(NAN)
    {
    }
    void clear()
    {
      value = NAN;
      bps = 0;
      lsb2native = NAN;
      buf.clear();
    }
    void read_buffer(const Response& r);
  };
  Time t;
  Channel ch1, ch2;
  double pwr;
  Measurement() : state(0), pwr(NAN)
  {
  }
  void clear()
  {
    state = 0;
    t.clear();
    ch1.clear();
    ch2.clear();
    pwr = NAN;
  }
  bool update(const Response& r, std::function<void(const Measurement&)> f);
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // __MEASUREMENT_H
