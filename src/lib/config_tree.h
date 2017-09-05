#ifndef __CONFIG_TREE_H
#define __CONFIG_TREE_H

#include <ios>
#include <string>
#include <vector>
#include <deque>
#include <map>

// Configuration tree is additional data set.  Contains information of all
// commands and their arguments.  Gives ability to use textual commands
// names instead of binary identifiers.
//
// ConfigTree class keeps two dictionaries (ordered by command name or
// command id respectively).

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
enum ArgumentType
{ PLAIN, LINK, CHOOSER, U8, U16, U32, S8, S16, S32, STR, BIN, FLT };
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct ChooserValue
{
  std::string value;
  // Commands CH1:RANGE_I and CH2:RANGE_I have argument of type U8. Actually
  // their argument type should be of type CHOOSER. However, set of available
  // values is not constant and depends on values of CH1:MAPPING, CH2:MAPPING
  // and SHARED. Moreover, MAPPING and SHARED are described by two levels of
  // values in configuration tree. That second level represents values of
  // RANGE_I and is stored in subvalue vector.
  std::vector<std::string> subvalue;
  ChooserValue(const std::string& _value) : value(_value)
  {
  }
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct Element
{
  unsigned id;
  std::string name;
  std::string value;
  ArgumentType type;
  std::vector<ChooserValue> chooser_values;
  unsigned chooser_idx;

  Element(unsigned _id = 0, const std::string _name = "",
    ArgumentType _type = PLAIN)
   : id(_id), name(_name), type(_type), chooser_idx(0)
  {
  }
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class Tokenizer;
class ConfigTree
{
 private:
  Tokenizer* tokenizer;

  void parse(std::string cmd="", int chooser=0);
 public:
  std::map<unsigned, Element> tree_by_id;
  std::map<std::string, Element*> tree_by_name;

  ConfigTree();
  ~ConfigTree();

  // Rebuilds dictionaries based on compressed data s.
  void rebuild(const std::string& s);

  void update_range_i(const std::string& cmd);

  // Print textual version of data to given stream.
  void print(std::ostream& os) const;
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // __CONFIG_TREE_H
