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
        thread* startPub();
        thread* startSub();
        void closeConnection();
        void setData(uint32_t id, uint8_t data[]);
    private:
        char* address;
        char* port;
        int openMode;

        zmq::context_t *context;
        zmq::socket_t  *publisher;

        bool open;
        bool done;
        bool new_data;
        int errorCode;

        struct message {
            uint32_t topic;
            uint8_t payload[];
        };

        std::mutex mtx;
        condition_variable cv;
        // should be a pair of topic and message (maybe also ID)
        std::queue<message> buff_send;

        void reset();
        void stop();

        void loop();
        void sendMessage(char* topic, char* msg);
        void clearData();

        void add_on_open(std::function<void()>);
        void add_on_close(std::function<void(int code)>clbk);
        void add_on_error(std::function<void(int code)>clbk);

        void set_on_message(std::function<void(client*, websocketpp::connection_hdl, message_ptr)>clbk);

        void set_on_message(void (*clbk)(client*, websocketpp::connection_hdl, message_ptr));

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
        function<void(string topic, string msg)> clbk_on_message;
};

#endif
