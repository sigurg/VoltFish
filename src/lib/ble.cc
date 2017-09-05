#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include "ble.h"
#include "util.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class UnixError : public std::exception
{
  std::string msg;
 public:
  UnixError(const char* syscallname)
  {
    auto e = errno;
    msg.append(syscallname).append(": ").append(strerror(e));
  }
  const char* what() const throw ()
  {
    return msg.c_str();
  }
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class EndOfData : public std::runtime_error
{
 public:
  EndOfData() : runtime_error("end of data")
  {
  }
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Descriptor::Descriptor(int _fd, const char* syscallname) : fd(_fd)
{
  if (fd == -1)
  {
    throw UnixError(syscallname);
  }
}
//---------------------------------------------------------------------------
Descriptor::~Descriptor()
{
  close(fd);
}
//---------------------------------------------------------------------------
void Descriptor::force_close()
{
  close(fd);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BLE::BLE(const char* _hwaddr, uint16_t _hin, uint16_t _hout, bool _verbose)
 : hwaddr(_hwaddr), hin(_hin), hout(_hout),
   first_packet(true), verbose(_verbose),
   s(socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP),
     "BLE: socket")
{
  constexpr auto L2CAP_CID_LE_DATA = 4;

  sockaddr_l2 addr;
  memset(&addr, 0, sizeof(addr));
  addr.l2_family = AF_BLUETOOTH;
  addr.l2_cid = htobs(L2CAP_CID_LE_DATA);
  addr.l2_bdaddr_type = BDADDR_LE_PUBLIC;
  if (bind(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr))<0)
  {
    throw UnixError("BLE: bind");
  }

  bt_security btsec;
  memset(&btsec, 0, sizeof(btsec));
  btsec.level = BT_SECURITY_LOW;
  if (setsockopt(s, SOL_BLUETOOTH, BT_SECURITY, &btsec, sizeof(btsec))<0)
  {
    throw UnixError("BLE: setsockopt");
  }

  memset(&addr, 0, sizeof(addr));
  addr.l2_family = AF_BLUETOOTH;
  str2ba(hwaddr.c_str(), reinterpret_cast<bdaddr_t*>(&addr.l2_bdaddr));
  addr.l2_cid = htobs(L2CAP_CID_LE_DATA);
  addr.l2_bdaddr_type = BDADDR_LE_PUBLIC;
  if (connect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr))<0)
  {
    throw UnixError("BLE: connect");
  }
}
//---------------------------------------------------------------------------
void BLE::read(std::string& data)
{
  unsigned char buf[32];
  int n;
  while (true)
  {
    n = ::read(s, buf, sizeof(buf));
    if (n == -1)
    {
      if (errno == EINTR || errno == EAGAIN)
      {
        continue;
      }
      throw UnixError( "BLE: read" );
    }
    if (n == 0)
    {
      throw EndOfData();
    }
    break;
  }
  if (verbose)
  {
    constexpr auto WHITE = 37;
    StdOutExclusiveAccess guard;
    hexdump(std::cout, buf, n, "<--", WHITE);
    std::cout << std::endl;
  }
  constexpr auto ATT_OP_HANDLE_NOTIFY = 0x1b;
  if (n < 4 ||
      buf[0] != ATT_OP_HANDLE_NOTIFY ||
      bt_get_le16(buf+1) != hin)
  {
    return;
  }
  auto id = buf[3];
  if (first_packet)
  {
    first_packet = false;
    prev_id = id;
  }
  else if (++prev_id != id)
  {
    throw std::runtime_error("BLE: read: invalid sequence id");
  }
  data.append(reinterpret_cast<char*>(buf+4), n-4);
}
//---------------------------------------------------------------------------
void BLE::write(const std::string& data)
{
  if (data.size()>19)
  {
    throw std::runtime_error("BLE: write: data too large");
  }
  constexpr auto ATT_OP_WRITE_REQ = 0x12;
  char buf[32];
  buf[0] = ATT_OP_WRITE_REQ;
  bt_put_le16(hout, buf+1);
  buf[3] = 0;
  memcpy(buf+4, &data[0], data.size());
  int n = data.size() + 4;
  if (verbose)
  {
    constexpr auto YELLOW = 33;
    StdOutExclusiveAccess guard;
    hexdump(std::cout, buf, n, "-->", YELLOW);
    std::cout << std::endl;
  }
  while (true)
  {
    auto i = ::write(s, buf, n);
    if (i == -1)
    {
      if (errno == EINTR || errno == EAGAIN)
      {
        continue;
      }
      throw UnixError("BLE: write");
    }
    if (i == 0)
    {
      throw EndOfData();
    }
    if (i != n)
    {
      throw std::runtime_error("BLE: write: not all data written");
    }
    break;
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
