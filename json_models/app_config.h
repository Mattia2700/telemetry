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

typedef struct app_connection_t{
    std::string ip;
    std::string port;
    std::string mode;
}app_connection_t;

typedef struct app_config{
    std::string csv_path;
    bool csv_auto_save;
    bool telemetry_auto_get_config;
    float last_login_time;
    app_connection_t app_connection;
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
bool CheckJson(const app_connection_t& obj, const rapidjson::Document& doc)
{
    bool check = true;
    if(!doc.HasMember("ip")){
        JSON_LOG_FUNC("app_connection_t MISSING FIELD: ip"); 
        check = false;
    }
    if(!doc.HasMember("port")){
        JSON_LOG_FUNC("app_connection_t MISSING FIELD: port"); 
        check = false;
    }
    if(!doc.HasMember("mode")){
        JSON_LOG_FUNC("app_connection_t MISSING FIELD: mode"); 
        check = false;
    }
    return check;
}

template<>
void Serialize(rapidjson::Value& out, const app_connection_t& obj, rapidjson::Document::AllocatorType& alloc)
{
    out.SetObject();
    out.AddMember("ip", rapidjson::Value().SetString(obj.ip.c_str(), obj.ip.size(), alloc), alloc);
    out.AddMember("port", rapidjson::Value().SetString(obj.port.c_str(), obj.port.size(), alloc), alloc);
    out.AddMember("mode", rapidjson::Value().SetString(obj.mode.c_str(), obj.mode.size(), alloc), alloc);
}
template<>
void Deserialize(app_connection_t& obj, rapidjson::Value& doc)
{
    if(!doc.HasMember("ip")){
        JSON_LOG_FUNC("app_connection_t MISSING FIELD: ip"); 
    }else{
        obj.ip = doc["ip"].GetString();
    }
    if(!doc.HasMember("port")){
        JSON_LOG_FUNC("app_connection_t MISSING FIELD: port"); 
    }else{
        obj.port = doc["port"].GetString();
    }
    if(!doc.HasMember("mode")){
        JSON_LOG_FUNC("app_connection_t MISSING FIELD: mode"); 
    }else{
        obj.mode = doc["mode"].GetString();
    }
}

template <>
bool CheckJson(const app_config& obj, const rapidjson::Document& doc)
{
    bool check = true;
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
    if(!doc.HasMember("app_connection")){
        JSON_LOG_FUNC("app_config MISSING FIELD: app_connection"); 
        check = false;
    }
    return check;
}

template<>
void Serialize(rapidjson::Document& out, const app_config& obj)
{
    out.SetObject();
    rapidjson::Document::AllocatorType& alloc = out.GetAllocator();
    out.AddMember("csv_path", rapidjson::Value().SetString(obj.csv_path.c_str(), obj.csv_path.size(), alloc), alloc);
    out.AddMember("csv_auto_save", rapidjson::Value().SetBool(obj.csv_auto_save), alloc);
    out.AddMember("telemetry_auto_get_config", rapidjson::Value().SetBool(obj.telemetry_auto_get_config), alloc);
    out.AddMember("last_login_time", rapidjson::Value().SetDouble(obj.last_login_time), alloc);
    {
        rapidjson::Value v;
        Serialize(v, obj.app_connection, alloc);
        out.AddMember("app_connection", v, alloc);
    }
}
template<>
void Deserialize(app_config& obj, rapidjson::Document& doc)
{
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
    if(!doc.HasMember("app_connection")){
        JSON_LOG_FUNC("app_config MISSING FIELD: app_connection"); 
    }else{
        Deserialize(obj.app_connection, doc["app_connection"]);
    }
}
template<>
void Deserialize(app_config& obj, rapidjson::Value& doc)
{
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
    if(!doc.HasMember("app_connection")){
        JSON_LOG_FUNC("app_config MISSING FIELD: app_connection"); 
    }else{
        Deserialize(obj.app_connection, doc["app_connection"]);
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