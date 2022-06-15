#ifndef __JSON_LOADER__
#define __JSON_LOADER__

#include <string>
#include <vector>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <iostream>


#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"

#ifndef JSON_LOG_FUNC
#define JSON_LOG_FUNC(msg) std::cout << msg << std::endl;
#endif

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
    float Canlib_Version;
}session_config;

typedef struct csv_parser_config{
    std::string subfolder_name;
    bool parse_candump;
    bool parse_gps;
    bool generate_report;
}csv_parser_config;

typedef struct stat_json{
    int Messages;
    int Average_Frequency_Hz;
    float Duration_seconds;
}stat_json;

#ifndef __JSON_LOADER_DEFINITION__
#define __JSON_LOADER_DEFINITION__

// T is a struct
// J is a rapidjson::Document or a rapidjson::Value
template<class T, class J>
bool CheckJson(const T& obj, const J& doc);

// T is a struct
// J is a rapidjson::Document or a rapidjson::Value
template<class T, class J>
void Deserialize(T& obj, J& doc);
template<class T>
void Serialize(rapidjson::Document& out, const T& obj);
template<class T>
void Serialize(rapidjson::Value& out, const T& obj, rapidjson::Document::AllocatorType& alloc);

// T is a struct
// J is a rapidjson::Document or a rapidjson::Value
template<class T>
bool LoadStruct(T& out, const std::string& path);
template<class T>
void SaveStruct(const T& obj, const std::string& path);

// If main struct
template<class T>
std::string JsonToString(const T& obj);

static void LoadJSON(rapidjson::Document& out, const std::string& path)
{
    std::ifstream f(path);
    std::stringstream buffer;
    buffer << f.rdbuf();
    out.Parse(buffer.str().c_str());
}
static void SaveJSON(const rapidjson::Document& doc, const std::string& path)
{
    char writeBuffer[65536];
    FILE* fp = fopen(path.c_str(), "w");
    rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
    rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
    doc.Accept(writer);
    fclose(fp);
}

#endif // __JSON_LOADER_DEFINITION__


template <>
bool CheckJson(const can_devices_o& obj, const rapidjson::Document& doc)
{
    bool check = true;
    if(!doc.HasMember("sock")){
        JSON_LOG_FUNC("can_devices_o MISSING FIELD: sock"); 
        check = false;
    }
    if(!doc.HasMember("name")){
        JSON_LOG_FUNC("can_devices_o MISSING FIELD: name"); 
        check = false;
    }
    return check;
}

template<>
void Serialize(rapidjson::Value& out, const can_devices_o& obj, rapidjson::Document::AllocatorType& alloc)
{
    out.SetObject();
    out.AddMember("sock", rapidjson::Value().SetString(obj.sock.c_str(), obj.sock.size(), alloc), alloc);
    out.AddMember("name", rapidjson::Value().SetString(obj.name.c_str(), obj.name.size(), alloc), alloc);
}
template<>
void Deserialize(can_devices_o& obj, rapidjson::Value& doc)
{
    if(!doc.HasMember("sock")){
        JSON_LOG_FUNC("can_devices_o MISSING FIELD: sock"); 
    }else{
        obj.sock = doc["sock"].GetString();
    }
    if(!doc.HasMember("name")){
        JSON_LOG_FUNC("can_devices_o MISSING FIELD: name"); 
    }else{
        obj.name = doc["name"].GetString();
    }
}

template <>
bool CheckJson(const telemetry_config& obj, const rapidjson::Document& doc)
{
    bool check = true;
    if(!doc.HasMember("camera_enable")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: camera_enable"); 
        check = false;
    }
    if(!doc.HasMember("can_devices")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: can_devices"); 
        check = false;
    }
    if(!doc.HasMember("generate_csv")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: generate_csv"); 
        check = false;
    }
    if(!doc.HasMember("gps_devices")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: gps_devices"); 
        check = false;
    }
    if(!doc.HasMember("gps_enabled")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: gps_enabled"); 
        check = false;
    }
    if(!doc.HasMember("gps_mode")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: gps_mode"); 
        check = false;
    }
    if(!doc.HasMember("ws_downsample")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_downsample"); 
        check = false;
    }
    if(!doc.HasMember("ws_downsample_mps")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_downsample_mps"); 
        check = false;
    }
    if(!doc.HasMember("ws_enabled")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_enabled"); 
        check = false;
    }
    if(!doc.HasMember("ws_send_rate")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_send_rate"); 
        check = false;
    }
    if(!doc.HasMember("ws_send_sensor_data")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_send_sensor_data"); 
        check = false;
    }
    if(!doc.HasMember("ws_server_url")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_server_url"); 
        check = false;
    }
    return check;
}

