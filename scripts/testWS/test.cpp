using namespace std;

#include <iostream>
#include <string>
#include <unistd.h>

#include "wsclient.h"

void onMessage(const WebSocketClient::message& msg) {
    cout << "Message received: " << msg.topic << ": " << msg.payload << endl;
}

void onError(const int& code, const string& message) {
    cout << "Error: " << code << " - " << message << endl;
}

void onClose(const int& code) {
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
    WebSocketClient pub;
    pub.init("127.0.0.1", "5555", WebSocketClient::PUB);
    WebSocketClient sub;
    pub.init("127.0.0.1", "5555", WebSocketClient::SUB);

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

    sub.subscribe("test");

    while(true) {
        sleep(1);
        pub.setData("test", "Hello World");
    }

    sub.closeConnection();
    pub.closeConnection();

    return 0;
}