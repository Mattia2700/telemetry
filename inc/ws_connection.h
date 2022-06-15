#ifndef __WS_CONECTION__
#define __WS_CONECTION__

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

enum ConnectionState_
{
	NONE,
	CONNECTING,
	CONNECTED,
	FAIL,
	CLOSED
};

class custom_ws_socket
{
public:
	client m_client;
	client::connection_ptr m_conn;
	websocketpp::connection_hdl m_hdl;
	websocketpp::lib::error_code ec;
};

class WSConnection : public Connection
{
public:
	WSConnection();
	~WSConnection();

	virtual void closeConnection();
	virtual thread *start();

private:
	custom_ws_socket *socket;

	void m_onOpen(websocketpp::connection_hdl);
	void m_onClose(websocketpp::connection_hdl conn);
	void m_onFail(websocketpp::connection_hdl);
	void m_onMessage(client *cli, websocketpp::connection_hdl hdl, message_ptr msg);

	websocketpp::lib::thread *asio_thread = nullptr;
	websocketpp::lib::thread *telemetry_thread = nullptr;

	virtual void sendMessage(const GenericMessage &msg);
	virtual void receiveMessage(GenericMessage &msg);
};

#endif // __WS_CONECTION__