#include "wsclient.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

using namespace std;
using namespace rapidjson;

WebSocketClient::WebSocketClient() : Connection() {
	socket = new custom_ws_socket();

    // set up access channels to only log interesting things
    socket->m_client.clear_access_channels(websocketpp::log::alevel::all);

    // Initialize the Asio transport policy
    socket->m_client.init_asio();

    // Bind the handlers we are using
    socket->m_client.set_open_handler(bind(&WebSocketClient::m_onOpen, this, _1));
    socket->m_client.set_close_handler(bind(&WebSocketClient::m_onClose, this, _1));
    socket->m_client.set_fail_handler(bind(&WebSocketClient::m_onFail, this, _1));

    // bind with the on_message
    socket->m_client.set_message_handler(
        bind(bind(&WebSocketClient::m_onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
            &socket->m_client, ::_1, ::_2));
}

// it will connect to the server using the selected mode
thread* WebSocketClient::start() {
    stringstream server;

    server << "ws://" << address;
    if(port != "")
        server << ":" << port;

	socket->m_conn = socket->m_client.get_connection(server.str(), socket->ec);

    if(socket->ec)
        return nullptr;

	socket->m_hdl = socket->m_conn->get_handle();
	socket->m_client.connect(socket->m_conn);

    socket->m_client.get_io_service().reset();

    open = true;
    done = false;

    asio_thread = new websocketpp::lib::thread(&client::run, &socket->m_client);
    telemetry_thread = new thread(&WebSocketClient::sendLoop, this);

    return telemetry_thread;
}

void WebSocketClient::m_onOpen(websocketpp::connection_hdl) {
    unique_lock<mutex> guard(mtx);
    open = true;
    
    if(onOpen) {
        onOpen(id);
    }
}

void WebSocketClient::m_onClose(websocketpp::connection_hdl conn) {
    if(onClose)
        onClose(id, 0);
    stop();
    reset();
}

void WebSocketClient::m_onFail(websocketpp::connection_hdl) {
    if(onError)
        onError(id, 0, "Failed");
    stop();
    reset();
}

WebSocketClient::~WebSocketClient() {
	delete &socket->m_client;
	delete &socket->m_conn;
	delete &socket->m_hdl;
	delete &socket->ec;
}

void WebSocketClient::closeConnection() {
    if(open) {
		// the code was 1000
		socket->m_client.close(socket->m_hdl, 0, "Closed by user");
	}

    done = true;
    cv.notify_all();
}

void WebSocketClient::m_onMessage(client* cli, websocketpp::connection_hdl hdl, message_ptr msg) {
    if(onMessage) {
        GenericMessage m;
        m.data = string(msg->get_payload().c_str(), msg->get_payload().size());
        onMessage(id, m);
    }
}

void WebSocketClient::sendMessage(const GenericMessage& msg) {
	socket->m_client.send(socket->m_hdl, msg.data, websocketpp::frame::opcode::binary, socket->ec);
}

void WebSocketClient::receiveMessage(GenericMessage& msg_) {
}