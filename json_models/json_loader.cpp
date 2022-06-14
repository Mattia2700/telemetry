#include "json_loader.h"

template <>
bool CheckJson(const GPS_t& obj, const rapidjson::Document& doc)
{
    bool check = true;
    if(!doc.HasMember("Messages")){
        std::cout << "GPS_t MISSING FIELD: Messages" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("Average_Frequency_Hz")){
        std::cout << "GPS_t MISSING FIELD: Average_Frequency_Hz" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("Duration_seconds")){
        std::cout << "GPS_t MISSING FIELD: Duration_seconds" << std::endl; 
        check = false;
    }
    return check;
}

template<>
void Serialize(rapidjson::Value& out, const GPS_t& obj, rapidjson::Document::AllocatorType& alloc)
{
    out.SetObject();
    out.AddMember("Messages", rapidjson::Value().SetInt(obj.Messages), alloc);
    out.AddMember("Average_Frequency_Hz", rapidjson::Value().SetInt(obj.Average_Frequency_Hz), alloc);
    out.AddMember("Duration_seconds", rapidjson::Value().SetDouble(obj.Duration_seconds), alloc);
}
template<>
void Deserialize(GPS_t& obj, rapidjson::Value& doc)
{
    obj.Messages = doc["Messages"].GetInt();
    obj.Average_Frequency_Hz = doc["Average_Frequency_Hz"].GetInt();
    obj.Duration_seconds = doc["Duration_seconds"].GetDouble();
}

template <>
bool CheckJson(const CAN_t& obj, const rapidjson::Document& doc)
{
    bool check = true;
    if(!doc.HasMember("Messages")){
        std::cout << "CAN_t MISSING FIELD: Messages" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("Average_Frequency_Hz")){
        std::cout << "CAN_t MISSING FIELD: Average_Frequency_Hz" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("Duration_seconds")){
        std::cout << "CAN_t MISSING FIELD: Duration_seconds" << std::endl; 
        check = false;
    }
    return check;
}

template<>
void Serialize(rapidjson::Value& out, const CAN_t& obj, rapidjson::Document::AllocatorType& alloc)
{
    out.SetObject();
    out.AddMember("Messages", rapidjson::Value().SetInt(obj.Messages), alloc);
    out.AddMember("Average_Frequency_Hz", rapidjson::Value().SetInt(obj.Average_Frequency_Hz), alloc);
    out.AddMember("Duration_seconds", rapidjson::Value().SetDouble(obj.Duration_seconds), alloc);
}
template<>
void Deserialize(CAN_t& obj, rapidjson::Value& doc)
{
    obj.Messages = doc["Messages"].GetInt();
    obj.Average_Frequency_Hz = doc["Average_Frequency_Hz"].GetInt();
    obj.Duration_seconds = doc["Duration_seconds"].GetDouble();
}

