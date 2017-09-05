#ifndef __MOOSHIMETER_H
#define __MOOSHIMETER_H

#include "ble.h"
#include "writer.h"
#include "response.h"
#include "reader.h"
#include "util.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class Mooshimeter
{
 private:
  BLE ble;
  ConfigTree config;
  ExpectedResponses expected;
  Writer wr;
  Reader rd;
 public:
  Mooshimeter(
    // Hardware address of the meter. Can be determined with hcitool
    // command, e.g.
    //
    //   % sudo hcitool lescan
    //   LE Scan ...
    //   88:4A:EA:8A:0B:0D Mooshimeter V.1
    const char* _hwaddr,

    // Handles corresponding to input and output streams. Can be found with
    // gatttool command, e.g.
    //
    //   % gatttool -b 88:4A:EA:8A:0B:0D --char-desc
    //   handle = 0x0001, uuid = 00002800-0000-1000-8000-00805f9b34fb
    //   handle = 0x0002, uuid = 00002803-0000-1000-8000-00805f9b34fb
    //   handle = 0x0003, uuid = 00002a00-0000-1000-8000-00805f9b34fb
    //   handle = 0x0004, uuid = 00002803-0000-1000-8000-00805f9b34fb
    //   handle = 0x0005, uuid = 00002a01-0000-1000-8000-00805f9b34fb
    //   handle = 0x0006, uuid = 00002803-0000-1000-8000-00805f9b34fb
    //   handle = 0x0007, uuid = 00002a02-0000-1000-8000-00805f9b34fb
    //   handle = 0x0008, uuid = 00002803-0000-1000-8000-00805f9b34fb
    //   handle = 0x0009, uuid = 00002a03-0000-1000-8000-00805f9b34fb
    //   handle = 0x000a, uuid = 00002803-0000-1000-8000-00805f9b34fb
    //   handle = 0x000b, uuid = 00002a04-0000-1000-8000-00805f9b34fb
    //   handle = 0x000c, uuid = 00002800-0000-1000-8000-00805f9b34fb
    //   handle = 0x000d, uuid = 00002803-0000-1000-8000-00805f9b34fb
    //   handle = 0x000e, uuid = 00002a05-0000-1000-8000-00805f9b34fb
    //   handle = 0x000f, uuid = 00002902-0000-1000-8000-00805f9b34fb
    //   handle = 0x0010, uuid = 00002800-0000-1000-8000-00805f9b34fb
    //   handle = 0x0011, uuid = 00002803-0000-1000-8000-00805f9b34fb
    //   handle = 0x0012, uuid = 1bc5ffa1-0200-62ab-e411-f254e005dbd4 <-- output stream
    //   handle = 0x0013, uuid = 00002901-0000-1000-8000-00805f9b34fb
    //   handle = 0x0014, uuid = 00002803-0000-1000-8000-00805f9b34fb
    //   handle = 0x0015, uuid = 1bc5ffa2-0200-62ab-e411-f254e005dbd4 <-- input stream
    //   handle = 0x0016, uuid = 00002902-0000-1000-8000-00805f9b34fb
    //   handle = 0x0017, uuid = 00002901-0000-1000-8000-00805f9b34fb
    uint16_t _hin, uint16_t _hout,

    // Function is called every time when complete measurement has been
    // read. Function is called from reader thread context.
    std::function<void(const Measurement&)> _measurement,

    // Function is called for responses that do not belong to measurement
    // data; battery status, logging status etc. Function is called from
    // reader thread context.
    std::function<void(const Response&)> _others,

    // Setting _verbose to true will print all sent and received packets to
    // output stream.
    bool _verbose=false);
  ~Mooshimeter();
  // Sends command cmd to the mooshimeter. Returns future object which allows
  // to obtain result. Function is non-blocking and thread safe.
  std::future<std::string> cmd(std::string cmd)
  {
    return wr.cmd(cmd);
  }
  // Returns configuration tree data in read-only mode.
  const ConfigTree& get_config_tree()
  {
    return config;
  }
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // __MOOSHIMETER_H
