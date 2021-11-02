#pragma once


#include "renderer.h"
#include "Pages/Page1.h"
#include "Pages/Page2.h"
#include "Pages/Page3.h"

#include "websocket.h"
#include <ostream>

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

ChimeraData* chimera_data;
devices::Chimera* chimera_proto;

Page1* page1;
Page2* page2;
Page3* page3;
Renderer* renderer;

int W = 800, H = 800;
std::string uri = "ws://localhost:8080";

void on_key_press(char);

void on_message(client* cli, websocketpp::connection_hdl hdl, message_ptr msg);
