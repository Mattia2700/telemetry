#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>
#include <iomanip>

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "json_models.h"




#ifndef JSON_LOADER_H
#define JSON_LOADER_H

template<class T>
void CheckJson(const T& obj, const json& j);
template<class T>
json Serialize(const T& obj);
template<class T>
void Deserialize(T& obj, const json& j);
template<class T>
bool LoadJson(T& obj, const std::string& path);
template<class T>
void SaveJson(const T& obj, const std::string& path);

#endif // JSON_LOADER_H