template<>
void Serialize(rapidjson::Document& out, const telemetry_config& obj)
{
    out.SetObject();
    rapidjson::Document::AllocatorType& alloc = out.GetAllocator();
    out.AddMember("camera_enable", rapidjson::Value().SetBool(obj.camera_enable), alloc);
    {
        rapidjson::Value v0;
        v0.SetArray();
        for(size_t i = 0; i < obj.can_devices.size(); i++){
        	rapidjson::Value new_obj;
        	Serialize(new_obj, obj.can_devices[i], alloc);
        	v0.PushBack(new_obj, alloc);
    	}
    	out.AddMember("can_devices", v0, alloc);
    }
    out.AddMember("generate_csv", rapidjson::Value().SetBool(obj.generate_csv), alloc);
    {
        rapidjson::Value v0;
        v0.SetArray();
        for(size_t i = 0; i < obj.gps_devices.size(); i++){
        	v0.PushBack(rapidjson::Value().SetString(obj.gps_devices[i].c_str(), obj.gps_devices[i].size(), alloc), alloc);
    	}
    	out.AddMember("gps_devices", v0, alloc);
    }
    {
        rapidjson::Value v0;
        v0.SetArray();
        for(size_t i = 0; i < obj.gps_enabled.size(); i++){
        	v0.PushBack(rapidjson::Value().SetBool(obj.gps_enabled[i]), alloc);
    	}
    	out.AddMember("gps_enabled", v0, alloc);
    }
    {
        rapidjson::Value v0;
        v0.SetArray();
        for(size_t i = 0; i < obj.gps_mode.size(); i++){
        	v0.PushBack(rapidjson::Value().SetString(obj.gps_mode[i].c_str(), obj.gps_mode[i].size(), alloc), alloc);
    	}
    	out.AddMember("gps_mode", v0, alloc);
    }
    out.AddMember("ws_downsample", rapidjson::Value().SetBool(obj.ws_downsample), alloc);
    out.AddMember("ws_downsample_mps", rapidjson::Value().SetInt(obj.ws_downsample_mps), alloc);
    out.AddMember("ws_enabled", rapidjson::Value().SetBool(obj.ws_enabled), alloc);
    out.AddMember("ws_send_rate", rapidjson::Value().SetInt(obj.ws_send_rate), alloc);
    out.AddMember("ws_send_sensor_data", rapidjson::Value().SetBool(obj.ws_send_sensor_data), alloc);
    out.AddMember("ws_server_url", rapidjson::Value().SetString(obj.ws_server_url.c_str(), obj.ws_server_url.size(), alloc), alloc);
}
template<>
void Deserialize(telemetry_config& obj, rapidjson::Document& doc)
{
    if(!doc.HasMember("camera_enable")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: camera_enable"); 
    }else{
        obj.camera_enable = doc["camera_enable"].GetBool();
    }
    if(!doc.HasMember("can_devices")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: can_devices"); 
    }else{
	obj.can_devices.resize(doc["can_devices"].Size());
	for(rapidjson::SizeType i = 0; i < doc["can_devices"].Size(); i++){
		Deserialize(obj.can_devices[i], doc["can_devices"][i]);
	}
    }
    if(!doc.HasMember("generate_csv")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: generate_csv"); 
    }else{
        obj.generate_csv = doc["generate_csv"].GetBool();
    }
    if(!doc.HasMember("gps_devices")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: gps_devices"); 
    }else{
	obj.gps_devices.resize(doc["gps_devices"].Size());
	for(rapidjson::SizeType i = 0; i < doc["gps_devices"].Size(); i++){
		obj.gps_devices[i] = doc["gps_devices"][i].GetString();
	}
    }
    if(!doc.HasMember("gps_enabled")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: gps_enabled"); 
    }else{
	obj.gps_enabled.resize(doc["gps_enabled"].Size());
	for(rapidjson::SizeType i = 0; i < doc["gps_enabled"].Size(); i++){
		obj.gps_enabled[i] = doc["gps_enabled"][i].GetBool();
	}
    }
    if(!doc.HasMember("gps_mode")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: gps_mode"); 
    }else{
	obj.gps_mode.resize(doc["gps_mode"].Size());
	for(rapidjson::SizeType i = 0; i < doc["gps_mode"].Size(); i++){
		obj.gps_mode[i] = doc["gps_mode"][i].GetString();
	}
    }
    if(!doc.HasMember("ws_downsample")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_downsample"); 
    }else{
        obj.ws_downsample = doc["ws_downsample"].GetBool();
    }
    if(!doc.HasMember("ws_downsample_mps")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_downsample_mps"); 
    }else{
        obj.ws_downsample_mps = doc["ws_downsample_mps"].GetInt();
    }
    if(!doc.HasMember("ws_enabled")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_enabled"); 
    }else{
        obj.ws_enabled = doc["ws_enabled"].GetBool();
    }
    if(!doc.HasMember("ws_send_rate")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_send_rate"); 
    }else{
        obj.ws_send_rate = doc["ws_send_rate"].GetInt();
    }
    if(!doc.HasMember("ws_send_sensor_data")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_send_sensor_data"); 
    }else{
        obj.ws_send_sensor_data = doc["ws_send_sensor_data"].GetBool();
    }
    if(!doc.HasMember("ws_server_url")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_server_url"); 
    }else{
        obj.ws_server_url = doc["ws_server_url"].GetString();
    }
}
template<>
void Deserialize(telemetry_config& obj, rapidjson::Value& doc)
{
    if(!doc.HasMember("camera_enable")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: camera_enable"); 
    }else{
        obj.camera_enable = doc["camera_enable"].GetBool();
    }
    if(!doc.HasMember("can_devices")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: can_devices"); 
    }else{
	obj.can_devices.resize(doc["can_devices"].Size());
	for(rapidjson::SizeType i = 0; i < doc["can_devices"].Size(); i++){
		Deserialize(obj.can_devices[i], doc["can_devices"][i]);
	}
    }
    if(!doc.HasMember("generate_csv")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: generate_csv"); 
    }else{
        obj.generate_csv = doc["generate_csv"].GetBool();
    }
    if(!doc.HasMember("gps_devices")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: gps_devices"); 
    }else{
	obj.gps_devices.resize(doc["gps_devices"].Size());
	for(rapidjson::SizeType i = 0; i < doc["gps_devices"].Size(); i++){
		obj.gps_devices[i] = doc["gps_devices"][i].GetString();
	}
    }
    if(!doc.HasMember("gps_enabled")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: gps_enabled"); 
    }else{
	obj.gps_enabled.resize(doc["gps_enabled"].Size());
	for(rapidjson::SizeType i = 0; i < doc["gps_enabled"].Size(); i++){
		obj.gps_enabled[i] = doc["gps_enabled"][i].GetBool();
	}
    }
    if(!doc.HasMember("gps_mode")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: gps_mode"); 
    }else{
	obj.gps_mode.resize(doc["gps_mode"].Size());
	for(rapidjson::SizeType i = 0; i < doc["gps_mode"].Size(); i++){
		obj.gps_mode[i] = doc["gps_mode"][i].GetString();
	}
    }
    if(!doc.HasMember("ws_downsample")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_downsample"); 
    }else{
        obj.ws_downsample = doc["ws_downsample"].GetBool();
    }
    if(!doc.HasMember("ws_downsample_mps")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_downsample_mps"); 
    }else{
        obj.ws_downsample_mps = doc["ws_downsample_mps"].GetInt();
    }
    if(!doc.HasMember("ws_enabled")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_enabled"); 
    }else{
        obj.ws_enabled = doc["ws_enabled"].GetBool();
    }
    if(!doc.HasMember("ws_send_rate")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_send_rate"); 
    }else{
        obj.ws_send_rate = doc["ws_send_rate"].GetInt();
    }
    if(!doc.HasMember("ws_send_sensor_data")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_send_sensor_data"); 
    }else{
        obj.ws_send_sensor_data = doc["ws_send_sensor_data"].GetBool();
    }
    if(!doc.HasMember("ws_server_url")){
        JSON_LOG_FUNC("telemetry_config MISSING FIELD: ws_server_url"); 
    }else{
        obj.ws_server_url = doc["ws_server_url"].GetString();
    }
}

