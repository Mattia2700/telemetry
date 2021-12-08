#pragma once

#include "proto_deserializer.h"

class Data{};
class Page;

class ChimeraData : public Data
{
public:
  ChimeraData(ChimeraDeserializer* chim): data(chim){};

  ChimeraDeserializer* data = nullptr;
  
  bool newData;
};