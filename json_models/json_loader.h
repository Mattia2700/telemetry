#ifndef __JSON_LOADER__
#define __JSON_LOADER__

#include <string>
#include <vector>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <iostream>

#include "rapidjson/filewritestream.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"

typedef struct GPS_t{
    int Messages;
    int Average_Frequency_Hz;
    float Duration_seconds;
}GPS_t;

typedef struct CAN_t{
    int Messages;
    int Average_Frequency_Hz;
    float Duration_seconds;
}CAN_t;

typedef struct can_devices_o{
    std::string sock;
    std::string name;
}can_devices_o;

typedef struct telemetry_config{
    bool camera_enable;
    std::vector<can_devices_o> can_devices;
    bool generate_csv;
    std::vector<std::string> gps_devices;
    std::vector<bool> gps_enabled;
    std::vector<std::string> gps_mode;
    bool ws_downsample;
    int ws_downsample_mps;
    bool ws_enabled;
    int ws_send_rate;
    bool ws_send_sensor_data;
    std::string ws_server_url;
}telemetry_config;

typedef struct session_config{
    std::string Circuit;
    std::string Pilot;
    std::string Race;
    std::string Configuration;
    std::string Date;
    std::string Time;
}session_config;

typedef struct csv_parser_config{
    std::string subfolder_name;
    bool parse_candump;
    bool parse_gps;
    bool generate_report;
}csv_parser_config;

typedef struct can_stat_json{
    std::string Date;
    std::string Circuit;
    std::string Pilot;
    std::string Race;
    std::string Configuration;
    CAN_t CAN;
}can_stat_json;

typedef struct gps_stat_json{
    std::string Date;
    GPS_t GPS;
}gps_stat_json;

// T is a struct
// J is a rapidjson::Document or a rapidjson::Value
template<class T, class J>
bool CheckJson(const T& obj, const J& doc);

// T is a struct
// J is a rapidjson::Document or a rapidjson::Value
template<class T>
void Serialize(rapidjson::Document& out, const T& obj);
template<class T>
void Serialize(rapidjson::Value& out, const T& obj, rapidjson::Document::AllocatorType& alloc);
template<class T, class J>
void Deserialize(T& obj, J& doc);

// T is a struct
// J is a rapidjson::Document or a rapidjson::Value
template<class T>
bool LoadStruct(T& out, const std::string& path);
template<class T>
void SaveStruct(const T& obj, const std::string& path);

void LoadJSON(rapidjson::Document& out, const std::string& path);
void SaveJSON(const rapidjson::Document& obj, const std::string& path);

#endif // __JSON_LOADER__