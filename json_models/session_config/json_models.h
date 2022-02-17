#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>
#include <iomanip>



struct session_config
{
	std::string circuit;
	std::string pilot;
	std::string test;
	std::string configuration;
	std::string date;
	std::string time;
};

