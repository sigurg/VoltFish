#ifndef __GENERIC_H
#define __GENERIC_H

#include <mutex>
#include <condition_variable>
#include <queue>

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class ForceQuit
{
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
template <class T>
class BlockingQueue
{
 private:
  std::queue<T> q;
  std::mutex mtx;
  std::condition_variable cv;
  std::atomic<bool> running;
 public:
  BlockingQueue() : running(true)
  {
  }
  void push(T e)
  {
    {
      std::lock_guard<std::mutex> guard(mtx);
      q.push(std::move(e));
    }
    cv.notify_one();
  }
  T pop()
  {
    std::unique_lock<std::mutex> guard(mtx);
    cv.wait(guard, [&](){ return q.size()>0 || !running; });
    if (!running)
    {
      throw ForceQuit();
    }
    auto e = std::move(q.front());
    q.pop();
    return e;
  }
  void force_quit()
  {
    running = false;
    cv.notify_one();
  }
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // __GENERIC_H