template<>
std::string JsonToString(const telemetry_config& obj)
{
    rapidjson::Document doc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> w(sb);
    Serialize(doc, obj);
    doc.Accept(w);
    return sb.GetString();
}

template<>
bool LoadStruct(telemetry_config& out, const std::string& path)
{
    rapidjson::Document doc;
    LoadJSON(doc, path);
    bool check_passed = CheckJson(out, doc);
    Deserialize(out, doc);
    return check_passed;
}
template<>
void SaveStruct(const telemetry_config& obj, const std::string& path)
{
    rapidjson::Document doc;
    Serialize(doc, obj);
    SaveJSON(doc, path);
}

template <>
bool CheckJson(const session_config& obj, const rapidjson::Document& doc)
{
    bool check = true;
    if(!doc.HasMember("Circuit")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Circuit"); 
        check = false;
    }
    if(!doc.HasMember("Pilot")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Pilot"); 
        check = false;
    }
    if(!doc.HasMember("Race")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Race"); 
        check = false;
    }
    if(!doc.HasMember("Configuration")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Configuration"); 
        check = false;
    }
    if(!doc.HasMember("Date")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Date"); 
        check = false;
    }
    if(!doc.HasMember("Time")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Time"); 
        check = false;
    }
    if(!doc.HasMember("Canlib_Version")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Canlib_Version"); 
        check = false;
    }
    return check;
}

