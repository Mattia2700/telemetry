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

#include "can.h"
#include "utils.h"
#include "serial.h"
#include "vehicle.h"
#include "gps_logger.h"

#ifdef WITH_CAMERA
#include "camera.h"
#endif

#include "wsclient.h"
#include "devices.pb.h"

#include "console.h"
#include "StateMachine/StateMachine.h"

#include "telemetry_config/json_loader.h"
#include "session_config/json_loader.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
using namespace rapidjson;



using namespace std;
using namespace std::chrono;

struct CAN_Stat_t
{
  double duration;
  uint64_t msg_count;
};
enum TelemetryError
{
	TEL_NONE,
	TEL_LOAD_CONFIG_TELEMETRY,
	TEL_LOAD_CONFIG_SESSION,
	TEL_LOG_FOLDER,
	TEL_CAN_SOCKET_OPEN
};
static const char* TelemetryErrorStr[]
{
	"None",
	"Load telemetry config Error",
	"Load session config Error",
	"Log folder error",
	"Can failed opening socket"
};


class TelemetrySM : public StateMachine
{
public:
	TelemetrySM();
	~TelemetrySM();

  void Init();
  void Run();
  void Stop();
	void Reset();

	TelemetryError GetError();

private:
	enum States
	{
		ST_UNINITIALIZED,
		ST_INIT,
		ST_IDLE,
		ST_RUN,
		ST_STOP,
		ST_ERROR,
		ST_MAX_STATES
	};
	string StatesStr[ST_MAX_STATES];


	string HOME_PATH;
	string CAN_DEVICE;
	string FOLDER_PATH;
	string CURRENT_LOG_FOLDER;

	std::fstream* dump_file;

	Can* can;
	sockaddr_can addr;
	Chimera* chimera;
	WebSocketClient* ws_cli;
	vector<GpsLogger*> gps_loggers;
#ifdef WITH_CAMERA
	Camera camera;
#endif

	// Threads
	mutex mtx;
	atomic<bool> kill_threads;

	thread* data_thread;
	thread* status_thread;
	thread* ws_conn_thread;
	thread* ws_cli_thread;

	// JSON
	telemetry_config tel_conf;
	session_config sesh_config;

	// Stats
	CAN_Stat_t can_stat;

	// WebSocket
	ConnectionState_ ws_conn_state = ConnectionState_::NONE;
	atomic<States> wsRequestState = ST_UNINITIALIZED;


	// Maps
	unordered_map<string, uint32_t> msgs_counters;
	unordered_map<string, uint32_t> msgs_per_second;
	unordered_map<string, double> timers;


	TelemetryError currentError;

private:
	string GetDate();
	string GetTime();
	string CanMessage2Str(const can_frame& msg);

	void OpenCanSocket();
	void OpenLogFolder(const string& path);
	void CreateHeader(string &header);
	void CreateFolderName(string& folder);
	void LogCan(const double& timestamp, const can_frame& msg);

	// GPS
	void SetupGps();
	void OnGpsLine(int id, string line);
	

	// WebSocket
	void ConnectToWS();
	void OnOpen();
	void OnClose(int);
	void OnError(int);
	void SendWsData();
	void SendStatus();
	void OnMessage(client* cli, websocketpp::connection_hdl hdl, message_ptr msg);


	// State Machine
	void SetError(TelemetryError);
	void EmitError(TelemetryError);


	// Configs
	void LoadAllConfig();
	void SaveAllConfig();

	// Stats
	void SaveStat();

	// Protobuffer
	void ProtoSerialize(const double& timestamp, Device*);


private:
	// Define the state machine state functions with event data type
	STATE_DECLARE(TelemetrySM, 	UninitializedImpl,	NoEventData)
	STATE_DECLARE(TelemetrySM, 	InitImpl,						NoEventData)
	STATE_DECLARE(TelemetrySM, 	IdleImpl,						NoEventData)
	STATE_DECLARE(TelemetrySM, 	RunImpl,						NoEventData)
	STATE_DECLARE(TelemetrySM, 	StopImpl,						NoEventData)
	STATE_DECLARE(TelemetrySM, 	ErrorImpl,					NoEventData)

	ENTRY_DECLARE(TelemetrySM,	Deinitialize,				NoEventData)
	ENTRY_DECLARE(TelemetrySM,	ToRun,							NoEventData)

	// State map to define state object order. Each state map entry defines a
	// state object.
	BEGIN_STATE_MAP_EX
		STATE_MAP_ENTRY_ALL_EX(&UninitializedImpl, nullptr, &Deinitialize, nullptr)
		STATE_MAP_ENTRY_EX(&InitImpl)
		STATE_MAP_ENTRY_EX(&IdleImpl)
		STATE_MAP_ENTRY_ALL_EX(&RunImpl, nullptr, &ToRun, nullptr)
		STATE_MAP_ENTRY_EX(&StopImpl)
		STATE_MAP_ENTRY_EX(&ErrorImpl)
	END_STATE_MAP_EX
};

#define TEL_ERROR_CHECK if(GetError() != TelemetryError::TEL_NONE) return;
