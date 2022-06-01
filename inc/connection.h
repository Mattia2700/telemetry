// Abstact class to know which function to implement for different connection types

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <queue>
#include <mutex>
#include <thread>
#include <string>
#include <unistd.h>
#include <functional>
#include <condition_variable>

using namespace std;

class GenericSocket {};
class GenericMessage {
public:
    GenericMessage(){};
    GenericMessage(const string& data_): data(data_){};
    GenericMessage(const string& topic_, const string& payload_):
                   topic(topic_), payload(payload_){};

    
    // use these if in subsub
    string topic;
    string payload;

    // use this if in raw
    string data;
};

class Connection {
    public:
        int getId();

        void init(const string& address, const string& port, const int& openMode);

        enum {
            NONE,
            PUB,
            SUB
        };

        struct message {
            string topic;
            string payload;
        };

        virtual void closeConnection() = 0;
        virtual thread* start() = 0;

        void clearData();
        void setData(const GenericMessage& message);

        void addOnOpen(function<void(const int& id)>);
        void addOnClose(function<void(const int& id, const int& code)>);
        void addOnError(function<void(const int& id, const int& code, const string& msg)>);
        void addOnMessage(function<void(const int& id, const GenericMessage&)>);

    protected:
        Connection();
        ~Connection();

        int id;

        string port;
        string address;
        int openMode;

        bool open = false;
        bool done = false;
        bool new_data = false;

        mutex mtx;
        condition_variable cv;
        queue<GenericMessage> buff_send;

    protected:

        void stop();
        void reset();

        void sendLoop();
        void receiveLoop();

        virtual void sendMessage(const GenericMessage& msg) = 0;
        virtual void receiveMessage(GenericMessage& msg) = 0;

        function<void(const int& id)> onOpen;
        function<void(const int& id, const int& code)> onClose;
        function<void(const int& id, const int& code, const string& msg)> onError;
        function<void(const int& id, const GenericMessage& msg)> onMessage;
    private:
        static int instance_count;
};

#endif