template<>
void Serialize(rapidjson::Document& out, const session_config& obj)
{
    out.SetObject();
    rapidjson::Document::AllocatorType& alloc = out.GetAllocator();
    out.AddMember("Circuit", rapidjson::Value().SetString(obj.Circuit.c_str(), obj.Circuit.size(), alloc), alloc);
    out.AddMember("Pilot", rapidjson::Value().SetString(obj.Pilot.c_str(), obj.Pilot.size(), alloc), alloc);
    out.AddMember("Race", rapidjson::Value().SetString(obj.Race.c_str(), obj.Race.size(), alloc), alloc);
    out.AddMember("Configuration", rapidjson::Value().SetString(obj.Configuration.c_str(), obj.Configuration.size(), alloc), alloc);
    out.AddMember("Date", rapidjson::Value().SetString(obj.Date.c_str(), obj.Date.size(), alloc), alloc);
    out.AddMember("Time", rapidjson::Value().SetString(obj.Time.c_str(), obj.Time.size(), alloc), alloc);
    out.AddMember("Canlib_Version", rapidjson::Value().SetDouble(obj.Canlib_Version), alloc);
}
template<>
void Deserialize(session_config& obj, rapidjson::Document& doc)
{
    if(!doc.HasMember("Circuit")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Circuit"); 
    }else{
        obj.Circuit = doc["Circuit"].GetString();
    }
    if(!doc.HasMember("Pilot")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Pilot"); 
    }else{
        obj.Pilot = doc["Pilot"].GetString();
    }
    if(!doc.HasMember("Race")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Race"); 
    }else{
        obj.Race = doc["Race"].GetString();
    }
    if(!doc.HasMember("Configuration")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Configuration"); 
    }else{
        obj.Configuration = doc["Configuration"].GetString();
    }
    if(!doc.HasMember("Date")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Date"); 
    }else{
        obj.Date = doc["Date"].GetString();
    }
    if(!doc.HasMember("Time")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Time"); 
    }else{
        obj.Time = doc["Time"].GetString();
    }
    if(!doc.HasMember("Canlib_Version")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Canlib_Version"); 
    }else{
        obj.Canlib_Version = doc["Canlib_Version"].GetDouble();
    }
}
template<>
void Deserialize(session_config& obj, rapidjson::Value& doc)
{
    if(!doc.HasMember("Circuit")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Circuit"); 
    }else{
        obj.Circuit = doc["Circuit"].GetString();
    }
    if(!doc.HasMember("Pilot")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Pilot"); 
    }else{
        obj.Pilot = doc["Pilot"].GetString();
    }
    if(!doc.HasMember("Race")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Race"); 
    }else{
        obj.Race = doc["Race"].GetString();
    }
    if(!doc.HasMember("Configuration")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Configuration"); 
    }else{
        obj.Configuration = doc["Configuration"].GetString();
    }
    if(!doc.HasMember("Date")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Date"); 
    }else{
        obj.Date = doc["Date"].GetString();
    }
    if(!doc.HasMember("Time")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Time"); 
    }else{
        obj.Time = doc["Time"].GetString();
    }
    if(!doc.HasMember("Canlib_Version")){
        JSON_LOG_FUNC("session_config MISSING FIELD: Canlib_Version"); 
    }else{
        obj.Canlib_Version = doc["Canlib_Version"].GetDouble();
    }
}

