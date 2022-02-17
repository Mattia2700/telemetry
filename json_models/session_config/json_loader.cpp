#include "json_loader.h"

template <class T>
void CheckJson(T& obj, json j)
{
	throw std::logic_error("Check function is not implemented for this struct");
}
template <class T>
void Deserialize(T& obj, json j)
{
	throw std::logic_error("Deserialize function is not implemented for this struct");
}
template <class T>
json Serialize(const T& obj)
{
	throw std::logic_error("Serialize function is not implemented for this struct");
}
template <class T>
bool LoadJson(T& obj, std::string path)
{
	throw std::logic_error("LoadJson function is not implemented for this struct");
}
template <class T>
void SaveJson(T& obj, std::string path)
{
	throw std::logic_error("SaveJson function is not implemented for this struct");
}


////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////


template <>
void CheckJson(session_config& obj, json j)
{
	if(!j.contains("circuit"))
		throw std::runtime_error("JSON does not contain key [circuit] of type [std::string] in object [session_config]");
	if(!j.contains("pilot"))
		throw std::runtime_error("JSON does not contain key [pilot] of type [std::string] in object [session_config]");
	if(!j.contains("test"))
		throw std::runtime_error("JSON does not contain key [test] of type [std::string] in object [session_config]");
	if(!j.contains("configuration"))
		throw std::runtime_error("JSON does not contain key [configuration] of type [std::string] in object [session_config]");
	if(!j.contains("date"))
		throw std::runtime_error("JSON does not contain key [date] of type [std::string] in object [session_config]");
	if(!j.contains("time"))
		throw std::runtime_error("JSON does not contain key [time] of type [std::string] in object [session_config]");
}
template <>
void Deserialize(session_config& obj, json j)
{
	CheckJson<session_config>(obj, j);
	obj.circuit = j["circuit"];
	obj.pilot = j["pilot"];
	obj.test = j["test"];
	obj.configuration = j["configuration"];
	obj.date = j["date"];
	obj.time = j["time"];
}
template <>
json Serialize(const session_config& obj) 
{
	json j;
	j["circuit"] = obj.circuit;
	j["pilot"] = obj.pilot;
	j["test"] = obj.test;
	j["configuration"] = obj.configuration;
	j["date"] = obj.date;
	j["time"] = obj.time;
	return j;
}
template <>
bool LoadJson(session_config& obj, std::string path)
{
	std::ifstream f(path);
	json j;
	f >> j;
	f.close();
	Deserialize<session_config>(obj, j);
	return true;
}
template <>
void SaveJson(session_config& obj, std::string path)
{
	std::ofstream f(path);
	f << std::setw(4) << Serialize<session_config>(obj);
	f.close();
}


////////////////////////////////////////
////////////////////////////////////////


