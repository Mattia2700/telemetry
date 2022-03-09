#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>
#include <iomanip>



struct _GPS_
{
	int Messages;
	int Average_Frequency_Hz;
	float Duration_seconds;
};

struct gps_stat_json
{
	std::string Date;
	_GPS_ GPS;
};

