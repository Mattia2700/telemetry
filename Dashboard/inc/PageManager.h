#pragma once

#include "Page.h"

class PageManager
{


private:
  vector<Page *> m_Pages;

  int m_PageIndex;

  bool OnEvent(Event &e);

};