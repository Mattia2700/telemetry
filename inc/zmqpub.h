#ifndef __CLIENT_LIB_H__
#define __CLIENT_LIB_H__

#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <queue>

#include "zhelpers.hpp"

class Connection {
    public:
        Connection(char* address, char* port, int openMode);
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
    private:
        char* address;
        char* port;
        int openMode;

        zmq::context_t *context;
        zmq::socket_t  *socket;

        bool open;
        bool done;
        bool new_data;
        int errorCode;

        std::mutex mtx;
        condition_variable cv;
        // should be a pair of topic and message (maybe also ID)
        std::queue<message> buff_send;

        thread* startPub();
        thread* startSub();

        void subLoop();
        void pubLoop();
        
        void reset();
        void stop();

        void sendMessage(string topic, string msg);
        void clearData();

        /*
        // The open handler will signal that we are ready to start sending telemetry
        void on_open(websocketpp::connection_hdl);

        // The close handler will signal that we should stop sending telemetry
        void on_close(websocketpp::connection_hdl);

        // The fail handler will signal that we should stop sending telemetry
        void on_fail(websocketpp::connection_hdl);
        */

        std::function<void()> clbk_on_open;
        std::function<void(int code)> clbk_on_close;
        std::function<void(int code)> clbk_on_error;
        function<void(zmq::socket_t* socket, message msg)> clbk_on_message;
};

#endif
