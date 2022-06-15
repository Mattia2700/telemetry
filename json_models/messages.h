#ifndef __MESSAGES__
#define __MESSAGES__

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

typedef struct msgs_per_second_o{
    std::string device;
    int count;
}msgs_per_second_o;

typedef struct get_telemetry_config{
    std::string type;
    std::string telemetry_config;
    std::string session_config;
}get_telemetry_config;

typedef struct telemetry_status{
    std::string type;
    float timestamp;
    int data;
    std::vector<msgs_per_second_o> msgs_per_second;
    std::string camera_status;
    std::string camera_error;
    int cpu_total_load;
    int cpu_process_load;
    int mem_process_load;
}telemetry_status;

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
bool CheckJson(const msgs_per_second_o& obj, const rapidjson::Document& doc)
{
    bool check = true;
    if(!doc.HasMember("device")){
        JSON_LOG_FUNC("msgs_per_second_o MISSING FIELD: device"); 
        check = false;
    }
    if(!doc.HasMember("count")){
        JSON_LOG_FUNC("msgs_per_second_o MISSING FIELD: count"); 
        check = false;
    }
    return check;
}

template<>
void Serialize(rapidjson::Value& out, const msgs_per_second_o& obj, rapidjson::Document::AllocatorType& alloc)
{
    out.SetObject();
    out.AddMember("device", rapidjson::Value().SetString(obj.device.c_str(), obj.device.size(), alloc), alloc);
    out.AddMember("count", rapidjson::Value().SetInt(obj.count), alloc);
}
template<>
void Deserialize(msgs_per_second_o& obj, rapidjson::Value& doc)
{
    if(!doc.HasMember("device")){
        JSON_LOG_FUNC("msgs_per_second_o MISSING FIELD: device"); 
    }else{
        obj.device = doc["device"].GetString();
    }
    if(!doc.HasMember("count")){
        JSON_LOG_FUNC("msgs_per_second_o MISSING FIELD: count"); 
    }else{
        obj.count = doc["count"].GetInt();
    }
}

template <>
bool CheckJson(const get_telemetry_config& obj, const rapidjson::Document& doc)
{
    bool check = true;
    if(!doc.HasMember("type")){
        JSON_LOG_FUNC("get_telemetry_config MISSING FIELD: type"); 
        check = false;
    }
    if(!doc.HasMember("telemetry_config")){
        JSON_LOG_FUNC("get_telemetry_config MISSING FIELD: telemetry_config"); 
        check = false;
    }
    if(!doc.HasMember("session_config")){
        JSON_LOG_FUNC("get_telemetry_config MISSING FIELD: session_config"); 
        check = false;
    }
    return check;
}

template<>
void Serialize(rapidjson::Document& out, const get_telemetry_config& obj)
{
    out.SetObject();
    rapidjson::Document::AllocatorType& alloc = out.GetAllocator();
    out.AddMember("type", rapidjson::Value().SetString(obj.type.c_str(), obj.type.size(), alloc), alloc);
    out.AddMember("telemetry_config", rapidjson::Value().SetString(obj.telemetry_config.c_str(), obj.telemetry_config.size(), alloc), alloc);
    out.AddMember("session_config", rapidjson::Value().SetString(obj.session_config.c_str(), obj.session_config.size(), alloc), alloc);
}
template<>
void Deserialize(get_telemetry_config& obj, rapidjson::Document& doc)
{
    if(!doc.HasMember("type")){
        JSON_LOG_FUNC("get_telemetry_config MISSING FIELD: type"); 
    }else{
        obj.type = doc["type"].GetString();
    }
    if(!doc.HasMember("telemetry_config")){
        JSON_LOG_FUNC("get_telemetry_config MISSING FIELD: telemetry_config"); 
    }else{
        obj.telemetry_config = doc["telemetry_config"].GetString();
    }
    if(!doc.HasMember("session_config")){
        JSON_LOG_FUNC("get_telemetry_config MISSING FIELD: session_config"); 
    }else{
        obj.session_config = doc["session_config"].GetString();
    }
}
template<>
void Deserialize(get_telemetry_config& obj, rapidjson::Value& doc)
{
    if(!doc.HasMember("type")){
        JSON_LOG_FUNC("get_telemetry_config MISSING FIELD: type"); 
    }else{
        obj.type = doc["type"].GetString();
    }
    if(!doc.HasMember("telemetry_config")){
        JSON_LOG_FUNC("get_telemetry_config MISSING FIELD: telemetry_config"); 
    }else{
        obj.telemetry_config = doc["telemetry_config"].GetString();
    }
    if(!doc.HasMember("session_config")){
        JSON_LOG_FUNC("get_telemetry_config MISSING FIELD: session_config"); 
    }else{
        obj.session_config = doc["session_config"].GetString();
    }
}