template <>
bool CheckJson(const telemetry_config& obj, const rapidjson::Document& doc)
{
    bool check = true;
    if(!doc.HasMember("camera_enable")){
        std::cout << "telemetry_config MISSING FIELD: camera_enable" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("can_device")){
        std::cout << "telemetry_config MISSING FIELD: can_device" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("generate_csv")){
        std::cout << "telemetry_config MISSING FIELD: generate_csv" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("gps_devices")){
        std::cout << "telemetry_config MISSING FIELD: gps_devices" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("gps_enabled")){
        std::cout << "telemetry_config MISSING FIELD: gps_enabled" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("gps_mode")){
        std::cout << "telemetry_config MISSING FIELD: gps_mode" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("ws_downsample")){
        std::cout << "telemetry_config MISSING FIELD: ws_downsample" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("ws_downsample_mps")){
        std::cout << "telemetry_config MISSING FIELD: ws_downsample_mps" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("ws_enabled")){
        std::cout << "telemetry_config MISSING FIELD: ws_enabled" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("ws_send_rate")){
        std::cout << "telemetry_config MISSING FIELD: ws_send_rate" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("ws_send_sensor_data")){
        std::cout << "telemetry_config MISSING FIELD: ws_send_sensor_data" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("ws_server_url")){
        std::cout << "telemetry_config MISSING FIELD: ws_server_url" << std::endl; 
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
    out.AddMember("can_device", rapidjson::Value().SetString(obj.can_device.c_str(), obj.can_device.size(), alloc), alloc);
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
    obj.camera_enable = doc["camera_enable"].GetBool();
    obj.can_device = doc["can_device"].GetString();
    obj.generate_csv = doc["generate_csv"].GetBool();
	obj.gps_devices.resize(doc["gps_devices"].Size());
	for(rapidjson::SizeType i = 0; i < doc["gps_devices"].Size(); i++){
		obj.gps_devices[i] = doc["gps_devices"][i].GetString();
	}
	obj.gps_enabled.resize(doc["gps_enabled"].Size());
	for(rapidjson::SizeType i = 0; i < doc["gps_enabled"].Size(); i++){
		obj.gps_enabled[i] = doc["gps_enabled"][i].GetBool();
	}
	obj.gps_mode.resize(doc["gps_mode"].Size());
	for(rapidjson::SizeType i = 0; i < doc["gps_mode"].Size(); i++){
		obj.gps_mode[i] = doc["gps_mode"][i].GetString();
	}
    obj.ws_downsample = doc["ws_downsample"].GetBool();
    obj.ws_downsample_mps = doc["ws_downsample_mps"].GetInt();
    obj.ws_enabled = doc["ws_enabled"].GetBool();
    obj.ws_send_rate = doc["ws_send_rate"].GetInt();
    obj.ws_send_sensor_data = doc["ws_send_sensor_data"].GetBool();
    obj.ws_server_url = doc["ws_server_url"].GetString();
}
template<>
void Deserialize(telemetry_config& obj, rapidjson::Value& doc)
{
    obj.camera_enable = doc["camera_enable"].GetBool();
    obj.can_device = doc["can_device"].GetString();
    obj.generate_csv = doc["generate_csv"].GetBool();
	obj.gps_devices.resize(doc["gps_devices"].Size());
	for(rapidjson::SizeType i = 0; i < doc["gps_devices"].Size(); i++){
		obj.gps_devices[i] = doc["gps_devices"][i].GetString();
	}
	obj.gps_enabled.resize(doc["gps_enabled"].Size());
	for(rapidjson::SizeType i = 0; i < doc["gps_enabled"].Size(); i++){
		obj.gps_enabled[i] = doc["gps_enabled"][i].GetBool();
	}
	obj.gps_mode.resize(doc["gps_mode"].Size());
	for(rapidjson::SizeType i = 0; i < doc["gps_mode"].Size(); i++){
		obj.gps_mode[i] = doc["gps_mode"][i].GetString();
	}
    obj.ws_downsample = doc["ws_downsample"].GetBool();
    obj.ws_downsample_mps = doc["ws_downsample_mps"].GetInt();
    obj.ws_enabled = doc["ws_enabled"].GetBool();
    obj.ws_send_rate = doc["ws_send_rate"].GetInt();
    obj.ws_send_sensor_data = doc["ws_send_sensor_data"].GetBool();
    obj.ws_server_url = doc["ws_server_url"].GetString();
}
template<>
bool LoadStruct(telemetry_config& out, const std::string& path)
{
    rapidjson::Document doc;
    LoadJSON(doc, path);
    if(!CheckJson(out, doc))
        return false;
    Deserialize(out, doc);
    return true;
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
        std::cout << "session_config MISSING FIELD: Circuit" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("Pilot")){
        std::cout << "session_config MISSING FIELD: Pilot" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("Race")){
        std::cout << "session_config MISSING FIELD: Race" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("Configuration")){
        std::cout << "session_config MISSING FIELD: Configuration" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("Date")){
        std::cout << "session_config MISSING FIELD: Date" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("Time")){
        std::cout << "session_config MISSING FIELD: Time" << std::endl; 
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
}
template<>
void Deserialize(session_config& obj, rapidjson::Document& doc)
{
    obj.Circuit = doc["Circuit"].GetString();
    obj.Pilot = doc["Pilot"].GetString();
    obj.Race = doc["Race"].GetString();
    obj.Configuration = doc["Configuration"].GetString();
    obj.Date = doc["Date"].GetString();
    obj.Time = doc["Time"].GetString();
}
template<>
void Deserialize(session_config& obj, rapidjson::Value& doc)
{
    obj.Circuit = doc["Circuit"].GetString();
    obj.Pilot = doc["Pilot"].GetString();
    obj.Race = doc["Race"].GetString();
    obj.Configuration = doc["Configuration"].GetString();
    obj.Date = doc["Date"].GetString();
    obj.Time = doc["Time"].GetString();
}
template<>
bool LoadStruct(session_config& out, const std::string& path)
{
    rapidjson::Document doc;
    LoadJSON(doc, path);
    if(!CheckJson(out, doc))
        return false;
    Deserialize(out, doc);
    return true;
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
        std::cout << "csv_parser_config MISSING FIELD: subfolder_name" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("parse_candump")){
        std::cout << "csv_parser_config MISSING FIELD: parse_candump" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("parse_gps")){
        std::cout << "csv_parser_config MISSING FIELD: parse_gps" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("generate_report")){
        std::cout << "csv_parser_config MISSING FIELD: generate_report" << std::endl; 
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
    obj.subfolder_name = doc["subfolder_name"].GetString();
    obj.parse_candump = doc["parse_candump"].GetBool();
    obj.parse_gps = doc["parse_gps"].GetBool();
    obj.generate_report = doc["generate_report"].GetBool();
}
template<>
void Deserialize(csv_parser_config& obj, rapidjson::Value& doc)
{
    obj.subfolder_name = doc["subfolder_name"].GetString();
    obj.parse_candump = doc["parse_candump"].GetBool();
    obj.parse_gps = doc["parse_gps"].GetBool();
    obj.generate_report = doc["generate_report"].GetBool();
}
template<>
bool LoadStruct(csv_parser_config& out, const std::string& path)
{
    rapidjson::Document doc;
    LoadJSON(doc, path);
    if(!CheckJson(out, doc))
        return false;
    Deserialize(out, doc);
    return true;
}
template<>
void SaveStruct(const csv_parser_config& obj, const std::string& path)
{
    rapidjson::Document doc;
    Serialize(doc, obj);
    SaveJSON(doc, path);
}

template <>
bool CheckJson(const can_stat_json& obj, const rapidjson::Document& doc)
{
    bool check = true;
    if(!doc.HasMember("Date")){
        std::cout << "can_stat_json MISSING FIELD: Date" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("Circuit")){
        std::cout << "can_stat_json MISSING FIELD: Circuit" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("Pilot")){
        std::cout << "can_stat_json MISSING FIELD: Pilot" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("Race")){
        std::cout << "can_stat_json MISSING FIELD: Race" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("Configuration")){
        std::cout << "can_stat_json MISSING FIELD: Configuration" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("CAN")){
        std::cout << "can_stat_json MISSING FIELD: CAN" << std::endl; 
        check = false;
    }
    return check;
}

template<>
void Serialize(rapidjson::Document& out, const can_stat_json& obj)
{
    out.SetObject();
    rapidjson::Document::AllocatorType& alloc = out.GetAllocator();
    out.AddMember("Date", rapidjson::Value().SetString(obj.Date.c_str(), obj.Date.size(), alloc), alloc);
    out.AddMember("Circuit", rapidjson::Value().SetString(obj.Circuit.c_str(), obj.Circuit.size(), alloc), alloc);
    out.AddMember("Pilot", rapidjson::Value().SetString(obj.Pilot.c_str(), obj.Pilot.size(), alloc), alloc);
    out.AddMember("Race", rapidjson::Value().SetString(obj.Race.c_str(), obj.Race.size(), alloc), alloc);
    out.AddMember("Configuration", rapidjson::Value().SetString(obj.Configuration.c_str(), obj.Configuration.size(), alloc), alloc);
    {
        rapidjson::Value v;
        Serialize(v, obj.CAN, alloc);
        out.AddMember("CAN", v, alloc);
    }
}
template<>
void Deserialize(can_stat_json& obj, rapidjson::Document& doc)
{
    obj.Date = doc["Date"].GetString();
    obj.Circuit = doc["Circuit"].GetString();
    obj.Pilot = doc["Pilot"].GetString();
    obj.Race = doc["Race"].GetString();
    obj.Configuration = doc["Configuration"].GetString();
    Deserialize(obj.CAN, doc["CAN"]);
}
template<>
void Deserialize(can_stat_json& obj, rapidjson::Value& doc)
{
    obj.Date = doc["Date"].GetString();
    obj.Circuit = doc["Circuit"].GetString();
    obj.Pilot = doc["Pilot"].GetString();
    obj.Race = doc["Race"].GetString();
    obj.Configuration = doc["Configuration"].GetString();
    Deserialize(obj.CAN, doc["CAN"]);
}
template<>
bool LoadStruct(can_stat_json& out, const std::string& path)
{
    rapidjson::Document doc;
    LoadJSON(doc, path);
    if(!CheckJson(out, doc))
        return false;
    Deserialize(out, doc);
    return true;
}
template<>
void SaveStruct(const can_stat_json& obj, const std::string& path)
{
    rapidjson::Document doc;
    Serialize(doc, obj);
    SaveJSON(doc, path);
}

template <>
bool CheckJson(const gps_stat_json& obj, const rapidjson::Document& doc)
{
    bool check = true;
    if(!doc.HasMember("Date")){
        std::cout << "gps_stat_json MISSING FIELD: Date" << std::endl; 
        check = false;
    }
    if(!doc.HasMember("GPS")){
        std::cout << "gps_stat_json MISSING FIELD: GPS" << std::endl; 
        check = false;
    }
    return check;
}

template<>
void Serialize(rapidjson::Document& out, const gps_stat_json& obj)
{
    out.SetObject();
    rapidjson::Document::AllocatorType& alloc = out.GetAllocator();
    out.AddMember("Date", rapidjson::Value().SetString(obj.Date.c_str(), obj.Date.size(), alloc), alloc);
    {
        rapidjson::Value v;
        Serialize(v, obj.GPS, alloc);
        out.AddMember("GPS", v, alloc);
    }
}
template<>
void Deserialize(gps_stat_json& obj, rapidjson::Document& doc)
{
    obj.Date = doc["Date"].GetString();
    Deserialize(obj.GPS, doc["GPS"]);
}
template<>
void Deserialize(gps_stat_json& obj, rapidjson::Value& doc)
{
    obj.Date = doc["Date"].GetString();
    Deserialize(obj.GPS, doc["GPS"]);
}
template<>
bool LoadStruct(gps_stat_json& out, const std::string& path)
{
    rapidjson::Document doc;
    LoadJSON(doc, path);
    if(!CheckJson(out, doc))
        return false;
    Deserialize(out, doc);
    return true;
}
template<>
void SaveStruct(const gps_stat_json& obj, const std::string& path)
{
    rapidjson::Document doc;
    Serialize(doc, obj);
    SaveJSON(doc, path);
}

void LoadJSON(rapidjson::Document& out, const std::string& path)
{
    std::ifstream f(path);
    std::stringstream buffer;
    buffer << f.rdbuf();
    out.Parse(buffer.str().c_str());
}
void SaveJSON(const rapidjson::Document& doc, const std::string& path)
{
    FILE* fp = fopen(path.c_str(), "w");
 
    char writeBuffer[65536];
    rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
    
    rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
    doc.Accept(writer);
    
    fclose(fp);
}