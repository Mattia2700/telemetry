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
void CheckJson(const _CAN_& obj, const json& j)
{
	if(!j.contains("Messages"))
		std::cout << "ERROR " << "JSON does not contain key [Messages] of type [int] in object [CAN]" << std::endl;
	if(!j.contains("Average_Frequency_Hz"))
		std::cout << "ERROR " << "JSON does not contain key [Average_Frequency_Hz] of type [int] in object [CAN]" << std::endl;
	if(!j.contains("Duration_seconds"))
		std::cout << "ERROR " << "JSON does not contain key [Duration_seconds] of type [float] in object [CAN]" << std::endl;
}
template <>
void Deserialize(_CAN_& obj,const json& j)
{
	CheckJson<_CAN_>(obj, j);
	if(j.contains("Messages"))
	{
		obj.Messages = j["Messages"];
	}
	if(j.contains("Average_Frequency_Hz"))
	{
		obj.Average_Frequency_Hz = j["Average_Frequency_Hz"];
	}
	if(j.contains("Duration_seconds"))
	{
		obj.Duration_seconds = j["Duration_seconds"];
	}
}
template <>
json Serialize(const _CAN_& obj) 
{
	json j;
	j["Messages"] = obj.Messages;
	j["Average_Frequency_Hz"] = obj.Average_Frequency_Hz;
	j["Duration_seconds"] = obj.Duration_seconds;
	return j;
}
template <>
bool LoadJson(_CAN_& obj, const std::string& path)
{
	std::ifstream f(path);
	if(!f.is_open())
		return false;
	json j;
	f >> j;
	f.close();
	Deserialize<_CAN_>(obj, j);
	return true;
}
template <>
void SaveJson(const _CAN_& obj, const std::string& path)
{
	std::ofstream f(path);
	f << std::setw(4) << Serialize<_CAN_>(obj);
	f.close();
}


////////////////////////////////////////
////////////////////////////////////////


template <>
void CheckJson(const can_stat_json& obj, const json& j)
{
	if(!j.contains("Date"))
		std::cout << "ERROR " << "JSON does not contain key [Date] of type [std::string] in object [can_stat_json]" << std::endl;
	if(!j.contains("Circuit"))
		std::cout << "ERROR " << "JSON does not contain key [Circuit] of type [std::string] in object [can_stat_json]" << std::endl;
	if(!j.contains("Pilot"))
		std::cout << "ERROR " << "JSON does not contain key [Pilot] of type [std::string] in object [can_stat_json]" << std::endl;
	if(!j.contains("Race"))
		std::cout << "ERROR " << "JSON does not contain key [Race] of type [std::string] in object [can_stat_json]" << std::endl;
	if(!j.contains("Configuration"))
		std::cout << "ERROR " << "JSON does not contain key [Configuration] of type [std::string] in object [can_stat_json]" << std::endl;
	if(!j.contains("CAN"))
		std::cout << "ERROR " << "JSON does not contain key [CAN] of type [_CAN_] in object [can_stat_json]" << std::endl;
}
template <>
void Deserialize(can_stat_json& obj,const json& j)
{
	CheckJson<can_stat_json>(obj, j);
	if(j.contains("Date"))
	{
		obj.Date = j["Date"];
	}
	if(j.contains("Circuit"))
	{
		obj.Circuit = j["Circuit"];
	}
	if(j.contains("Pilot"))
	{
		obj.Pilot = j["Pilot"];
	}
	if(j.contains("Race"))
	{
		obj.Race = j["Race"];
	}
	if(j.contains("Configuration"))
	{
		obj.Configuration = j["Configuration"];
	}
	if(j.contains("CAN"))
	{
		Deserialize<_CAN_>(obj.CAN, j["CAN"]);
	}
}
template <>
json Serialize(const can_stat_json& obj) 
{
	json j;
	j["Date"] = obj.Date;
	j["Circuit"] = obj.Circuit;
	j["Pilot"] = obj.Pilot;
	j["Race"] = obj.Race;
	j["Configuration"] = obj.Configuration;
	j["CAN"] = Serialize<_CAN_>(obj.CAN);
	return j;
}
template <>
bool LoadJson(can_stat_json& obj, const std::string& path)
{
	std::ifstream f(path);
	if(!f.is_open())
		return false;
	json j;
	f >> j;
	f.close();
	Deserialize<can_stat_json>(obj, j);
	return true;
}
template <>
void SaveJson(const can_stat_json& obj, const std::string& path)
{
	std::ofstream f(path);
	f << std::setw(4) << Serialize<can_stat_json>(obj);
	f.close();
}


////////////////////////////////////////
////////////////////////////////////////


