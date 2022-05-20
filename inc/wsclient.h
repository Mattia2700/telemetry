#include <mutex>
#include <string>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

#include "connection.h"

using namespace std;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;
typedef websocketpp::client<websocketpp::config::asio_client> client;

enum ConnectionState_ {
	NONE,
	CONNECTING,
	CONNECTED,
	FAIL,
	CLOSED
};

class custom_ws_socket : GeneralSocket {
	public:
		client m_client;
		client::connection_ptr m_conn;
		websocketpp::connection_hdl m_hdl;
		websocketpp::lib::error_code ec;
};

class WebSocketClient: public Connection {
	public:
		typedef websocketpp::lib::lock_guard<websocketpp::lib::mutex> scoped_lock;

		WebSocketClient();

		void closeConnection();

		void subscribe(const string& topic);
		void unsubscribe(const string& topic);

	private:
		custom_ws_socket* socket;

		unordered_map<string, bool> topic;	// to be improved

		websocketpp::lib::mutex m_lock;

		websocketpp::lib::thread* asio_thread;
		websocketpp::lib::thread* telemetry_thread;

		thread* startPub();
		thread* startSub();

		void sendMessage(const message& msg);
		void receiveMessage(message& msg);
};
