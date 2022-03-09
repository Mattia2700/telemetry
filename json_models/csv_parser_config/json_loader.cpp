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
void CheckJson(const csv_parser_config& obj, const json& j)
{
	if(!j.contains("subfolder_name"))
		std::cout << "ERROR" << "JSON does not contain key [subfolder_name] of type [std::string] in object [csv_parser_config]" << std::endl;
	if(!j.contains("parse_candump"))
		std::cout << "ERROR" << "JSON does not contain key [parse_candump] of type [bool] in object [csv_parser_config]" << std::endl;
	if(!j.contains("parse_gps"))
		std::cout << "ERROR" << "JSON does not contain key [parse_gps] of type [bool] in object [csv_parser_config]" << std::endl;
	if(!j.contains("generate_report"))
		std::cout << "ERROR" << "JSON does not contain key [generate_report] of type [bool] in object [csv_parser_config]" << std::endl;
}
template <>
void Deserialize(csv_parser_config& obj,const json& j)
{
	CheckJson<csv_parser_config>(obj, j);
	if(j.contains("subfolder_name"))
	{
		obj.subfolder_name = j["subfolder_name"];
	}
	if(j.contains("parse_candump"))
	{
		obj.parse_candump = j["parse_candump"];
	}
	if(j.contains("parse_gps"))
	{
		obj.parse_gps = j["parse_gps"];
	}
	if(j.contains("generate_report"))
	{
		obj.generate_report = j["generate_report"];
	}
}
template <>
json Serialize(const csv_parser_config& obj) 
{
	json j;
	j["subfolder_name"] = obj.subfolder_name;
	j["parse_candump"] = obj.parse_candump;
	j["parse_gps"] = obj.parse_gps;
	j["generate_report"] = obj.generate_report;
	return j;
}
template <>
bool LoadJson(csv_parser_config& obj, const std::string& path)
{
	std::ifstream f(path);
	if(!f.is_open())
		return false;
	json j;
	f >> j;
	f.close();
	Deserialize<csv_parser_config>(obj, j);
	return true;
}
template <>
void SaveJson(const csv_parser_config& obj, const std::string& path)
{
	std::ofstream f(path);
	f << std::setw(4) << Serialize<csv_parser_config>(obj);
	f.close();
}


////////////////////////////////////////
////////////////////////////////////////


