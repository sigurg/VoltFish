#include "mooshimeter.h"
#include <iostream>

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Mooshimeter::Mooshimeter(const char* _hwaddr, uint16_t _hin, uint16_t _hout,
  std::function<void(const Measurement&)> _measurement,
  std::function<void(const Response&)> _others,
  bool _verbose)
 : ble(_hwaddr, _hin, _hout, _verbose),
   wr(ble, config, expected),
   rd(ble, config, expected, _measurement, _others)
{
  // Switch into full operational mode.
  wr.cmd("ADMIN:CRC32 "+wr.cmd("ADMIN:CRC32").get());
  // Update config tree data.
  config.rebuild(wr.cmd("ADMIN:TREE").get());

#ifndef _NDEBUG
  std::cerr << std::endl << "*** Config Tree: " << std::endl;
  config.print(std::cerr);
  std::cerr << std::endl << std::endl;
#endif
}
//---------------------------------------------------------------------------
Mooshimeter::~Mooshimeter()
{
  ble.force_close();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