template<>
std::string JsonToString(const session_config& obj)
{
    rapidjson::Document doc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> w(sb);
    Serialize(doc, obj);
    doc.Accept(w);
    return sb.GetString();
}

template<>
bool LoadStruct(session_config& out, const std::string& path)
{
    rapidjson::Document doc;
    LoadJSON(doc, path);
    bool check_passed = CheckJson(out, doc);
    Deserialize(out, doc);
    return check_passed;
}
template<>
void SaveStruct(const session_config& obj, const std::string& path)
{
    rapidjson::Document doc;
    Serialize(doc, obj);
    SaveJSON(doc, path);
}

template <>
bool CheckJson(const csv_parser_config& obj, const rapidjson::Document& doc)
{
    bool check = true;
    if(!doc.HasMember("subfolder_name")){
        JSON_LOG_FUNC("csv_parser_config MISSING FIELD: subfolder_name"); 
        check = false;
    }
    if(!doc.HasMember("parse_candump")){
        JSON_LOG_FUNC("csv_parser_config MISSING FIELD: parse_candump"); 
        check = false;
    }
    if(!doc.HasMember("parse_gps")){
        JSON_LOG_FUNC("csv_parser_config MISSING FIELD: parse_gps"); 
        check = false;
    }
    if(!doc.HasMember("generate_report")){
        JSON_LOG_FUNC("csv_parser_config MISSING FIELD: generate_report"); 
        check = false;
    }
    return check;
}

template<>
void Serialize(rapidjson::Document& out, const csv_parser_config& obj)
{
    out.SetObject();
    rapidjson::Document::AllocatorType& alloc = out.GetAllocator();
    out.AddMember("subfolder_name", rapidjson::Value().SetString(obj.subfolder_name.c_str(), obj.subfolder_name.size(), alloc), alloc);
    out.AddMember("parse_candump", rapidjson::Value().SetBool(obj.parse_candump), alloc);
    out.AddMember("parse_gps", rapidjson::Value().SetBool(obj.parse_gps), alloc);
    out.AddMember("generate_report", rapidjson::Value().SetBool(obj.generate_report), alloc);
}
template<>
void Deserialize(csv_parser_config& obj, rapidjson::Document& doc)
{
    if(!doc.HasMember("subfolder_name")){
        JSON_LOG_FUNC("csv_parser_config MISSING FIELD: subfolder_name"); 
    }else{
        obj.subfolder_name = doc["subfolder_name"].GetString();
    }
    if(!doc.HasMember("parse_candump")){
        JSON_LOG_FUNC("csv_parser_config MISSING FIELD: parse_candump"); 
    }else{
        obj.parse_candump = doc["parse_candump"].GetBool();
    }
    if(!doc.HasMember("parse_gps")){
        JSON_LOG_FUNC("csv_parser_config MISSING FIELD: parse_gps"); 
    }else{
        obj.parse_gps = doc["parse_gps"].GetBool();
    }
    if(!doc.HasMember("generate_report")){
        JSON_LOG_FUNC("csv_parser_config MISSING FIELD: generate_report"); 
    }else{
        obj.generate_report = doc["generate_report"].GetBool();
    }
}
template<>
void Deserialize(csv_parser_config& obj, rapidjson::Value& doc)
{
    if(!doc.HasMember("subfolder_name")){
        JSON_LOG_FUNC("csv_parser_config MISSING FIELD: subfolder_name"); 
    }else{
        obj.subfolder_name = doc["subfolder_name"].GetString();
    }
    if(!doc.HasMember("parse_candump")){
        JSON_LOG_FUNC("csv_parser_config MISSING FIELD: parse_candump"); 
    }else{
        obj.parse_candump = doc["parse_candump"].GetBool();
    }
    if(!doc.HasMember("parse_gps")){
        JSON_LOG_FUNC("csv_parser_config MISSING FIELD: parse_gps"); 
    }else{
        obj.parse_gps = doc["parse_gps"].GetBool();
    }
    if(!doc.HasMember("generate_report")){
        JSON_LOG_FUNC("csv_parser_config MISSING FIELD: generate_report"); 
    }else{
        obj.generate_report = doc["generate_report"].GetBool();
    }
}

