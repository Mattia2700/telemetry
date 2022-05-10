#ifndef __ZMQ_LIB_H__
#define __ZMQ_LIB_H__

#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <queue>

#include "connection.h"
#include "zhelpers.hpp"

class custom_zmq_socket : GeneralSocket {
    public:
        zmq::context_t* context;
        zmq::socket_t* socket;
};

class ZMQ: public Connection {
    public:
        ZMQ();
        
        void closeConnection();

        void subscribe(const string& topic);
        void unsubscribe(const string& topic);

    private:
        custom_zmq_socket* socket;

        thread* startPub();
        thread* startSub();

        void sendMessage(const message& msg);
        void receiveMessage(message& msg);
};

#endif
