// Abstact class to know which function to implement for different connection types

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

using namespace std;

#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <queue>

// should not be here, should be generic
#include "zhelpers.hpp"

class Connection {
    protected:
        Connection(char* address, char* port, int openMode);
        
        enum {
            NONE,
            PUB,
            SUB
        };

        struct message {
            string topic;
            string payload;
        };

        // all private attributes and functions should be protected
        char* address;
        char* port;
        int openMode;

        // what should it be?
        zmq::context_t *context;
        zmq::socket_t  *socket;

        bool open = false;
        bool done = false;
        bool new_data = false;
        int errorCode = NULL;

        std::mutex mtx;
        condition_variable cv;
        std::queue<message> buff_send;

        virtual thread* startPub() = 0;
        virtual thread* startSub() = 0;

        void pubLoop();
        void subLoop();
        
        void reset();
        void stop();
        void clearData();

        virtual void sendMessage(string topic, string payload) = 0;
        virtual void receiveMessage(string& topic, string& msg) = 0;

        function<void()> clbk_on_open;
        function<void(int code)> clbk_on_close;
        function<void(int code)> clbk_on_error;
        function<void(zmq::socket_t* socket, message msg)> clbk_on_message;
        function<void(string topic)> clbk_on_subscribe;
        function<void(string topic)> clbk_on_unsubscribe;

    public:
        virtual void closeConnection() = 0;
        virtual void subscribe(string topic, int len) = 0;
        virtual void unsubscribe(string topic, int len) = 0;

        void setData(string id, string data);

        thread* start();

        virtual void add_on_open(function<void()>) = 0;
        virtual void add_on_close(function<void(int code)>) = 0;
        virtual void add_on_error(function<void(int code)>) = 0;
        virtual void add_on_message(function<void(zmq::socket_t*, message)>) = 0;
        virtual void add_on_subscribe(function<void(string)>) = 0;
        virtual void add_on_unsubscribe(function<void(string)>) = 0;
};

#endif