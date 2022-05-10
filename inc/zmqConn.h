#ifndef __ZMQ_LIB_H__
#define __ZMQ_LIB_H__

#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <queue>

#include "connection.h"
#include "zhelpers.hpp"

class ZMQ: public Connection {
    public:
        ZMQ(char* address, char* port, int openMode);
        
        void closeConnection();

        void subscribe(string topic, int len);
        void unsubscribe(string topic, int len);
        
        // to be deleted
        //
        
        void add_on_open(function<void()>);
        void add_on_close(function<void(int code)>clbk);
        void add_on_error(function<void(int code)>clbk);
        void add_on_message(function<void(zmq::socket_t*, message)> clbk);
        void add_on_subscribe(function<void(string)> clbk);
        void add_on_unsubscribe(function<void(string)> clbk);
    private:
        // to be deleted
        //

        thread* startPub();
        thread* startSub();

        // to be deleted
        //

        void sendMessage(string topic, string msg);
        void receiveMessage(string& topic, string& payload);

        function<void()> clbk_on_open;
        function<void(int code)> clbk_on_close;
        function<void(int code)> clbk_on_error;
        function<void(zmq::socket_t* socket, message msg)> clbk_on_message;
        function<void(string topic)> clbk_on_subscribe;
        function<void(string topic)> clbk_on_unsubscribe;
};

#endif
