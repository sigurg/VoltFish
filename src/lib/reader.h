#ifndef __READER_H
#define __READER_H

#include <string>
#include <map>
#include <functional>
#include "ble.h"
#include "config_tree.h"
#include "measurement.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class Reader
{
 private:
  BLE& ble;
  ConfigTree& config;
  ExpectedResponses& expected;
  std::function<void(const Measurement&)> measurement;
  std::function<void(const Response&)> others;
  std::string data;
  Measurement single;

  const char* get_bytes(int n=0);
  std::string get_blob();
  template <class T> std::string get()
  {
    return std::to_string(*reinterpret_cast<const T*>(get_bytes(sizeof(T))));
  }

  void handle_response(Response& r);
  void process();

  std::atomic<bool> running;
  std::thread reader_t;
  void reader_thread();
 public:
  Reader(BLE& _ble, ConfigTree& _config, ExpectedResponses& _expected,
    std::function<void(const Measurement&)> _measurement,
    std::function<void(const Response&)> _others);
  ~Reader();
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // __READER_H
