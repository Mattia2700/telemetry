#pragma once

#include "renderer.h"
#include "Pages/Page1.h"
#include "Pages/Page2.h"
#include "Pages/Page3.h"
#include "Pages/Page4.h"
#include "Pages/Page5.h"
#include "Pages/Page6.h"
#include "Pages/Page7.h"
#include "Pages/Page8.h"
#include "Pages/Page9.h"
#include "Pages/Page10.h"
#include "Pages/Page11.h"

#include "websocket.h"
#include <ostream>
#include <iomanip>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
using namespace rapidjson;

#include "devices.pb.h"
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/json_util.h>
using namespace google::protobuf;
using namespace google::protobuf::util;

typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

Client* c;
Renderer* renderer;
ChimeraData* chimera_data;
devices::Chimera* chimera_proto;

int W = 1480, H = 1000;
// std::string uri = "ws://192.168.195.1:9090";
std::string uri = "http://eagle-telemetry-server.herokuapp.com";
// std::string uri = "ws://127.0.0.1:3000";

void on_key_press(char);

void on_message(client* cli, websocketpp::connection_hdl hdl, message_ptr msg);

string HumanReadable(uintmax_t size);

void on_telemetry_conf_enter(string json);
