#include "ubxparser.h"


bool parse_ubx_line(const string& line, UBX_MSG_PVT& msg)
{
  UBX_MSG_MATCH match {0x01, 0x07, sizeof(UBX_MSG_PVT)};

  const uint8_t* buff = (uint8_t*)line.c_str();
  int idx = -1;
  for(int i = 0; i < line.size(); i++)
  {
    if(buff[i] == 0xb5 && buff[i+1] == 0x62)
    {
      if(buff[i + 2] == match.msgClass && buff[i + 3] == match.msgID)
      {
        uint16_t sz = (buff[i + 5] << 8) | buff[i + 4];
        if(sz == (match.length))
        {
          idx = i + 2;
        }
      }
    }
  }
  if(idx == -1)
    return false;
  

  // Checksum
  uint8_t a = 0;
  uint8_t b = 0;
  for(int i = 0; i < (sizeof(UBX_MSG_MATCH) + sizeof(UBX_MSG_PVT)); i++)
  {
    a += buff[i + idx];
    b += a;
  }
  // Check of checksum
  if(a != buff[sizeof(UBX_MSG_MATCH) + sizeof(UBX_MSG_PVT) + idx    ] ||
     b != buff[sizeof(UBX_MSG_MATCH) + sizeof(UBX_MSG_PVT) + idx + 1])
    return false;
  
  msg = *(UBX_MSG_PVT*)(&buff[idx + sizeof(UBX_MSG_MATCH)]);
  // msg.year    = reverse16(msg.year);
  // msg.pDOP    = reverse16(msg.pDOP);
  // msg.magAcc  = reverse16(msg.magAcc);
  // msg.iTOW    = reverse32(msg.iTOW);
  // msg.tAcc    = reverse32(msg.tAcc);
  // msg.hAcc    = reverse32(msg.hAcc);
  // msg.vAcc    = reverse32(msg.vAcc);
  // msg.sAcc    = reverse32(msg.sAcc);
  // msg.headAcc = reverse32(msg.headAcc);
  // msg.nano    = reversei32(msg.nano);
  // // msg.lon     = reversei32(msg.lon);
  // // msg.lat     = reversei32(msg.lat);
  // msg.height  = reversei32(msg.height);
  // msg.hMSL    = reversei32(msg.hMSL);
  // msg.velN    = reversei32(msg.velN);
  // msg.velE    = reversei32(msg.velE);
  // msg.velD    = reversei32(msg.velD);
  // msg.gSpeed  = reversei32(msg.gSpeed);
  // msg.headMot = reversei32(msg.headMot);
  // msg.headVeh = reversei32(msg.headVeh);
  // msg.magDec  = reversei16(msg.magDec);

  return true;
}

uint16_t reverse16(const uint16_t& in)
{
  uint16_t a = ((in >> 8) & 0xFF);
  a |= (in & 0xFF) << 8;
  return a;
}

uint32_t reverse32(const uint32_t& in)
{
  uint32_t a = ((in >> 24) & 0xFF);
  a |= ((in >> 16) & 0xFF) << 8;
  a |= ((in >> 8) & 0xFF) << 16;
  a |= (in & 0xFF) << 24;
  return a;
}

int16_t reversei16(const int16_t& in)
{
  int16_t a = ((in >> 8) & 0xFF);
  a |= (in & 0xFF) << 8;
  return a;
}
int32_t reversei32(const int32_t& in)
{
  int32_t a = ((in >> 24) & 0xFF);
  a |= ((in >> 16) & 0xFF) << 8;
  a |= ((in >> 8) & 0xFF) << 16;
  a |= (in & 0xFF) << 24;
  return a;
}