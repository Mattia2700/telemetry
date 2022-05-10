using namespace std;

#include "connection.h"

Connection::Connection() {
    open = false;
    done = false;
    new_data = false;
    errorCode = NULL;
}

// passing address, port and mode it will save it in the class
void Connection::init(const string& address, const string& port, const int& openMode) {
    this->address = address;
    this->port = port;
    this->openMode = openMode;
}

// it will connect to the server using the selected mode
thread* Connection::start() {
    thread* t;

    if(openMode == PUB) {
        t = startPub(); // start a thread for publishing
    } else if(openMode == SUB) {
        t = startSub(); // start a thread for subscribing
    }

    return t;
}

// this loop check if there are new data to send
void Connection::pubLoop() {
    while(!open) usleep(10000);
    while(!done) {
        unique_lock<mutex> lck(mtx);

        while(!new_data && !done) cv.wait(lck);

        // Adding this because code can be stuck waiting for cv
        // when actually the socket is being closed
        if(done) break;

        message msg = buff_send.front();
        
        this->sendMessage(msg);

        buff_send.pop();

        if(buff_send.empty()) new_data = false;
    }
}

// this loop check if there are new data to receive
// the receive function is blocking, so it will wait for new data
void Connection::subLoop() {
    while(!open) usleep(10000);
    while(!done) {
        message msg;

        this->receiveMessage(msg);

        if(clbk_on_message) {
            clbk_on_message(msg);
        }
    }
}

void Connection::clearData() {
    unique_lock<mutex> guard(mtx);

    while(!buff_send.empty()) {
        buff_send.pop();
        new_data = false;
    }
}

void Connection::setData(string id, string data) {
    unique_lock<mutex> guard(mtx);
    
    message msg;

    msg.topic = id;
    msg.payload = data;

    buff_send.push(msg);

    if(buff_send.size() > 20) {
        buff_send.pop();
    }

    if(!new_data) new_data = true;

    cv.notify_all();
}

void Connection::stop() {
    scoped_lock guard(mtx);
    done = true;
    open = false;

    cv.notify_all();

    if(clbk_on_close) {
        clbk_on_close(1000);
    }
}

void Connection::reset() {
    scoped_lock guard(mtx);
    done = false;
    open = false;
    new_data = false;
    errorCode = NULL;
    clearData();
}

////////////////////////////////////////////////////////////////////
//////////////////////////    CALLBACKS   //////////////////////////
////////////////////////////////////////////////////////////////////

void Connection::add_on_open(function<void()> clbk) {
    clbk_on_open = clbk;
}

void Connection::add_on_close(function<void(const int&)> clbk) {
    clbk_on_close = clbk;
}

void Connection::add_on_error(function<void(const int&)> clbk) {
    clbk_on_error = clbk;
}


void Connection::add_on_message(function<void(const message&)> clbk) {
    clbk_on_message = clbk;
}

void Connection::add_on_subscribe(function<void(const string&)> clbk) {
    clbk_on_subscribe = clbk;
}

void Connection::add_on_unsubscribe(function<void(const string&)> clbk) {
    clbk_on_unsubscribe = clbk;
}