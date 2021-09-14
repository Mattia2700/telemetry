#include "websocket.h"
#include <ostream>

int main()
{

  StringBuffer s;
  Writer<StringBuffer> w(s);
  w.StartObject();
  w.Key("Date");
  w.String("date_c");

  w.Key("obj2");
  w.StartObject();
  w.Key("key2");
  w.String("ckmd");
  w.EndObject();
  w.EndObject();

  cout << s.GetString() << endl;

  Document d;
  rapidjson::Document::AllocatorType &alloc = d.GetAllocator();
  // d.AddMember("timestamp", "timestamp", alloc);

  Value val;
  val.SetObject();
  {
    val.AddMember("member", 1, alloc);
  }

  d.SetObject();
  d.AddMember("obj2", 2, alloc);
  d.AddMember("obj1", val, alloc);
  d.SetObject();
  d.AddMember("obj3", val, alloc);

  // d.CopyFrom(val, alloc);
  StringBuffer s_;
  Writer<StringBuffer> writer(s_);
  d.Accept(writer);
  cout << s_.GetString() << endl;

  return 0;
}

Value set_string(char *str)
{
  Value v;
  v.SetString(StringRef(str));
  return v;
}