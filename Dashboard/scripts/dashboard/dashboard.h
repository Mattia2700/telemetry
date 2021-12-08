#pragma once

// #include "PageManager.h"

// #include "Pages/Page1.h"
// #include "Pages/Page2.h"
// #include "Pages/Page3.h"
// #include "Pages/Page4.h"
// #include "Pages/Page5.h"
// #include "Pages/Page6.h"
// #include "Pages/Page7.h"
// #include "Pages/Page8.h"
// #include "Pages/Page9.h"
// #include "Pages/Page10.h"
// #include "Pages/Page11.h"

#include "websocket.h"
#include <ostream>
#include <iomanip>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

#include "devices.pb.h"
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/json_util.h>

using namespace std;
using namespace rapidjson;
using namespace google::protobuf;
using namespace google::protobuf::util;

typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

