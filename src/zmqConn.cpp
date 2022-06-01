using namespace std;

#include "zmqConn.h"
#include "zhelpers.hpp"

// passing address (string) and port (int) it will enstablish a connection
ZMQ::ZMQ() : Connection() {
    /*code*/
}

ZMQ::~ZMQ() {
    delete socket->socket;
    delete socket->context;
}

// it will connect to the server using the selected mode
thread* ZMQ::start() {
    thread* t;

    if(openMode == PUB) {
        t = startPub(); // start a thread for publishing
    } else if(openMode == SUB) {
        t = startSub(); // start a thread for subscribing
    }

    return t;
}

thread* ZMQ::startPub() {
    socket = new custom_zmq_socket();
    socket->context = new zmq::context_t(1);
    socket->socket = new zmq::socket_t(*socket->context, ZMQ_PUB);

    stringstream server;

    server << "tcp://" << address << ":" << port;

    try {
        (*socket->socket).bind(server.str());
    } catch (zmq::error_t e) {
        if(onError) {
            onError(id, e.num(), e.what());
        }

        if(onClose) {
            onClose(id, e.num());
        }
    }

    open = true;

    if(onOpen) {
        onOpen(id);
    }

    thread* telemetry_thread = new thread(&ZMQ::sendLoop, this);

    return telemetry_thread;
}

thread* ZMQ::startSub() {
    socket = new custom_zmq_socket();
    socket->context = new zmq::context_t(1);
    socket->socket = new zmq::socket_t(*socket->context, ZMQ_SUB);

    stringstream server;

    server << "tcp://" << address << ":" << port;

    try {
        (*socket->socket).connect(server.str());
    } catch(zmq::error_t& e) {
        if(onError) {
            onError(id, e.num(), e.what());
        }

        if(onClose) {
            onClose(id, e.num());
        }
    }

    open = true;

    if(onOpen) {
        onOpen(id);
    }

    thread* telemetry_thread = new thread(&ZMQ::receiveLoop, this);

    return telemetry_thread;
}

int ZMQ::subscribe(const string& topic) {
    try {
        (*socket->socket).setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
    } catch(zmq::error_t& e) {
        if(onError)
            onError(id, e.num(), e.what());
        return -1;
    }
    return 0;
}

int ZMQ::unsubscribe(const string& topic) {
    try {
        (*socket->socket).setsockopt(ZMQ_UNSUBSCRIBE, topic.c_str(), topic.size());
    } catch(zmq::error_t& e) {
        if(onError)
            onError(id, e.num(), e.what());
        return -1;
    }
    return 0;

}

// before ending the program remember to close the connection
void ZMQ::closeConnection() {
    socket->socket->close();
    socket->context->close();

    if(onClose) {
        onClose(id, 0);   // 0 means no error
    }

    done = true;
    cv.notify_all();
}

// passing the topic and the message it will send it
void ZMQ::sendMessage(const GenericMessage& msg) {
    int rc;
    rc = s_sendmore(*(socket->socket), msg.topic); // #id
    if(rc < 0) {
        if(onError) {
            onError(id, rc, "Error seding topic");
        }
    }

    rc = s_send(*(socket->socket), msg.payload); // message

    if(rc < 0) {
        if(onError) {
            onError(id, rc, "Error sending payload");
        }
    }
}

void ZMQ::receiveMessage(GenericMessage& msg) {
    try {
        msg.topic = s_recv(*(socket->socket));
        msg.payload = s_recv(*(socket->socket));
    } catch(zmq::error_t& e) {
        if(onError) {
            onError(id, e.num(), e.what());
        }
    }
}