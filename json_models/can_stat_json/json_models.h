#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>
#include <iomanip>



struct _CAN_
{
	int Messages;
	int Average_Frequency_Hz;
	float Duration_seconds;
};

struct can_stat_json
{
	std::string Date;
	std::string Circuit;
	std::string Pilot;
	std::string Race;
	std::string Configuration;
	_CAN_ CAN;
};

