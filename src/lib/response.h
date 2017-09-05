#ifndef __RESPONSE_H
#define __RESPONSE_H

#include <string>
#include <queue>
#include <future>
#include "generic.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct Response
{
  std::string name;
  std::string value;
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class ExpectedResponses
{
 private:
  std::queue<std::pair<std::string, std::promise<std::string>>> q;
  std::mutex mtx;
 public:
  void push(const std::string& cmd, std::promise<std::string> p)
  {
    std::lock_guard<std::mutex> guard(mtx);
    q.push(make_pair(cmd, std::move(p)));
  }
  bool pop(const Response& r)
  {
    std::lock_guard<std::mutex> guard(mtx);
    if (q.size() && q.front().first==r.name)
    {
      q.front().second.set_value(r.value);
      q.pop();
      return true;
    }
    return false;
  }
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // __RESPONSE_H
