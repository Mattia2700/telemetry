#ifndef __CLIENT_LIB_H__
#define __CLIENT_LIB_H__

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
        void setData(string id, string data);

        thread* start();

        void subscribe(string topic, int len);
        void unsubscribe(string topic, int len);

        enum {
            NONE,
            PUB,
            SUB
        };

        struct message {
            string topic;
            string payload;
        };
        
        void add_on_open(function<void()>);
        void add_on_close(function<void(int code)>clbk);
        void add_on_error(function<void(int code)>clbk);
        void add_on_message(function<void(zmq::socket_t*, message)> clbk);
        void add_on_subscribe(function<void(string)> clbk);
        void add_on_unsubscribe(function<void(string)> clbk);
    private:
        char* address;
        char* port;
        int openMode;

        zmq::context_t *context;
        zmq::socket_t  *socket;

        bool open = false;
        bool done = false;
        bool new_data = false;
        int errorCode = NULL;

        std::mutex mtx;
        condition_variable cv;
        std::queue<message> buff_send;

        thread* startPub();
        thread* startSub();

        void subLoop();
        void pubLoop();
        
        void reset();
        void stop();

        void sendMessage(string topic, string msg);
        void receiveMessage(string& topic, string& payload);
        void clearData();

        function<void()> clbk_on_open;
        function<void(int code)> clbk_on_close;
        function<void(int code)> clbk_on_error;
        function<void(zmq::socket_t* socket, message msg)> clbk_on_message;
        function<void(string topic)> clbk_on_subscribe;
        function<void(string topic)> clbk_on_unsubscribe;
};

#endif
