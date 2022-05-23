#include "wsclient.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

using namespace std;
using namespace rapidjson;

WebSocketClient::WebSocketClient() : Connection() {
	socket = new custom_ws_socket();

    // set up access channels to only log interesting things
    socket->m_client.clear_access_channels(websocketpp::log::alevel::all);
    socket->m_client.set_access_channels(websocketpp::log::alevel::connect);
    socket->m_client.set_access_channels(websocketpp::log::alevel::disconnect);
    socket->m_client.set_access_channels(websocketpp::log::alevel::app);

    // Initialize the Asio transport policy
    socket->m_client.init_asio();

    // Bind the handlers we are using
    socket->m_client.set_open_handler(bind(&WebSocketClient::on_open, this, _1));
    socket->m_client.set_close_handler(bind(&WebSocketClient::on_close, this, _1));
    socket->m_client.set_fail_handler(bind(&WebSocketClient::on_fail, this, _1));
    //m_new_data.store(false);
}

void WebSocketClient::on_open(websocketpp::connection_hdl) {
    scoped_lock guard(mtx);
    open = true;
    
    if(clbk_on_open) {
        clbk_on_open();
    }
}

void WebSocketClient::on_close(websocketpp::connection_hdl conn) {
    socket->m_client.get_alog().write(websocketpp::log::alevel::app, "Connection closed, stopping telemetry!");

    stop();
    reset();
}

void WebSocketClient::on_fail(websocketpp::connection_hdl) {
    socket->m_client.get_alog().write(websocketpp::log::alevel::app, "Connection failed, stopping telemetry!");

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

void WebSocketClient::subscribe(const string& topic) {
	this->topic.insert(topic);

    if(clbk_on_subscribe) {
        clbk_on_subscribe(topic);
    }
}

void WebSocketClient::unsubscribe(const string& topic) {
    auto index = this->topic.find(topic);
	if(index != this->topic.end()) {
		this->topic.erase(index);
	}

    if(clbk_on_unsubscribe) {
        clbk_on_unsubscribe(topic);
    }
}

thread* WebSocketClient::startPub() {
    stringstream server;

    server << "ws://" << address;

	socket->m_conn = socket->m_client.get_connection(server.str(), socket->ec);

    if(socket->ec) {
        socket->m_client.get_alog().write(websocketpp::log::alevel::app, "Get Connection Error: " + socket->ec.message());
        return nullptr;
    }

	socket->m_hdl = socket->m_conn->get_handle();

	socket->m_client.connect(socket->m_conn);

    // Ensure publisher connection has time to complete
    sleep(1);

    socket->m_client.get_io_service().reset();

    open = true;

    if(clbk_on_open) {
        clbk_on_open();
    }

    thread* telemetry_thread = new thread(&WebSocketClient::pubLoop, this);

    return telemetry_thread;
}

thread* WebSocketClient::startSub() {
    stringstream server;

    server << "ws://" << address;

    // bind with the on_message
    socket->m_client.set_message_handler(
        bind(
            bind(&WebSocketClient::on_message, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),

            &socket->m_client,
            ::_1,
            ::_2));

    // Ensure subscriber connection has time to complete
    sleep(1);

    return nullptr;
}

void WebSocketClient::on_message(client* cli, websocketpp::connection_hdl hdl, message_ptr msg) {
    if(clbk_on_message) {
        message m;

        Document doc;
        ParseResult ok = doc.Parse(msg->get_payload().c_str(), msg->get_payload().size());

        if(!ok || !doc.HasMember("topic")) {
            return;
        } else {
            m.topic = doc["topic"].GetString();
        }

        if(this->topic.find(m.topic) != this->topic.end()) {
            if(doc.HasMember("data")) {
                // data can be a JSON
                // user have to check and manage it
                m.payload = doc["data"].GetString();
                clbk_on_message(m);
            }
        }
    }
}

void WebSocketClient::sendMessage(const message& msg) {
    stringstream message;
    message << "{" <<
                    "\"topic\": \"" << msg.topic << "\", " <<
                    "\"data\": \"" << msg.payload << "\"" <<
                "}";

	socket->m_client.send(socket->m_hdl, message.str(), websocketpp::frame::opcode::binary, socket->ec);
}

void WebSocketClient::receiveMessage(message& msg) {
	string topic;	// received strings must be
	string payload;	// saved here

	if(this->topic.find(topic) != this->topic.end()) {
		msg.topic = topic;
		msg.payload = payload;
	}
}