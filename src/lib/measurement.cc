#include <algorithm>
#include <bluetooth/bluetooth.h>
#include "measurement.h"
#include "util.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Measurement::Channel::read_buffer(const Response& r)
{
  if (bps==8)
  {
    for (unsigned i=0; i<r.value.size(); ++i)
    {
      buf.push_back(int8_t(r.value[i]) * lsb2native);
    }
  }
  else if (bps==16)
  {
    for (unsigned i=0; i<r.value.size(); i+=2)
    {
      buf.push_back(bt_get_le16(&r.value[i]) * lsb2native);
    }
  }
  else if (bps==24)
  {
    for (unsigned i=0; i<r.value.size(); i+=3)
    {
      int v = *reinterpret_cast<const int8_t*>(&r.value[i+2]);
      v <<= 8;
      v |= *reinterpret_cast<const uint8_t*>(&r.value[i+1]);
      v <<= 8;
      v |= *reinterpret_cast<const uint8_t*>(&r.value[i]);
      buf.push_back(v * lsb2native);
    }
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool Measurement::update(const Response& r,
  std::function<void(const Measurement&)> f)
{
  constexpr const char* cmds[] =
  {
    "TIME_UTC", "TIME_UTC_MS",
    "CH1:VALUE", "CH1:BUF_BPS", "CH1:BUF_LSB2NATIVE", "CH1:BUF",
    "CH2:VALUE", "CH2:BUF_BPS", "CH2:BUF_LSB2NATIVE", "CH2:BUF",
    "REAL_PWR"
  };
  constexpr int N = sizeof(cmds)/sizeof(cmds[0]);
  auto it = std::find_if(cmds, cmds+N,
    [&r](std::string p){ return r.name == p; });
  if (it == cmds + N)
  {
    return false;
  }
  if (r.name == "CH1:BUF_BPS" && std::string("CH1:VALUE") == cmds[state])
  {
    ++state;
  }
  else if (r.name == "CH2:BUF_BPS" && std::string("CH2:VALUE") == cmds[state])
  {
    ++state;
  }
  if (r.name != cmds[state])
  {
    StdOutExclusiveAccess guard;
    std::cout
      << r.name << " received, "
      << cmds[state] << " expected"
      << std::endl;
    clear();
    return true;
  }
       if (r.name == "TIME_UTC"          ) t.s            = std::stoul(r.value);
  else if (r.name == "TIME_UTC_MS"       ) t.ms           = std::stoul(r.value);
  else if (r.name == "CH1:VALUE"         ) ch1.value      = std::stod (r.value);
  else if (r.name == "CH1:BUF_BPS"       ) ch1.bps        = std::stoul(r.value);
  else if (r.name == "CH1:BUF_LSB2NATIVE") ch1.lsb2native = std::stod (r.value);
  else if (r.name == "CH1:BUF"           ) ch1.read_buffer(r);
  else if (r.name == "CH2:VALUE"         ) ch2.value      = std::stod (r.value);
  else if (r.name == "CH2:BUF_BPS"       ) ch2.bps        = std::stoul(r.value);
  else if (r.name == "CH2:BUF_LSB2NATIVE") ch2.lsb2native = std::stod (r.value);
  else if (r.name == "CH2:BUF"           ) ch2.read_buffer(r);
  else if (r.name == "REAL_PWR"          ) pwr            = std::stod (r.value);

  if (r.name == "REAL_PWR")
  {
    try
    {
      f(*this);
    }
    catch (std::exception& ex)
    {
      StdOutExclusiveAccess guard;
      std::cerr << "measurement: " << ex.what() << std::endl;
    }
    clear();
  }
  else
  {
    state += (r.name == "CH1:VALUE" || r.name == "CH2:VALUE") ? 4 : 1;
  }
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
