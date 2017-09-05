#include <iostream>
#include <iomanip>
#include <zlib.h>
#include "config_tree.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class Tokenizer
{
 private:
  friend class Inflater;

  char buf[2048];
  const char* src;
  const char* eod;
 public:
  Tokenizer() : src(buf), eod(src)
  {
  }
  uint8_t get_byte()
  {
    if (src+1>eod)
    {
      throw std::runtime_error("Tokenizer: get_byte: end of data");
    }
    return *src++;
  }
  void get_string(std::string& s)
  {
    unsigned namelen = get_byte();
    if (src+namelen>eod)
    {
      throw std::runtime_error("Tokenizer: get_string: end of data");
    }
    s.assign(src, namelen);
    src += namelen;
  }
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class Inflater
{
 private:
  z_stream s;
 public:
  Inflater()
  {
    s.zalloc   = Z_NULL;
    s.zfree    = Z_NULL;
    s.opaque   = Z_NULL;
    s.avail_in = 0;
    s.next_in  = Z_NULL;
    if (inflateInit(&s)!=Z_OK)
    {
      throw std::runtime_error("Inflater: inflateInit");
    }
  }
  ~Inflater()
  {
    inflateEnd(&s);
  }
  void exec(const std::string& src, Tokenizer& dst)
  {
    s.avail_in = uLongf(src.length());
    s.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(&src[0]));
    s.avail_out = uLongf(sizeof(dst.buf));
    s.next_out = reinterpret_cast<Bytef*>(dst.buf);
    if (inflate(&s,Z_FINISH)!=Z_STREAM_END)
    {
      throw std::runtime_error("Inflater: inflate");
    }
    dst.src = dst.buf;
    dst.eod = dst.src + s.total_out;
  }
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ConfigTree::ConfigTree()
 : tokenizer(new Tokenizer)
{
  // Initialy configuration tree contains description of two commands:
  // ADMIN:CRC32 and ADMIN:TREE.
  tree_by_id[0] = Element(0, "ADMIN:CRC32", U32);
  tree_by_id[1] = Element(1, "ADMIN:TREE" , BIN);
  tree_by_name["ADMIN:CRC32"] = &tree_by_id[0];
  tree_by_name["ADMIN:TREE" ] = &tree_by_id[1];
}
//---------------------------------------------------------------------------
ConfigTree::~ConfigTree()
{
  delete tokenizer;
}
//---------------------------------------------------------------------------
void ConfigTree::rebuild(const std::string& s)
{
  tree_by_id  .clear();
  tree_by_name.clear();
  Inflater().exec(s, *tokenizer);
  parse();
  tree_by_name["CH1:RANGE_I"]->type = CHOOSER;
  tree_by_name["CH2:RANGE_I"]->type = CHOOSER;
}
//---------------------------------------------------------------------------
void ConfigTree::update_range_i(const std::string& cmd)
{
  if (cmd == "CH1:MAPPING")
  {
    auto m = tree_by_name["CH1:RANGE_I"];
    m->value.clear();
    m->chooser_idx = 0;
    auto& cv = m->chooser_values;
    cv.clear();
    m = tree_by_name["CH1:MAPPING"];
    if (m->value == "SHARED")
    {
      m = tree_by_name["SHARED"];
    }
    if (!m->value.empty())
    {
      for (auto& e: m->chooser_values[m->chooser_idx].subvalue)
      {
        cv.push_back(e);
      }
    }
  }
  else if (cmd == "CH2:MAPPING")
  {
    auto m = tree_by_name["CH2:RANGE_I"];
    m->value.clear();
    m->chooser_idx = 0;
    auto& cv = m->chooser_values;
    cv.clear();
    m = tree_by_name["CH2:MAPPING"];
    if (m->value == "SHARED")
    {
      m = tree_by_name["SHARED"];
    }
    if (!m->value.empty())
    {
      for (auto& e: m->chooser_values[m->chooser_idx].subvalue)
      {
        cv.push_back(e);
      }
    }
  }
}
//---------------------------------------------------------------------------
void ConfigTree::parse(std::string cmd, int chooser)
{
  unsigned t = tokenizer->get_byte();
  if (t>FLT)
  {
    throw std::runtime_error("ConfigTree: parse: invalid type");
  }
  std::string name;
  tokenizer->get_string(name);
  unsigned nchildren = tokenizer->get_byte();

  if (!chooser && name.length())
  {
    if (cmd.length())
    {
      cmd += ":";
    }
    cmd += name;
  }

  if (chooser==1)
  {
    int idx = tree_by_id.size() - 1;
    tree_by_id[idx].chooser_values.push_back(ChooserValue(name));
  }
  else if (chooser==2)
  {
    int idx = tree_by_id.size() - 1;
    tree_by_id[idx].chooser_values.back().subvalue.push_back(name);
  }
  else if (t!=PLAIN)
  {
    int idx = tree_by_id.size();
    tree_by_id  [idx] = Element(idx, cmd, ArgumentType(t));
    tree_by_name[cmd] = &tree_by_id[idx];
  }

  for (unsigned i=0; i<nchildren; ++i)
  {
    parse(cmd, chooser || t==CHOOSER ? chooser+1 : chooser);
  }
}
//---------------------------------------------------------------------------
void ConfigTree::print(std::ostream& os) const
{
  const char* types[] = { "PLAIN", "LINK", "CHOOSER", "U8", "U16", "U32",
    "S8", "S16", "S32", "STR", "BIN", "FLT" };

  for (const auto& e: tree_by_id)
  {
    os
      << std::setw(2) << e.first
      << " " << e.second.name
      << " " << types[e.second.type];
    if (e.second.value.size())
    {
      if (e.second.type != BIN)
      {
        os << ", value = " << e.second.value;
      }
      if (e.second.type == CHOOSER)
      {
        os << ", idx = " << e.second.chooser_idx;
      }
    }
    if (e.second.chooser_values.size())
    {
      os << std::endl;
      for (unsigned i=0; i<e.second.chooser_values.size(); ++i)
      {
        os
          << std::setw(5) << i
          << " " << e.second.chooser_values[i].value << std::endl;
        for (unsigned j=0; j<e.second.chooser_values[i].subvalue.size(); ++j)
        {
          os
            << std::setw(9) << j
            << " " << e.second.chooser_values[i].subvalue[j] << std::endl;
        }
      }
    }
    else
    {
      os << std::endl;
    }
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
