#include "wsclient.h"

WebSocketClient::WebSocketClient() : m_open(false),m_done(false) {
      // set up access channels to only log interesting things
    m_client.clear_access_channels(websocketpp::log::alevel::all);
    m_client.set_access_channels(websocketpp::log::alevel::connect);
    m_client.set_access_channels(websocketpp::log::alevel::disconnect);
    m_client.set_access_channels(websocketpp::log::alevel::app);

    // Initialize the Asio transport policy
    m_client.init_asio();

    // Bind the handlers we are using
    m_client.set_open_handler(bind(&WebSocketClient::on_open,this,_1));
    m_client.set_close_handler(bind(&WebSocketClient::on_close,this,_1));
    m_client.set_fail_handler(bind(&WebSocketClient::on_fail,this,_1));
    m_new_data.store(false);
}

void WebSocketClient::close()
{
  m_client.close(m_hdl, 1000, "Closed by user");
}

websocketpp::lib::thread* WebSocketClient::run(const std::string & uri) {
    m_done = false;
    // Create a new connection to the given URI
    m_conn = m_client.get_connection(uri, ec);
    if (ec) {
        m_client.get_alog().write(websocketpp::log::alevel::app, "Get Connection Error: "+ec.message());
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
    telemetry_thread = new websocketpp::lib::thread(&WebSocketClient::loop,this);

    return telemetry_thread;
}


void WebSocketClient::loop(){
    while(!m_open)
      usleep(1000);
    while(!m_done){
      unique_lock<mutex> lck(m_worker_mtx);

      while (!m_new_data && !m_done)
        m_cv.wait(lck);

      // Adding this because code can be stuck waiting for m_cv
      // when actually the socket is being closed
      if(m_done)
        break;

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

//////// CALLBACKS ////////
void WebSocketClient::set_on_message(void (*clbk)(client*, websocketpp::connection_hdl, message_ptr)){
    m_client.set_message_handler(bind(clbk,&m_client,::_1,::_2));
}
void WebSocketClient::set_on_message(std::function<void(client*, websocketpp::connection_hdl, message_ptr)>clbk){
    m_client.set_message_handler(bind(clbk,&m_client,::_1,::_2));
}

void WebSocketClient::on_open(websocketpp::connection_hdl) {
    scoped_lock guard(m_lock);
    m_open = true;
    
    if(clbk_on_open)
      clbk_on_open();
}

void WebSocketClient::on_close(websocketpp::connection_hdl conn) {
    m_client.get_alog().write(websocketpp::log::alevel::app,
        "Connection closed, stopping telemetry!");
    stop();
    reset();
}

void WebSocketClient::stop()
{
    scoped_lock guard(m_lock);
    m_done = true;
    m_open = false;
    m_cv.notify_all();

    if(clbk_on_close)
      clbk_on_close(1000);
}

void WebSocketClient::reset()
{
    m_hdl.reset();
    m_client.reset();
}

void WebSocketClient::on_fail(websocketpp::connection_hdl) {
    m_client.get_alog().write(websocketpp::log::alevel::app,
        "Connection failed, stopping telemetry!");

    stop();
    reset();
}

void WebSocketClient::add_on_open(std::function<void()> clbk)
{
  clbk_on_open = clbk;
}

void WebSocketClient::add_on_close(std::function<void(int)> clbk)
{
  clbk_on_close = clbk;
}

void WebSocketClient::add_on_error(std::function<void(int)> clbk)
{
  clbk_on_error = clbk;
}
//////// END CALLBACKS ////////


void WebSocketClient::clear_data()
{
  unique_lock<mutex> guard(m_worker_mtx);
  while(!m_to_send_data.empty())
    m_to_send_data.pop();
}
void WebSocketClient::set_data(string data){
  unique_lock<mutex> guard(m_worker_mtx);
  m_to_send_data.push(data);
  if(m_to_send_data.size() > 20)
    m_to_send_data.pop();
  m_new_data.store(true);
  m_cv.notify_all();
}