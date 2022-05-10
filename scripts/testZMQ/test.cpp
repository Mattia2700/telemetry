using namespace std;

#include <iostream>
#include <string>
#include "zmqConn.h"

void onMessage(zmq::socket_t* s, ZMQ::message msg) {
    // serve davvero la socket?
    cout << "Message received: " << msg.topic << ": " << msg.payload << endl;
}

void onError(int code) {
    cout << "Error: " << code << endl;
}

void onClose(int code) {
    cout << "Connection closed: " << code << endl;
}

void onOpen() {
    cout << "Connection opened" << endl;
}

void onSubscribe(string topic) {
    cout << "Subscribed at " << topic << endl;
}

void onUnsubscribe(string topic) {
    cout << "Unsubscribed from " << topic << endl;
}

int main() {
    ZMQ pub("127.0.0.1", "5555", ZMQ::PUB);
    ZMQ sub("127.0.0.1", "5555", ZMQ::SUB);

    //pub.add_on_message(onMessage);
    pub.add_on_error(onError);
    pub.add_on_close(onClose);
    pub.add_on_open(onOpen);

    sub.add_on_message(onMessage);
    sub.add_on_error(onError);
    sub.add_on_close(onClose);
    sub.add_on_open(onOpen);
    sub.add_on_subscribe(onSubscribe);
    sub.add_on_unsubscribe(onUnsubscribe);

    thread *pub_thread = pub.start();
    thread *sub_thread = sub.start();

    sub.subscribe("test", 5);

    while(true) {
        sleep(1);
        pub.setData("test", "Hello World");
    }

    sub.closeConnection();
    pub.closeConnection();

    return 0;
}