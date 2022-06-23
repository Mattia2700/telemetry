using namespace std;

#include <iostream>
#include <string>
#include <unistd.h>

#include "zmq_connection.h"

mutex mtx;

void onMessage(const int &id, const GenericMessage &msg)
{
    unique_lock<mutex> lck(mtx);
    cout << id << " RECV topic{" << msg.topic << "} payload: " << msg.payload << endl;
}

void onError(const int &id, const int &code, const string &message)
{
    cout << id << " Error: " << code << " - " << message << endl;
}

void onClose(const int &id, const int &code)
{
    cout << id << " Connection closed: " << code << endl;
}

void onOpen(const int &id)
{
    cout << id << " Connection opened" << endl;
}

int main()
{

    string message1 = string("topic\000payload", 14);

    const char *topic = message1.c_str();
    printf("%s\r\n", topic);
    printf("%s\r\n", (topic + strlen(topic) + 1));
    cout << message1 << endl;
    string topic_str = message1.c_str();
    string payload_str = message1.c_str() + topic_str.size() + 1;
    cout << topic_str << " ||| " << payload_str << endl;

    return 0;

    ZmqConnection pub;
    pub.init("127.0.0.1", "5555", ZmqConnection::PUB);
    ZmqConnection sub1;
    sub1.init("127.0.0.1", "5555", ZmqConnection::SUB);
    ZmqConnection sub2;
    sub2.init("127.0.0.1", "5555", ZmqConnection::SUB);

    // pub.add_on_message(onMessage);
    pub.addOnError(onError);
    pub.addOnClose(onClose);
    pub.addOnOpen(onOpen);

    sub1.addOnMessage(onMessage);
    sub1.addOnError(onError);
    sub1.addOnClose(onClose);
    sub1.addOnOpen(onOpen);
    sub2.addOnMessage(onMessage);
    sub2.addOnError(onError);
    sub2.addOnClose(onClose);
    sub2.addOnOpen(onOpen);

    thread *pub_thread = pub.start();
    thread *sub_thread1 = sub1.start();
    thread *sub_thread2 = sub2.start();

    sub1.subscribe("test1");
    sub2.subscribe("test1");
    sub2.subscribe("test2");

    while (true)
    {
        sleep(1);
        pub.setData(GenericMessage("test1", "Hello World"));
        pub.setData(GenericMessage("test2", "Secret"));
    }

    pub.closeConnection();
    sub1.closeConnection();
    sub2.closeConnection();

    return 0;
}