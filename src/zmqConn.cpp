using namespace std;

#include "zmqConn.h"
#include "zhelpers.hpp"

// passing address (string) and port (int) it will enstablish a connection
ZMQ::ZMQ() : Connection {/*arguments*/} {
    /*code*/
}

thread* ZMQ::startPub() {
    socket = new custom_zmq_socket();
    socket->context = new zmq::context_t(1);
    socket->socket = new zmq::socket_t(*socket->context, ZMQ_PUB);

    stringstream server;

    server << "tcp://" << address << ":" << port;
    //cout << "Trying to connect at " << server.str() << endl;

    try {
        (*socket->socket).bind(server.str());
    } catch (zmq::error_t e) {
        clbk_on_error(e.num());
        clbk_on_close(e.num());
    }
    
    //cout << "Connection enstablished." << endl << "Wait a second..." << endl;

    // Ensure publisher connection has time to complete
    sleep(1);

    open = true;

    if(clbk_on_open) {
        clbk_on_open();
    }

    thread* telemetry_thread = new thread(&ZMQ::pubLoop, this);

    return telemetry_thread;
}

thread* ZMQ::startSub() {
    socket = new custom_zmq_socket();
    socket->context = new zmq::context_t(1);
    socket->socket = new zmq::socket_t(*socket->context, ZMQ_PUB);

    stringstream server;

    server << "tcp://" << address << ":" << port;
    //cout << "Trying to connect at " << server.str() << endl;

    try {
        (*socket->socket).connect(server.str());
    } catch(zmq::error_t& e) {
        if(clbk_on_error) {
            clbk_on_error(e.num());
        }

        if(clbk_on_close) {
            clbk_on_close(e.num());
        }
    }

    //cout << "Connection enstablished." << endl << "Wait a second..." << endl;

    // Ensure subscriber connection has time to complete
    sleep(1);

    open = true;

    if(clbk_on_open) {
        clbk_on_open();
    }

    thread* telemetry_thread = new thread(&ZMQ::subLoop, this);

    return telemetry_thread;
}

void ZMQ::subscribe(const string& topic, const int& len) {
    try {
        (*socket->socket).setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), 4);
    } catch(zmq::error_t& e) {
        clbk_on_error(e.num());
    }

    if(clbk_on_subscribe) {
        clbk_on_subscribe(topic);
    }
}

void ZMQ::unsubscribe(const string& topic, const int& len) {
    try {
        (*socket->socket).setsockopt(ZMQ_UNSUBSCRIBE, topic.c_str(), len);
    } catch(zmq::error_t& e) {
        clbk_on_error(e.num());
    }

    if(clbk_on_unsubscribe) {
        clbk_on_unsubscribe(topic);
    }
}

// before ending the program remember to close the connection
void ZMQ::closeConnection() {
    socket->socket->close();
    socket->context->close();

    if(clbk_on_close) {
        clbk_on_close(0);   // 0 means no error
    }

    done = true;
    cv.notify_all();
    //cout << "Connection closed." << endl;
}

// passing the topic and the message it will send it
void ZMQ::sendMessage(const message& msg) {
    //cout << topic << ": " << msg << endl;
    int rc;
    rc = s_sendmore(*(socket->socket), msg.topic); // #id
    if(rc < 0) {
        if(clbk_on_error) {
            clbk_on_error(rc);
        }
    }

    rc = s_send(*(socket->socket), msg.payload); // message

    if(rc < 0) {
        if(clbk_on_error) {
            clbk_on_error(rc);
        }
    }
}

void ZMQ::receiveMessage(message& msg) {
    try {
        msg.topic = s_recv(*(socket->socket));
        msg.payload = s_recv(*(socket->socket));
    } catch(zmq::error_t& e) {
        if(clbk_on_error) {
            clbk_on_error(e.num());
        }
    }
}