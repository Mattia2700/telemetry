#include <string>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/client.hpp>

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

class WebSocketClient {
public:
    typedef websocketpp::lib::lock_guard<websocketpp::lib::mutex> scoped_lock;

    WebSocketClient();

    void close();
    
    void clear_data();
    void set_data(string data);

    void add_on_open(std::function<void()>);
    void add_on_close(std::function<void(int code)>clbk);
    void add_on_error(std::function<void(int code)>clbk);

    void set_on_message(void (*clbk)(client*, websocketpp::connection_hdl, message_ptr));

    // This method will block until the connection is complete
    websocketpp::lib::thread* run(const std::string & uri);

private:
    // The open handler will signal that we are ready to start sending telemetry
    void on_open(websocketpp::connection_hdl);

    // The close handler will signal that we should stop sending telemetry
    void on_close(websocketpp::connection_hdl);

    // The fail handler will signal that we should stop sending telemetry
    void on_fail(websocketpp::connection_hdl);

    void stop();
    void reset();


    void loop();
    client m_client;
    client::connection_ptr m_conn;
    websocketpp::connection_hdl m_hdl;
    websocketpp::lib::mutex m_lock;

    websocketpp::lib::thread* asio_thread;
    websocketpp::lib::thread* telemetry_thread;


    atomic<bool> m_open;
    atomic<bool> m_done;
    websocketpp::lib::error_code ec;

    std::mutex m_worker_mtx;
    std::queue<string> m_to_send_data;
    atomic<bool> m_new_data;
    condition_variable m_cv;


    ///
    
    std::function<void()> clbk_on_open;
    std::function<void(int code)> clbk_on_close;
    std::function<void(int code)> clbk_on_error;
};
