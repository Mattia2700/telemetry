#ifndef __APP_CONFIG__
#define __APP_CONFIG__

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

typedef struct app_config{
    std::string ws_ip;
    std::string csv_path;
    bool csv_auto_save;
    bool telemetry_auto_get_config;
    float last_login_time;
}app_config;

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


#ifndef __JSON_LOADER_DEFINITION__
#define __JSON_LOADER_DEFINITION__

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


#ifdef __APP_CONFIG_IMPLEMENTATION__

template <>
bool CheckJson(const app_config& obj, const rapidjson::Document& doc)
{
    bool check = true;
    if(!doc.HasMember("ws_ip")){
        JSON_LOG_FUNC("app_config MISSING FIELD: ws_ip"); 
        check = false;
    }
    if(!doc.HasMember("csv_path")){
        JSON_LOG_FUNC("app_config MISSING FIELD: csv_path"); 
        check = false;
    }
    if(!doc.HasMember("csv_auto_save")){
        JSON_LOG_FUNC("app_config MISSING FIELD: csv_auto_save"); 
        check = false;
    }
    if(!doc.HasMember("telemetry_auto_get_config")){
        JSON_LOG_FUNC("app_config MISSING FIELD: telemetry_auto_get_config"); 
        check = false;
    }
    if(!doc.HasMember("last_login_time")){
        JSON_LOG_FUNC("app_config MISSING FIELD: last_login_time"); 
        check = false;
    }
    return check;
}

template<>
void Serialize(rapidjson::Document& out, const app_config& obj)
{
    out.SetObject();
    rapidjson::Document::AllocatorType& alloc = out.GetAllocator();
    out.AddMember("ws_ip", rapidjson::Value().SetString(obj.ws_ip.c_str(), obj.ws_ip.size(), alloc), alloc);
    out.AddMember("csv_path", rapidjson::Value().SetString(obj.csv_path.c_str(), obj.csv_path.size(), alloc), alloc);
    out.AddMember("csv_auto_save", rapidjson::Value().SetBool(obj.csv_auto_save), alloc);
    out.AddMember("telemetry_auto_get_config", rapidjson::Value().SetBool(obj.telemetry_auto_get_config), alloc);
    out.AddMember("last_login_time", rapidjson::Value().SetDouble(obj.last_login_time), alloc);
}
template<>
void Deserialize(app_config& obj, rapidjson::Document& doc)
{
    if(!doc.HasMember("ws_ip")){
        JSON_LOG_FUNC("app_config MISSING FIELD: ws_ip"); 
    }else{
        obj.ws_ip = doc["ws_ip"].GetString();
    }
    if(!doc.HasMember("csv_path")){
        JSON_LOG_FUNC("app_config MISSING FIELD: csv_path"); 
    }else{
        obj.csv_path = doc["csv_path"].GetString();
    }
    if(!doc.HasMember("csv_auto_save")){
        JSON_LOG_FUNC("app_config MISSING FIELD: csv_auto_save"); 
    }else{
        obj.csv_auto_save = doc["csv_auto_save"].GetBool();
    }
    if(!doc.HasMember("telemetry_auto_get_config")){
        JSON_LOG_FUNC("app_config MISSING FIELD: telemetry_auto_get_config"); 
    }else{
        obj.telemetry_auto_get_config = doc["telemetry_auto_get_config"].GetBool();
    }
    if(!doc.HasMember("last_login_time")){
        JSON_LOG_FUNC("app_config MISSING FIELD: last_login_time"); 
    }else{
        obj.last_login_time = doc["last_login_time"].GetDouble();
    }
}
template<>
void Deserialize(app_config& obj, rapidjson::Value& doc)
{
    if(!doc.HasMember("ws_ip")){
        JSON_LOG_FUNC("app_config MISSING FIELD: ws_ip"); 
    }else{
        obj.ws_ip = doc["ws_ip"].GetString();
    }
    if(!doc.HasMember("csv_path")){
        JSON_LOG_FUNC("app_config MISSING FIELD: csv_path"); 
    }else{
        obj.csv_path = doc["csv_path"].GetString();
    }
    if(!doc.HasMember("csv_auto_save")){
        JSON_LOG_FUNC("app_config MISSING FIELD: csv_auto_save"); 
    }else{
        obj.csv_auto_save = doc["csv_auto_save"].GetBool();
    }
    if(!doc.HasMember("telemetry_auto_get_config")){
        JSON_LOG_FUNC("app_config MISSING FIELD: telemetry_auto_get_config"); 
    }else{
        obj.telemetry_auto_get_config = doc["telemetry_auto_get_config"].GetBool();
    }
    if(!doc.HasMember("last_login_time")){
        JSON_LOG_FUNC("app_config MISSING FIELD: last_login_time"); 
    }else{
        obj.last_login_time = doc["last_login_time"].GetDouble();
    }
}

template<>
std::string JsonToString(const app_config& obj)
{
    rapidjson::Document doc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> w(sb);
    Serialize(doc, obj);
    doc.Accept(w);
    return sb.GetString();
}

template<>
bool LoadStruct(app_config& out, const std::string& path)
{
    rapidjson::Document doc;
    LoadJSON(doc, path);
    bool check_passed = CheckJson(out, doc);
    Deserialize(out, doc);
    return check_passed;
}
template<>
void SaveStruct(const app_config& obj, const std::string& path)
{
    rapidjson::Document doc;
    Serialize(doc, obj);
    SaveJSON(doc, path);
}

#endif //__APP_CONFIG_IMPLEMENTATION__

#endif // __JSON_LOADER__