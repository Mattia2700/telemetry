#ifndef __ZMQ_LIB_H__
#define __ZMQ_LIB_H__

#include <thread>
#include <string>

#include "connection.h"
#include "zhelpers.hpp"

class custom_zmq_socket {
    public:
        zmq::context_t* context;
        zmq::socket_t* socket;
};

class ZMQ: public Connection {
    public:
        ZMQ();
        ~ZMQ();
        
        virtual void closeConnection();
        virtual thread* start();

        int subscribe(const string& topic);
        int unsubscribe(const string& topic);

    private:
        custom_zmq_socket* socket;

        thread* startPub();
        thread* startSub();

        virtual void sendMessage(const GenericMessage& msg);
        virtual void receiveMessage(GenericMessage& msg);
};

#endif
