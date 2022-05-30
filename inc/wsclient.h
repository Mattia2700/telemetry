#include <mutex>
#include <string>
#include <unordered_set>
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
		~WebSocketClient();

		void closeConnection();

		void subscribe(const string& topic);
		void unsubscribe(const string& topic);

	private:
		custom_ws_socket* socket;

		void on_open(websocketpp::connection_hdl);
		void on_close(websocketpp::connection_hdl conn);
		void on_fail(websocketpp::connection_hdl);
		void on_message(client* cli, websocketpp::connection_hdl hdl, message_ptr msg);

		unordered_set<string> topic;

		websocketpp::lib::thread* asio_thread;
		websocketpp::lib::thread* telemetry_thread;

		thread* startPub();
		thread* startSub();

		void sendMessage(const message& msg);
		void receiveMessage(message& msg);
};
