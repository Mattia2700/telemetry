using namespace std;

#include "wsclient.h"

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
    socket->m_client.set_open_handler(bind(&Connection::on_open, this, _1));
    socket->m_client.set_close_handler(bind(&Connection::on_close, this, _1));
    socket->m_client.set_fail_handler(bind(&Connection::on_fail, this, _1));
    //m_new_data.store(false);
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
    /*try {
        (*socket->socket).setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
    } catch(zmq::error_t& e) {
        clbk_on_error(e.num(), e.what());
    }*/
	// create a subscription at the topic
	this->topic[topic] = true;

    if(clbk_on_subscribe) {
        clbk_on_subscribe(topic);
    }
}

void WebSocketClient::unsubscribe(const string& topic) {
    /*try {
        (*socket->socket).setsockopt(ZMQ_UNSUBSCRIBE, topic.c_str(), topic.size());
    } catch(zmq::error_t& e) {
        clbk_on_error(e.num(), e.what());
    }*/

	if(this->topic.find(topic) != this->topic.end()) {
		this->topic[topic] = false;
	}

    if(clbk_on_unsubscribe) {
        clbk_on_unsubscribe(topic);
    }
}

thread* WebSocketClient::startPub() {
    stringstream server;

    server << "tcp://" << address << ":" << port;

	socket->m_conn = socket->m_client.get_connection(server.str(), socket->ec);

    if(socket->ec) {
        socket->m_client.get_alog().write(websocketpp::log::alevel::app, "Get Connection Error: " + socket->ec.message());
        return nullptr;
    }

	socket->m_hdl = socket->m_conn->get_handle();

	socket->m_client.connect(socket->m_conn);

    // Ensure publisher connection has time to complete
    sleep(1);

	// PHIL
    // socket->m_client.get_io_service().reset();

    open = true;

    if(clbk_on_open) {
        clbk_on_open();
    }

    thread* telemetry_thread = new thread(&WebSocketClient::pubLoop, this);

    return telemetry_thread;
}

thread* WebSocketClient::startSub() {
    stringstream server;

    server << "tcp://" << address << ":" << port;

	socket->m_conn = socket->m_client.get_connection(server.str(), socket->ec);

    if(socket->ec) {
        socket->m_client.get_alog().write(websocketpp::log::alevel::app, "Get Connection Error: " + socket->ec.message());
        return nullptr;
    }

	socket->m_hdl = socket->m_conn->get_handle();

	socket->m_client.connect(socket->m_conn);

    // Ensure subscriber connection has time to complete
    sleep(1);

    open = true;

    if(clbk_on_open) {
        clbk_on_open();
    }

    thread* telemetry_thread = new thread(&WebSocketClient::subLoop, this);

    return telemetry_thread;
}

/*websocketpp::lib::thread* WebSocketClient::run(const std::string& uri) {
    m_done = false;
    // Create a new connection to the given URI
    m_conn = m_client.get_connection(uri, ec);
    if(ec) {
        m_client.get_alog().write(websocketpp::log::alevel::app, "Get Connection Error: " + ec.message());
        return nullptr;
    }

    // Grab a handle for this connection so we can talk to it in a thread
    // safe manor after the event loop starts.
    m_hdl = m_conn->get_handle();

    // Queue the connection. No DNS queries or network connections will be
    // made until the io_service event loop is run.
    m_client.connect(m_conn);

    m_client.get_io_service().reset();
    asio_thread = new websocketpp::lib::thread(&client::run, &m_client);
    telemetry_thread = new websocketpp::lib::thread(&WebSocketClient::loop, this);

    return telemetry_thread;
}*/

/*void WebSocketClient::loop() {
    while (!m_open) usleep(10000);
    while (!m_done) {
        unique_lock<mutex> lck(m_worker_mtx);

        while (!m_new_data && !m_done) m_cv.wait(lck);

        // Adding this because code can be stuck waiting for m_cv
        // when actually the socket is being closed
        if (m_done) break;

        // m_client.get_alog().write(websocketpp::log::alevel::app, "Seding");
        m_client.send(m_hdl, m_to_send_data.front(), websocketpp::frame::opcode::binary, ec);
        // std::cout << "Sent" << std::endl;

        m_to_send_data.pop();

        if (m_to_send_data.empty()) m_new_data.store(false);

        if (ec) {
            m_client.get_alog().write(websocketpp::log::alevel::app, "Send Error: " + ec.message());
            continue;
        }
    }
}*/

void WebSocketClient::sendMessage(const message& msg) {
	string message = msg.topic + ": " + msg.payload;
	// find a way to send both topic and payload
	socket->m_client.send(
		socket->m_hdl, message,
		websocketpp::frame::opcode::binary, socket->ec);
}

void WebSocketClient::receiveMessage(message& msg) {
	string topic;	// received strings must be
	string payload;	// saved here

	if(this->topic.find(topic) != this->topic.end()) {
		msg.topic = topic;
		msg.payload = payload;
	}
}