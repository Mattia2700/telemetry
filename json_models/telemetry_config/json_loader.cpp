#include "json_loader.h"

template <class T>
void CheckJson(const T& obj, const json& j)
{
	throw std::logic_error("Check function is not implemented for this struct");
}
template <class T>
void Deserialize(T& obj, const json& j)
{
	throw std::logic_error("Deserialize function is not implemented for this struct");
}
template <class T>
json Serialize(const T& obj)
{
	throw std::logic_error("Serialize function is not implemented for this struct");
}
template <class T>
bool LoadJson(T& obj, const std::string& path)
{
	throw std::logic_error("LoadJson function is not implemented for this struct");
}
template <class T>
void SaveJson(const T& obj, const std::string& path)
{
	throw std::logic_error("SaveJson function is not implemented for this struct");
}


////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////


template <>
void CheckJson(const telemetry_config& obj, const json& j)
{
	if(!j.contains("can_device"))
		std::cout << "ERROR " << "JSON does not contain key [can_device] of type [std::string] in object [telemetry_config]" << std::endl;
	if(!j.contains("gps_devices"))
		std::cout << "ERROR " << "JSON does not contain key [gps_devices] of type [std::vector] in object [telemetry_config]" << std::endl;
	if(!j.contains("gps_mode"))
		std::cout << "ERROR " << "JSON does not contain key [gps_mode] of type [std::vector] in object [telemetry_config]" << std::endl;
	if(!j.contains("gps_enabled"))
		std::cout << "ERROR " << "JSON does not contain key [gps_enabled] of type [std::vector] in object [telemetry_config]" << std::endl;
	if(!j.contains("generate_csv"))
		std::cout << "ERROR " << "JSON does not contain key [generate_csv] of type [bool] in object [telemetry_config]" << std::endl;
	if(!j.contains("camera_enable"))
		std::cout << "ERROR " << "JSON does not contain key [camera_enable] of type [bool] in object [telemetry_config]" << std::endl;
	if(!j.contains("ws_enabled"))
		std::cout << "ERROR " << "JSON does not contain key [ws_enabled] of type [bool] in object [telemetry_config]" << std::endl;
	if(!j.contains("ws_send_sensor_data"))
		std::cout << "ERROR " << "JSON does not contain key [ws_send_sensor_data] of type [bool] in object [telemetry_config]" << std::endl;
	if(!j.contains("ws_send_rate"))
		std::cout << "ERROR " << "JSON does not contain key [ws_send_rate] of type [int] in object [telemetry_config]" << std::endl;
	if(!j.contains("ws_downsample"))
		std::cout << "ERROR " << "JSON does not contain key [ws_downsample] of type [bool] in object [telemetry_config]" << std::endl;
	if(!j.contains("ws_downsample_mps"))
		std::cout << "ERROR " << "JSON does not contain key [ws_downsample_mps] of type [int] in object [telemetry_config]" << std::endl;
	if(!j.contains("ws_server_url"))
		std::cout << "ERROR " << "JSON does not contain key [ws_server_url] of type [std::string] in object [telemetry_config]" << std::endl;
}
template <>
void Deserialize(telemetry_config& obj,const json& j)
{
	CheckJson<telemetry_config>(obj, j);
	if(j.contains("can_device"))
	{
		obj.can_device = j["can_device"];
	}
	if(j.contains("gps_devices"))
	{
		obj.gps_devices.clear();
		for(auto i: j["gps_devices"])
		{
				obj.gps_devices .push_back(i);
		}
	}
	if(j.contains("gps_mode"))
	{
		obj.gps_mode.clear();
		for(auto i: j["gps_mode"])
		{
				obj.gps_mode .push_back(i);
		}
	}
	if(j.contains("gps_enabled"))
	{
		obj.gps_enabled.clear();
		for(auto i: j["gps_enabled"])
		{
				obj.gps_enabled .push_back(i);
		}
	}
	if(j.contains("generate_csv"))
	{
		obj.generate_csv = j["generate_csv"];
	}
	if(j.contains("camera_enable"))
	{
		obj.camera_enable = j["camera_enable"];
	}
	if(j.contains("ws_enabled"))
	{
		obj.ws_enabled = j["ws_enabled"];
	}
	if(j.contains("ws_send_sensor_data"))
	{
		obj.ws_send_sensor_data = j["ws_send_sensor_data"];
	}
	if(j.contains("ws_send_rate"))
	{
		obj.ws_send_rate = j["ws_send_rate"];
	}
	if(j.contains("ws_downsample"))
	{
		obj.ws_downsample = j["ws_downsample"];
	}
	if(j.contains("ws_downsample_mps"))
	{
		obj.ws_downsample_mps = j["ws_downsample_mps"];
	}
	if(j.contains("ws_server_url"))
	{
		obj.ws_server_url = j["ws_server_url"];
	}
}
template <>
json Serialize(const telemetry_config& obj) 
{
	json j;
	j["can_device"] = obj.can_device;
	json gps_devices_json;
	for(int i = 0; i < obj.gps_devices .size(); i++)
	{
		gps_devices_json.push_back(obj.gps_devices [i]);
	}
	j["gps_devices"] = gps_devices_json;
	json gps_mode_json;
	for(int i = 0; i < obj.gps_mode .size(); i++)
	{
		gps_mode_json.push_back(obj.gps_mode [i]);
	}
	j["gps_mode"] = gps_mode_json;
	json gps_enabled_json;
	for(int i = 0; i < obj.gps_enabled .size(); i++)
	{
		gps_enabled_json.push_back(obj.gps_enabled [i]);
	}
	j["gps_enabled"] = gps_enabled_json;
	j["generate_csv"] = obj.generate_csv;
	j["camera_enable"] = obj.camera_enable;
	j["ws_enabled"] = obj.ws_enabled;
	j["ws_send_sensor_data"] = obj.ws_send_sensor_data;
	j["ws_send_rate"] = obj.ws_send_rate;
	j["ws_downsample"] = obj.ws_downsample;
	j["ws_downsample_mps"] = obj.ws_downsample_mps;
	j["ws_server_url"] = obj.ws_server_url;
	return j;
}
template <>
bool LoadJson(telemetry_config& obj, const std::string& path)
{
	std::ifstream f(path);
	if(!f.is_open())
		return false;
	json j;
	f >> j;
	f.close();
	Deserialize<telemetry_config>(obj, j);
	return true;
}
template <>
void SaveJson(const telemetry_config& obj, const std::string& path)
{
	std::ofstream f(path);
	f << std::setw(4) << Serialize<telemetry_config>(obj);
	f.close();
}


////////////////////////////////////////
////////////////////////////////////////


