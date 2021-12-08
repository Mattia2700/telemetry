#pragma once

#include "devices.pb.h"


class Data{};
class Page;

class ChimeraData : public Data
{
public:
  ChimeraData(devices::Chimera* chim): data(chim){};

  void SetData(devices::Chimera* chim)
  {
    data = chim;
  };

  devices::Chimera* data = nullptr;

private:
};