template<>
std::string JsonToString(const get_telemetry_config& obj)
{
    rapidjson::Document doc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> w(sb);
    Serialize(doc, obj);
    doc.Accept(w);
    return sb.GetString();
}

template<>
bool LoadStruct(get_telemetry_config& out, const std::string& path)
{
    rapidjson::Document doc;
    LoadJSON(doc, path);
    bool check_passed = CheckJson(out, doc);
    Deserialize(out, doc);
    return check_passed;
}
template<>
void SaveStruct(const get_telemetry_config& obj, const std::string& path)
{
    rapidjson::Document doc;
    Serialize(doc, obj);
    SaveJSON(doc, path);
}

template <>
bool CheckJson(const telemetry_status& obj, const rapidjson::Document& doc)
{
    bool check = true;
    if(!doc.HasMember("type")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: type"); 
        check = false;
    }
    if(!doc.HasMember("timestamp")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: timestamp"); 
        check = false;
    }
    if(!doc.HasMember("data")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: data"); 
        check = false;
    }
    if(!doc.HasMember("msgs_per_second")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: msgs_per_second"); 
        check = false;
    }
    if(!doc.HasMember("camera_status")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: camera_status"); 
        check = false;
    }
    if(!doc.HasMember("camera_error")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: camera_error"); 
        check = false;
    }
    if(!doc.HasMember("cpu_total_load")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: cpu_total_load"); 
        check = false;
    }
    if(!doc.HasMember("cpu_process_load")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: cpu_process_load"); 
        check = false;
    }
    if(!doc.HasMember("mem_process_load")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: mem_process_load"); 
        check = false;
    }
    return check;
}

