using namespace std;

#include "zmqpub.h"
#include "zhelpers.hpp"

// passing address (string) and port (int) it will enstablish a connection
Connection::Connection(char* address, char* port, int openMode) {
    this->address = address;
    this->port = port;
    this->openMode = openMode;

    if(openMode == 1) {
        startPub();
    } else if(openMode == 2) {
        startSub();
    }
}

thread* Connection::startPub() {
    context = new zmq::context_t(1);
    publisher = new zmq::socket_t(*context, ZMQ_PUB);

    stringstream server;

    server << "tcp://" << address << ":" << port;
    //cout << "Trying to connect at " << server.str() << endl;

    (*publisher).bind(server.str());

    //cout << "Connection enstablished." << endl << "Wait a second..." << endl;

    //  Ensure subscriber connection has time to complete
    sleep(1);

    thread* telemetry_thread = new thread(&Connection::loop, this);

    return telemetry_thread;
}

thread* Connection::startSub() {
    
}

// before ending the program remember to close the connection
void Connection::closeConnection() {
    publisher->close();
    context->close();

    done = true;
    cv.notify_all();
    //cout << "Connection closed." << endl;
}

void Connection::loop() {
    while(!open) usleep(10000);
    while(!done) {
        unique_lock<mutex> lck(mtx);

        while(!new_data && !done) cv.wait(lck);

        // Adding this because code can be stuck waiting for cv
        // when actually the socket is being closed
        if(done) break;

        message msg = buff_send.front();

        // convert them to char*
        char* topic = new char[4];
        topic = (char*) &msg.topic;
        this->sendMessage(topic, (char*)msg.payload);

        delete topic; // è molto brutto cit. phil

        buff_send.pop();

        if(buff_send.empty()) new_data = false;

        if(errorCode) {
            // write a message
            continue;
        }
    }
}

void Connection::clearData() {
    unique_lock<mutex> guard(mtx);

    while(!buff_send.empty()) {
        buff_send.pop();
    }
}

void Connection::setData(uint32_t id, uint8_t data[]) {
    unique_lock<mutex> guard(mtx);
    
    message msg;

    msg.topic = id;
    for(int i = 0; i < sizeof(data); i++) {
        msg.payload[i] = data[i];
    }

    buff_send.push(msg);

    if(buff_send.size() > 20) {
        buff_send.pop();
    }

    new_data = true;
    cv.notify_all();
}

// passing the topic and the message it will send it
void Connection::sendMessage(char* topic, char* msg) {
    //cout << topic << ": " << msg << endl;
    int rc;
    rc = s_sendmore(*publisher, string(topic)); // #id
    if(rc < 0) {
        if(clbk_on_error) {
            clbk_on_error(rc);
        }
    }

    rc = s_send(*publisher, string(msg)); // message

    if(rc < 0) {
        if(clbk_on_error) {
            clbk_on_error(rc);
        }
    }
}

void Connection::stop() {
    scoped_lock guard(mtx);
    done = true;
    open = false;

    cv.notify_all();

    if(clbk_on_close) {
        clbk_on_close(1000);
    }
}

void Connection::reset() {
    scoped_lock guard(mtx);
    done = false;
    open = false;
    new_data = false;
    errorCode = NULL;
    clearData();
}

////////////////////////////////////////////////////////////////////
//////////////////////////    CALLBACKS   //////////////////////////
////////////////////////////////////////////////////////////////////

void Connection::add_on_open(function<void()> clbk) {
    clbk_on_open = clbk;
}

void Connection::add_on_close(function<void(int)> clbk) {
    clbk_on_close = clbk;
}

void Connection::add_on_error(function<void(int)> clbk) {
    clbk_on_error = clbk;
}


void Connection::set_on_message(function<void(client*, websocketpp::connection_hdl, message_ptr)> clbk) {

}

void Connection::set_on_message(void (*clbk)(client*, websocketpp::connection_hdl, message_ptr)) {
    
}


void Connection::on_open(websocketpp::connection_hdl) {

}

void Connection::on_close(websocketpp::connection_hdl) {

}

void Connection::on_fail(websocketpp::connection_hdl) {
    cout << "Connection failed." << endl << "Stopping telemetry!" << endl;

    stop();     // stop the telemetry
    reset();    // reset the connection
}