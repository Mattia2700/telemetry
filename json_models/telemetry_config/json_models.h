#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>
#include <iomanip>



struct telemetry_config
{
	std::string can_device;
	std::vector<std::string> gps_devices;
	std::vector<std::string> gps_mode;
	std::vector<bool> gps_enabled;
	bool generate_csv;
	bool camera_enable;
	bool ws_enabled;
	bool ws_send_sensor_data;
	int ws_send_rate;
	bool ws_downsample;
	int ws_downsample_mps;
	std::string ws_server_url;
};

