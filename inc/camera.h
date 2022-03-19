#pragma once

#include <ctime>
#include <chrono>
#include <stdio.h>
#include <unistd.h>
#include <iomanip>
#include <iostream>
#include <string.h>
#include <iostream>
#include <exception>
#include <unordered_map>

#include <mutex>
#include <thread>
#include <condition_variable>

#include <raspicam/raspicam.h>
#include <opencv4/opencv2/opencv.hpp>



#include "console.h"
#include "StateMachine/StateMachine.h"


using namespace std;
using namespace std::chrono;


enum CamError
{
	CAM_NONE,
	CAM_OPENING_CAM,
	CAM_NOT_RASPI,
	CAM_OPENING_FILE,
	CAM_NOT_INITIALIZED,
	CAM_GENERAL_ERROR
};

const char* CamErrorStr[]
{
	"NONE",
	"OPENING CAMERA",
	"NOT ON RASPI",
	"OPENING FILE",
	"CAM NOT INITIALIZED",
	"GENERAL ERROR"
};


class CamInitData : public EventData
{
public:
  uint framerate;
	uint width;
	uint height;
};
class CamRunData : public EventData
{
public:
  string filename;
};
class ErrorData : public EventData
{
public:
  CamError error;
};

class Camera : public StateMachine
{
public:
  Camera();

  void Init(CamInitData*);
  void Run(CamRunData*);
  void Stop();

	CamError GetError();

private:
	NoEventData no_data;
	ErrorData error_data;
	CamError currentError;
	cv::VideoWriter writer;

	mutex mtx;
	condition_variable event;
	thread* save_thread = nullptr;

	cv::Mat* img;
	unsigned char *img_data;

	raspicam::RaspiCam* raspi_cam = nullptr;

	struct CamConfigData
	{
		uint framerate;
		string fname;
		cv::Size framesize;
	};

	CamConfigData cam_config_data;

private:
	void SetError(const CamError&);
	double get_timestamp();

	void SaveLoop();

private:
  // State enumeration order must match the order of state method entries
	// in the state map.
	enum States
	{
		ST_NONE,
		ST_INIT,
		ST_IDLE,
		ST_RUN,
		ST_STOP,
		ST_ERROR,
		ST_MAX_STATES
	};

	// Define the state machine state functions with event data type
	STATE_DECLARE(Camera, 	NoneImpl,			NoEventData)
	STATE_DECLARE(Camera, 	InitImpl,			CamInitData)
	STATE_DECLARE(Camera, 	IdleImpl,			NoEventData)
	STATE_DECLARE(Camera, 	RunImpl,			CamRunData)
	STATE_DECLARE(Camera, 	StopImpl,			NoEventData)
	STATE_DECLARE(Camera, 	ErrorImpl,		ErrorData)

	// State map to define state object order. Each state map entry defines a
	// state object.
	BEGIN_STATE_MAP
		STATE_MAP_ENTRY(&NoneImpl)
		STATE_MAP_ENTRY(&InitImpl)
		STATE_MAP_ENTRY(&IdleImpl)
		STATE_MAP_ENTRY(&RunImpl)
		STATE_MAP_ENTRY(&StopImpl)
		STATE_MAP_ENTRY(&ErrorImpl)
	END_STATE_MAP

};