template<>
std::string JsonToString(const csv_parser_config& obj)
{
    rapidjson::Document doc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> w(sb);
    Serialize(doc, obj);
    doc.Accept(w);
    return sb.GetString();
}

template<>
bool LoadStruct(csv_parser_config& out, const std::string& path)
{
    rapidjson::Document doc;
    LoadJSON(doc, path);
    bool check_passed = CheckJson(out, doc);
    Deserialize(out, doc);
    return check_passed;
}
template<>
void SaveStruct(const csv_parser_config& obj, const std::string& path)
{
    rapidjson::Document doc;
    Serialize(doc, obj);
    SaveJSON(doc, path);
}

template <>
bool CheckJson(const stat_json& obj, const rapidjson::Document& doc)
{
    bool check = true;
    if(!doc.HasMember("Messages")){
        JSON_LOG_FUNC("stat_json MISSING FIELD: Messages"); 
        check = false;
    }
    if(!doc.HasMember("Average_Frequency_Hz")){
        JSON_LOG_FUNC("stat_json MISSING FIELD: Average_Frequency_Hz"); 
        check = false;
    }
    if(!doc.HasMember("Duration_seconds")){
        JSON_LOG_FUNC("stat_json MISSING FIELD: Duration_seconds"); 
        check = false;
    }
    return check;
}

template<>
void Serialize(rapidjson::Document& out, const stat_json& obj)
{
    out.SetObject();
    rapidjson::Document::AllocatorType& alloc = out.GetAllocator();
    out.AddMember("Messages", rapidjson::Value().SetInt(obj.Messages), alloc);
    out.AddMember("Average_Frequency_Hz", rapidjson::Value().SetInt(obj.Average_Frequency_Hz), alloc);
    out.AddMember("Duration_seconds", rapidjson::Value().SetDouble(obj.Duration_seconds), alloc);
}
template<>
void Deserialize(stat_json& obj, rapidjson::Document& doc)
{
    if(!doc.HasMember("Messages")){
        JSON_LOG_FUNC("stat_json MISSING FIELD: Messages"); 
    }else{
        obj.Messages = doc["Messages"].GetInt();
    }
    if(!doc.HasMember("Average_Frequency_Hz")){
        JSON_LOG_FUNC("stat_json MISSING FIELD: Average_Frequency_Hz"); 
    }else{
        obj.Average_Frequency_Hz = doc["Average_Frequency_Hz"].GetInt();
    }
    if(!doc.HasMember("Duration_seconds")){
        JSON_LOG_FUNC("stat_json MISSING FIELD: Duration_seconds"); 
    }else{
        obj.Duration_seconds = doc["Duration_seconds"].GetDouble();
    }
}
template<>
void Deserialize(stat_json& obj, rapidjson::Value& doc)
{
    if(!doc.HasMember("Messages")){
        JSON_LOG_FUNC("stat_json MISSING FIELD: Messages"); 
    }else{
        obj.Messages = doc["Messages"].GetInt();
    }
    if(!doc.HasMember("Average_Frequency_Hz")){
        JSON_LOG_FUNC("stat_json MISSING FIELD: Average_Frequency_Hz"); 
    }else{
        obj.Average_Frequency_Hz = doc["Average_Frequency_Hz"].GetInt();
    }
    if(!doc.HasMember("Duration_seconds")){
        JSON_LOG_FUNC("stat_json MISSING FIELD: Duration_seconds"); 
    }else{
        obj.Duration_seconds = doc["Duration_seconds"].GetDouble();
    }
}

template<>
std::string JsonToString(const stat_json& obj)
{
    rapidjson::Document doc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> w(sb);
    Serialize(doc, obj);
    doc.Accept(w);
    return sb.GetString();
}

template<>
bool LoadStruct(stat_json& out, const std::string& path)
{
    rapidjson::Document doc;
    LoadJSON(doc, path);
    bool check_passed = CheckJson(out, doc);
    Deserialize(out, doc);
    return check_passed;
}
template<>
void SaveStruct(const stat_json& obj, const std::string& path)
{
    rapidjson::Document doc;
    Serialize(doc, obj);
    SaveJSON(doc, path);
}

#endif // __JSON_LOADER__