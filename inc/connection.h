// Abstact class to know which function to implement for different connection types

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

using namespace std;

#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <queue>
#include <functional>
#include <unistd.h>

class GeneralSocket {};

class Connection {
    public:
        enum {
            NONE,
            PUB,
            SUB
        };

        struct message {
            string topic;
            string payload;
        };

        void init(const string& address, const string& port, const int& openMode);

        virtual void closeConnection() = 0;
        virtual void subscribe(const string& topic, const int& len) = 0;
        virtual void unsubscribe(const string& topic, const int& len) = 0;

        void setData(string id, string data);

        thread* start();

        void add_on_open(function<void()>);
        void add_on_close(function<void(const int& code)>);
        void add_on_error(function<void(const int& code)>);
        void add_on_message(function<void(const message&)>);
        void add_on_subscribe(function<void(const string&)>);
        void add_on_unsubscribe(function<void(const string&)>);

    protected:
        Connection();

        string address;
        string port;
        int openMode;

        GeneralSocket* socket;

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

        virtual void sendMessage(const message& msg) = 0;
        virtual void receiveMessage(message& msg) = 0;

        function<void()> clbk_on_open;
        function<void(const int& code)> clbk_on_close;
        function<void(const int& code)> clbk_on_error;
        function<void(const message& msg)> clbk_on_message;
        function<void(const string& topic)> clbk_on_subscribe;
        function<void(const string& topic)> clbk_on_unsubscribe;
};

#endif