template<>
void Serialize(rapidjson::Document& out, const telemetry_status& obj)
{
    out.SetObject();
    rapidjson::Document::AllocatorType& alloc = out.GetAllocator();
    out.AddMember("type", rapidjson::Value().SetString(obj.type.c_str(), obj.type.size(), alloc), alloc);
    out.AddMember("timestamp", rapidjson::Value().SetDouble(obj.timestamp), alloc);
    out.AddMember("data", rapidjson::Value().SetInt(obj.data), alloc);
    {
        rapidjson::Value v0;
        v0.SetArray();
        for(size_t i = 0; i < obj.msgs_per_second.size(); i++){
        	rapidjson::Value new_obj;
        	Serialize(new_obj, obj.msgs_per_second[i], alloc);
        	v0.PushBack(new_obj, alloc);
    	}
    	out.AddMember("msgs_per_second", v0, alloc);
    }
    out.AddMember("camera_status", rapidjson::Value().SetString(obj.camera_status.c_str(), obj.camera_status.size(), alloc), alloc);
    out.AddMember("camera_error", rapidjson::Value().SetString(obj.camera_error.c_str(), obj.camera_error.size(), alloc), alloc);
    out.AddMember("cpu_total_load", rapidjson::Value().SetInt(obj.cpu_total_load), alloc);
    out.AddMember("cpu_process_load", rapidjson::Value().SetInt(obj.cpu_process_load), alloc);
    out.AddMember("mem_process_load", rapidjson::Value().SetInt(obj.mem_process_load), alloc);
}
template<>
void Deserialize(telemetry_status& obj, rapidjson::Document& doc)
{
    if(!doc.HasMember("type")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: type"); 
    }else{
        obj.type = doc["type"].GetString();
    }
    if(!doc.HasMember("timestamp")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: timestamp"); 
    }else{
        obj.timestamp = doc["timestamp"].GetDouble();
    }
    if(!doc.HasMember("data")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: data"); 
    }else{
        obj.data = doc["data"].GetInt();
    }
    if(!doc.HasMember("msgs_per_second")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: msgs_per_second"); 
    }else{
	obj.msgs_per_second.resize(doc["msgs_per_second"].Size());
	for(rapidjson::SizeType i = 0; i < doc["msgs_per_second"].Size(); i++){
		Deserialize(obj.msgs_per_second[i], doc["msgs_per_second"][i]);
	}
    }
    if(!doc.HasMember("camera_status")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: camera_status"); 
    }else{
        obj.camera_status = doc["camera_status"].GetString();
    }
    if(!doc.HasMember("camera_error")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: camera_error"); 
    }else{
        obj.camera_error = doc["camera_error"].GetString();
    }
    if(!doc.HasMember("cpu_total_load")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: cpu_total_load"); 
    }else{
        obj.cpu_total_load = doc["cpu_total_load"].GetInt();
    }
    if(!doc.HasMember("cpu_process_load")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: cpu_process_load"); 
    }else{
        obj.cpu_process_load = doc["cpu_process_load"].GetInt();
    }
    if(!doc.HasMember("mem_process_load")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: mem_process_load"); 
    }else{
        obj.mem_process_load = doc["mem_process_load"].GetInt();
    }
}
template<>
void Deserialize(telemetry_status& obj, rapidjson::Value& doc)
{
    if(!doc.HasMember("type")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: type"); 
    }else{
        obj.type = doc["type"].GetString();
    }
    if(!doc.HasMember("timestamp")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: timestamp"); 
    }else{
        obj.timestamp = doc["timestamp"].GetDouble();
    }
    if(!doc.HasMember("data")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: data"); 
    }else{
        obj.data = doc["data"].GetInt();
    }
    if(!doc.HasMember("msgs_per_second")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: msgs_per_second"); 
    }else{
	obj.msgs_per_second.resize(doc["msgs_per_second"].Size());
	for(rapidjson::SizeType i = 0; i < doc["msgs_per_second"].Size(); i++){
		Deserialize(obj.msgs_per_second[i], doc["msgs_per_second"][i]);
	}
    }
    if(!doc.HasMember("camera_status")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: camera_status"); 
    }else{
        obj.camera_status = doc["camera_status"].GetString();
    }
    if(!doc.HasMember("camera_error")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: camera_error"); 
    }else{
        obj.camera_error = doc["camera_error"].GetString();
    }
    if(!doc.HasMember("cpu_total_load")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: cpu_total_load"); 
    }else{
        obj.cpu_total_load = doc["cpu_total_load"].GetInt();
    }
    if(!doc.HasMember("cpu_process_load")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: cpu_process_load"); 
    }else{
        obj.cpu_process_load = doc["cpu_process_load"].GetInt();
    }
    if(!doc.HasMember("mem_process_load")){
        JSON_LOG_FUNC("telemetry_status MISSING FIELD: mem_process_load"); 
    }else{
        obj.mem_process_load = doc["mem_process_load"].GetInt();
    }
}

template<>
std::string JsonToString(const telemetry_status& obj)
{
    rapidjson::Document doc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> w(sb);
    Serialize(doc, obj);
    doc.Accept(w);
    return sb.GetString();
}

template<>
bool LoadStruct(telemetry_status& out, const std::string& path)
{
    rapidjson::Document doc;
    LoadJSON(doc, path);
    bool check_passed = CheckJson(out, doc);
    Deserialize(out, doc);
    return check_passed;
}
template<>
void SaveStruct(const telemetry_status& obj, const std::string& path)
{
    rapidjson::Document doc;
    Serialize(doc, obj);
    SaveJSON(doc, path);
}

#endif // __JSON_LOADER__