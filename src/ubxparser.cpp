#include "ubxparser.h"

bool parse_ubx_line(const string &line, UBX_MSG_PVT &msg)
{
  UBX_MSG_MATCH match{0x01, 0x07, sizeof(UBX_MSG_PVT)};

  const uint8_t *buff = (uint8_t *)line.c_str();
  int idx = -1;
  for (int i = 0; i < line.size(); i++)
  {
    if (buff[i] == 0xb5 && buff[i + 1] == 0x62)
    {
      if (buff[i + 2] == match.msgClass && buff[i + 3] == match.msgID)
      {
        uint16_t sz = (buff[i + 5] << 8) | buff[i + 4];
        if (sz == (match.length))
        {
          idx = i + 2;
        }
      }
    }
  }
  if (idx == -1)
    return false;

  // Checksum
  uint8_t a = 0;
  uint8_t b = 0;
  for (int i = 0; i < (sizeof(UBX_MSG_MATCH) + sizeof(UBX_MSG_PVT)); i++)
  {
    a += buff[i + idx];
    b += a;
  }
  // Check of checksum
  if (a != buff[sizeof(UBX_MSG_MATCH) + sizeof(UBX_MSG_PVT) + idx] ||
      b != buff[sizeof(UBX_MSG_MATCH) + sizeof(UBX_MSG_PVT) + idx + 1])
    return false;

  msg = *(UBX_MSG_PVT *)(&buff[idx + sizeof(UBX_MSG_MATCH)]);

  return true;
}

uint16_t reverse16(const uint16_t &in)
{
  uint16_t a = ((in >> 8) & 0xFF);
  a |= (in & 0xFF) << 8;
  return a;
}

uint32_t reverse32(const uint32_t &in)
{
  uint32_t a = ((in >> 24) & 0xFF);
  a |= ((in >> 16) & 0xFF) << 8;
  a |= ((in >> 8) & 0xFF) << 16;
  a |= (in & 0xFF) << 24;
  return a;
}

int16_t reversei16(const int16_t &in)
{
  int16_t a = ((in >> 8) & 0xFF);
  a |= (in & 0xFF) << 8;
  return a;
}
int32_t reversei32(const int32_t &in)
{
  int32_t a = ((in >> 24) & 0xFF);
  a |= ((in >> 16) & 0xFF) << 8;
  a |= ((in >> 8) & 0xFF) << 16;
  a |= (in & 0xFF) << 24;
  return a;
}

int parse_gps(Gps *gps_, const double &timestamp, string &line)
{

  UBX_MSG_PVT msg;
  if (parse_ubx_line(line, msg))
  {
    gps_->clear();
    gps_->timestamp = timestamp;
    gps_->msg_type = "UBX";
    // gps_->latitude = ((double)msg.lat) * 10e-8;
    // gps_->longitude = ((double)msg.lon) * 10e-8;
    // From this message the coord are kinda different to check
    gps_->latitude = 0.0;
    gps_->longitude = 0.0;

    gps_->altitude = msg.hMSL * 10e-4;
    gps_->speed_kmh = msg.gSpeed * 3.6 * 10e-4;
    gps_->heading_motion = ((double)msg.headMot) * 10e-6;
    gps_->speed_accuracy = msg.sAcc * 3.6 * 10e-4;
    gps_->heading_accuracy_estimate = ((double)msg.headAcc) * 10e-6;
    gps_->position_diluition_precision = ((double)msg.pDOP) * 0.01;
    gps_->age_of_correction = (uint8_t)(msg.flags3 & 0b0000000000011110);
    gps_->heading_vehicle = ((double)msg.headVeh) * 10e-6;
    return 1;
  }

  auto idx = line.find('$');
  if (idx == string::npos)
    return -1;
  if (idx > 0)
    line = line.substr(idx, line.size() - idx);
  if (line[0] != '$')
    return -1;

  vector<string> s_line = split(line, ',');

  if (s_line[0].size() != 6)
    return -2;

  // remove $GP or $GN
  s_line[0].erase(0, 3);

  if (s_line[0] == "GGA")
  {
    if (s_line.size() != 15)
      return -6;
    // check if needed fileds are empty
    int ret = empty_fields(s_line, vector<int>{1, 2, 4, 6, 7, 9});
    if (ret != -1)
      return -3;

    gps_->clear();

    gps_->timestamp = timestamp;
    gps_->msg_type = "GGA";

    gps_->time = s_line[1];
    gps_->latitude = stod(s_line[2]) / 100.0;
    gps_->longitude = stod(s_line[4]) / 100.0;
    gps_->fix = stoi(s_line[6]);
    gps_->satellites = stoi(s_line[7]);
    gps_->horizontal_diluition_precision = stod(s_line[8]);
    gps_->fix_state = FIX_STATE[gps_->fix];
    gps_->altitude = stod(s_line[9]);
    if (s_line[13] != "")
    {
      gps_->age_of_correction = stod(s_line[13]);
    }

    return 1;
  }
  else if (s_line[0] == "VTG")
  {
    if (s_line.size() != 10)
      return -5;

    gps_->clear();

    gps_->timestamp = timestamp;
    gps_->msg_type = "VTG";

    if (s_line[1] != "")
      gps_->course_over_ground_degrees = stod(s_line[1]);
    else
      gps_->course_over_ground_degrees = 0.0;
    if (s_line[3] != "")
      gps_->course_over_ground_degrees_magnetic = stod(s_line[3]);
    else
      gps_->course_over_ground_degrees_magnetic = 0.0;
    if (s_line[7] != "")
      gps_->speed_kmh = stod(s_line[7]);
    else
      gps_->speed_kmh = 0.0;

    return 1;
  }
  else if (s_line[0] == "GSA")
  {
    if (s_line.size() != 19)
      return -5;

    gps_->clear();

    gps_->timestamp = timestamp;
    gps_->msg_type = "GSA";
    gps_->mode = FIX_MODE[stoi(s_line[2]) - 1];
    gps_->position_diluition_precision = stod(s_line[15]);
    gps_->horizontal_diluition_precision = stod(s_line[16]);
    gps_->vertical_diluition_precision = stod(s_line[17]);
    return 1;
  }

  return 0;
}