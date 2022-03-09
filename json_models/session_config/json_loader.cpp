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
void CheckJson(const session_config& obj, const json& j)
{
	if(!j.contains("Circuit"))
		std::cout << "ERROR" << "JSON does not contain key [Circuit] of type [std::string] in object [session_config]" << std::endl;
	if(!j.contains("Pilot"))
		std::cout << "ERROR" << "JSON does not contain key [Pilot] of type [std::string] in object [session_config]" << std::endl;
	if(!j.contains("Race"))
		std::cout << "ERROR" << "JSON does not contain key [Race] of type [std::string] in object [session_config]" << std::endl;
	if(!j.contains("Configuration"))
		std::cout << "ERROR" << "JSON does not contain key [Configuration] of type [std::string] in object [session_config]" << std::endl;
	if(!j.contains("Date"))
		std::cout << "ERROR" << "JSON does not contain key [Date] of type [std::string] in object [session_config]" << std::endl;
	if(!j.contains("Time"))
		std::cout << "ERROR" << "JSON does not contain key [Time] of type [std::string] in object [session_config]" << std::endl;
}
template <>
void Deserialize(session_config& obj,const json& j)
{
	CheckJson<session_config>(obj, j);
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
	if(j.contains("Date"))
	{
		obj.Date = j["Date"];
	}
	if(j.contains("Time"))
	{
		obj.Time = j["Time"];
	}
}
template <>
json Serialize(const session_config& obj) 
{
	json j;
	j["Circuit"] = obj.Circuit;
	j["Pilot"] = obj.Pilot;
	j["Race"] = obj.Race;
	j["Configuration"] = obj.Configuration;
	j["Date"] = obj.Date;
	j["Time"] = obj.Time;
	return j;
}
template <>
bool LoadJson(session_config& obj, const std::string& path)
{
	std::ifstream f(path);
	if(!f.is_open())
		return false;
	json j;
	f >> j;
	f.close();
	Deserialize<session_config>(obj, j);
	return true;
}
template <>
void SaveJson(const session_config& obj, const std::string& path)
{
	std::ofstream f(path);
	f << std::setw(4) << Serialize<session_config>(obj);
	f.close();
}


////////////////////////////////////////
////////////////////////////////////////


