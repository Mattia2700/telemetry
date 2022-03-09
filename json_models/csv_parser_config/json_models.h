#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>
#include <iomanip>



struct csv_parser_config
{
	std::string subfolder_name;
	bool parse_candump;
	bool parse_gps;
	bool generate_report;
};

