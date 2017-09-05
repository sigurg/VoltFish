#ifndef __BLE_H
#define __BLE_H

#include <cstdint>
#include <string>

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Helper class. Represents stream descriptor. Closes stream automatically.
class Descriptor
{
 private:
  int fd;
 public:
  Descriptor(int _fd, const char* syscallname);
  ~Descriptor();
  operator int()
  {
    return fd;
  }
  void force_close();
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Represents serial connection with the meter.
class BLE
{
 private:
  std::string hwaddr;
  uint16_t hin;
  uint16_t hout;
  uint8_t prev_id;
  bool first_packet;
  bool verbose;
  Descriptor s;
 public:
  // Establishes connection with the meter.
  BLE(const char* _hwaddr, uint16_t _hin, uint16_t _hout,
      bool _verbose=false);

  void force_close()
  {
    s.force_close();
  }

  // Reads a single packet from the meter and appends its data to argument s.
  // Single packet can contain single response, multiple responses or be
  // a part of a response.
  //
  // Function is synchronuous (may block).
  //
  // Reader class offers more convenient way for reading data from the meter
  // and parsing them into individual responses.
  void read(std::string& s);

  // Sends packet with data s to the meter. Argument s has to contain single
  // command and to have no more than 19 bytes. All meter commands are
  // short enough and satisfy that condition.
  //
  // Function is synchronuous (may block) and thread safe.
  //
  // Writer class offers more convenient way for sending commands to the meter.
  void write(const std::string& s);
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // __BLE_H
