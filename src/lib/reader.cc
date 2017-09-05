#include <bluetooth/bluetooth.h>
#include "config_tree.h"
#include "writer.h"
#include "response.h"
#include "reader.h"
#include "util.h"
#include "measurement.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Reader::Reader(BLE& _ble, ConfigTree& _config, ExpectedResponses& _expected,
  std::function<void(const Measurement&)> _measurement,
  std::function<void(const Response&)> _others)
 : ble(_ble), config(_config), expected(_expected),
   measurement(_measurement), others(_others), running(true),
   reader_t([&](){ reader_thread(); })
{
}
//---------------------------------------------------------------------------
Reader::~Reader()
{
  running = false;
  reader_t.join();
}
//---------------------------------------------------------------------------
const char* Reader::get_bytes(int n)
{
  if (data.size()<1u+n)
  {
    throw false;
  }
  static char tab[256];
  memcpy(tab, &data[1], n);
  data.erase(0, 1+n);
  return tab;
}
//---------------------------------------------------------------------------
std::string Reader::get_blob()
{
  if (data.size()<1u+2)
  {
    throw false;
  }
  int len = bt_get_le16(&data[1]);
  if (data.size()<1u+2+len)
  {
    throw false;
  }
  std::string v = data.substr(3, len);
  data.erase(0,3+len);
  return v;
}
//---------------------------------------------------------------------------
void Reader::process()
{
  try
  {
    while (true)
    {
      if (!data.size())
      {
        throw false;
      }
      int id = data[0];
      auto it = config.tree_by_id.find(id);
      if (it == config.tree_by_id.end())
      {
        throw std::runtime_error("Reader: "+std::to_string(id)+
          ": invalid id");
      }
      Response r;
      r.name = it->second.name;
      switch (it->second.type)
      {
        case PLAIN:
        case LINK:          get_bytes    (); break;
        case U8 : r.value = get<uint8_t >(); break;
        case S8 : r.value = get<int8_t  >(); break;
        case U16: r.value = get<uint16_t>(); break;
        case S16: r.value = get<int16_t >(); break;
        case U32: r.value = get<uint32_t>(); break;
        case S32: r.value = get<int32_t >(); break;
        case FLT: r.value = get<float   >(); break;
        case STR:
        case BIN: r.value = get_blob     (); break;
        case CHOOSER:
        {
          auto v = std::stoul(get<uint8_t>());
          if (v>=it->second.chooser_values.size())
          {
            throw std::runtime_error("Reader: "+r.name+": "+std::to_string(v)+
              ": invalid chooser value");
          }
          r.value = it->second.chooser_values[v].value;
          it->second.chooser_idx = v;
          break;
        }
      }
      it->second.value = r.value;
      handle_response(r);
    }
  }
  catch (bool)
  {
  }
}
//---------------------------------------------------------------------------
// Reader thread calls Mooshimeter::read and parses received data into
// responses of three types:
//   - results for commands (return values of Writer::cmd)
//   - results of measurements
//   - other responses
void Reader::reader_thread()
{
  try
  {
    while (running)
    {
      ble.read(data);
      process();
    }
  }
  catch (std::exception& ex)
  {
    StdOutExclusiveAccess guard;
    std::cerr << "reader thread: " << ex.what() << std::endl;
  }
}
//---------------------------------------------------------------------------
void Reader::handle_response(Response& r)
{
  config.update_range_i(r.name);
  if (expected.pop(r) || single.update(r, measurement))
  {
    return;
  }
  try
  {
    others(r);
  }
  catch (std::exception& ex)
  {
    StdOutExclusiveAccess guard;
    std::cerr << "others: " << ex.what() << std::endl;
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
