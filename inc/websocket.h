#include <string>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/client.hpp>

using namespace std;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;
typedef websocketpp::client<websocketpp::config::asio_client> client;

class Client {
public:
    typedef websocketpp::lib::lock_guard<websocketpp::lib::mutex> scoped_lock;

    Client() : m_open(false),m_done(false) {
        // set up access channels to only log interesting things
        m_client.clear_access_channels(websocketpp::log::alevel::all);
        m_client.set_access_channels(websocketpp::log::alevel::connect);
        m_client.set_access_channels(websocketpp::log::alevel::disconnect);
        m_client.set_access_channels(websocketpp::log::alevel::app);

        // Initialize the Asio transport policy
        m_client.init_asio();

        // Bind the handlers we are using
        m_client.set_open_handler(bind(&Client::on_open,this,_1));
        m_client.set_close_handler(bind(&Client::on_close,this,_1));
        m_client.set_fail_handler(bind(&Client::on_fail,this,_1));
        // m_client.set_message_handler(bind(&oon_message,&m_client,::_1,::_2));
        m_new_data.store(false);
    }

    void set_on_message(void (*clbk)(client*, websocketpp::connection_hdl, message_ptr)){
      m_client.set_message_handler(bind(clbk,&m_client,::_1,::_2));
    }

    // This method will block until the connection is complete
    websocketpp::lib::thread* run(const std::string & uri) {
        // Create a new connection to the given URI
        client::connection_ptr con = m_client.get_connection(uri, ec);
        if (ec) {
            m_client.get_alog().write(websocketpp::log::alevel::app,
                    "Get Connection Error: "+ec.message());
            return nullptr;
        }

        // Grab a handle for this connection so we can talk to it in a thread
        // safe manor after the event loop starts.
        m_hdl = con->get_handle();

        // Queue the connection. No DNS queries or network connections will be
        // made until the io_service event loop is run.
        m_client.connect(con);


        asio_thread = new websocketpp::lib::thread(&client::run, &m_client);
        telemetry_thread = new websocketpp::lib::thread(&Client::loop,this);

        return telemetry_thread;
    }

    // The open handler will signal that we are ready to start sending telemetry
    void on_open(websocketpp::connection_hdl) {
        scoped_lock guard(m_lock);
        cout << "\n\n\n" << flush;
        m_open = true;
    }

    // The close handler will signal that we should stop sending telemetry
    void on_close(websocketpp::connection_hdl) {
        m_client.get_alog().write(websocketpp::log::alevel::app,
            "Connection closed, stopping telemetry!");

        {
          scoped_lock guard(m_lock);
          m_done = true;

          m_open = false;
        }

        // asio_thread->join();
        // telemetry_thread->join();
    }

    // The fail handler will signal that we should stop sending telemetry
    void on_fail(websocketpp::connection_hdl) {
        m_client.get_alog().write(websocketpp::log::alevel::app,
            "Connection failed, stopping telemetry!");

        scoped_lock guard(m_lock);
        m_done = true;
        m_open = false;
    }

    void set_data(string data){
      unique_lock<mutex> guard(m_worker_mtx);
      m_to_send_data.push(data);
      if(m_to_send_data.size() > 20)
        m_to_send_data.pop();
      m_new_data.store(true);
      m_cv.notify_all();
    }

    void loop(){
        while(!m_open)
          usleep(1000);
        while(!m_done){
          unique_lock<mutex> lck(m_worker_mtx);
          while (!m_new_data)
            m_cv.wait(lck);

          // m_client.get_alog().write(websocketpp::log::alevel::app, "Seding");
          m_client.send(m_hdl,m_to_send_data.front(),websocketpp::frame::opcode::binary,ec);
          // std::cout << "Sent" << std::endl;

          m_to_send_data.pop();

          if(m_to_send_data.empty())
            m_new_data.store(false);

          if (ec) {
            m_client.get_alog().write(websocketpp::log::alevel::app,"Send Error: "+ec.message());
            continue;
          }
        }
    }
private:
    client m_client;
    websocketpp::connection_hdl m_hdl;
    websocketpp::lib::mutex m_lock;

    websocketpp::lib::thread* asio_thread;
    websocketpp::lib::thread* telemetry_thread;


    bool m_open;
    bool m_done;
    websocketpp::lib::error_code ec;

    std::mutex m_worker_mtx;
    std::queue<string> m_to_send_data;
    atomic<bool> m_new_data;
    condition_variable m_cv;
};
