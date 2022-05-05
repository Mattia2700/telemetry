using namespace std;

#include <iostream>
#include <string>
#include "zmqpub.h"

void onMessage(zmq::socket_t* s, Connection::message msg) {
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

int main() {
    Connection pub("127.0.0.1", "5555", Connection::PUB);
    Connection sub("127.0.0.1", "5555", Connection::SUB);

    pub.add_on_message(onMessage);
    pub.add_on_error(onError);
    pub.add_on_close(onClose);
    pub.add_on_open(onOpen);

    sub.add_on_message(onMessage);
    sub.add_on_error(onError);
    sub.add_on_close(onClose);
    sub.add_on_open(onOpen);

    thread *pub_thread = pub.start();
    thread *sub_thread = sub.start();

    pub.setData("test", "Hello World");

    return 0;
}