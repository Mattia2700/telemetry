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
void CheckJson(const _GPS_& obj, const json& j)
{
	if(!j.contains("Messages"))
		std::cout << "ERROR" << "JSON does not contain key [Messages] of type [int] in object [GPS]" << std::endl;
	if(!j.contains("Average_Frequency_Hz"))
		std::cout << "ERROR" << "JSON does not contain key [Average_Frequency_Hz] of type [int] in object [GPS]" << std::endl;
	if(!j.contains("Duration_seconds"))
		std::cout << "ERROR" << "JSON does not contain key [Duration_seconds] of type [float] in object [GPS]" << std::endl;
}
template <>
void Deserialize(_GPS_& obj,const json& j)
{
	CheckJson<_GPS_>(obj, j);
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
json Serialize(const _GPS_& obj) 
{
	json j;
	j["Messages"] = obj.Messages;
	j["Average_Frequency_Hz"] = obj.Average_Frequency_Hz;
	j["Duration_seconds"] = obj.Duration_seconds;
	return j;
}
template <>
bool LoadJson(_GPS_& obj, const std::string& path)
{
	std::ifstream f(path);
	if(!f.is_open())
		return false;
	json j;
	f >> j;
	f.close();
	Deserialize<_GPS_>(obj, j);
	return true;
}
template <>
void SaveJson(const _GPS_& obj, const std::string& path)
{
	std::ofstream f(path);
	f << std::setw(4) << Serialize<_GPS_>(obj);
	f.close();
}


////////////////////////////////////////
////////////////////////////////////////


template <>
void CheckJson(const gps_stat_json& obj, const json& j)
{
	if(!j.contains("Date"))
		std::cout << "ERROR" << "JSON does not contain key [Date] of type [std::string] in object [gps_stat_json]" << std::endl;
	if(!j.contains("GPS"))
		std::cout << "ERROR" << "JSON does not contain key [GPS] of type [_GPS_] in object [gps_stat_json]" << std::endl;
}
template <>
void Deserialize(gps_stat_json& obj,const json& j)
{
	CheckJson<gps_stat_json>(obj, j);
	if(j.contains("Date"))
	{
		obj.Date = j["Date"];
	}
	if(j.contains("GPS"))
	{
		Deserialize<_GPS_>(obj.GPS, j["GPS"]);
	}
}
template <>
json Serialize(const gps_stat_json& obj) 
{
	json j;
	j["Date"] = obj.Date;
	j["GPS"] = Serialize<_GPS_>(obj.GPS);
	return j;
}
template <>
bool LoadJson(gps_stat_json& obj, const std::string& path)
{
	std::ifstream f(path);
	if(!f.is_open())
		return false;
	json j;
	f >> j;
	f.close();
	Deserialize<gps_stat_json>(obj, j);
	return true;
}
template <>
void SaveJson(const gps_stat_json& obj, const std::string& path)
{
	std::ofstream f(path);
	f << std::setw(4) << Serialize<gps_stat_json>(obj);
	f.close();
}


////////////////////////////////////////
////////////////////////////////////////


