#include "mooshimeter.h"
#include <zlib.h>
#include <iostream>

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Mooshimeter::Mooshimeter(const char *_hwaddr, uint16_t _hin, uint16_t _hout,
                         std::function<void(const Measurement &)> _measurement,
                         std::function<void(const Response &)> _others,
                         bool _verbose)
    : ble(_hwaddr, _hin, _hout, _verbose),
      wr(ble, config, expected),
      rd(ble, config, expected, _measurement, _others) {
    // get CRC of config tree
    auto expected_crc = std::stoul(wr.cmd("ADMIN:CRC32").get());

    // get config tree
    auto ctree = wr.cmd("ADMIN:TREE").get();

    // calc CRC32
    auto crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, reinterpret_cast<const Bytef *>(ctree.c_str()),
                ctree.length());

    if (expected_crc != crc) {
        throw std::runtime_error("Mooshimeter: config tree corrupt, CRC32 mismatch");
    }

    // Update config tree data.
    config.rebuild(ctree);

    // Switch into full operational mode.
    // dirty hack to fix 152xxx firmware bug: CRC32 value needs to be
    // sent as signed int
    wr.cmd("ADMIN:CRC32 "+std::to_string(int32_t(crc)));

#ifndef QT_NO_DEBUG_OUTPUT
    std::cerr << std::endl << "*** Config Tree: " << std::endl;
    config.print(std::cerr);
    std::cerr << std::endl << std::endl;
#endif
}
//---------------------------------------------------------------------------
Mooshimeter::~Mooshimeter() {
    ble.force_close();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
