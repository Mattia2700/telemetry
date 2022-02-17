#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>
#include <iomanip>

#include "nlohmann_json.hpp"
using json = nlohmann::json;

#include "./session_config/json_models.h"
#include "./telemetry_config/json_models.h"


template<class T>
void CheckJson(T& obj, json);
template<class T>
json Serialize(const T& obj);
template<class T>
void Deserialize(T& obj, json);
template<class T>
bool LoadJson(T& obj, std::string path);
template<class T>
void SaveJson(T& obj, std::string path);

