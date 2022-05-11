# New Connection Type

## Introduction
To create a new connection type you should just implement all virtual functions in the abstract Connection class, that is in the inc/connection.h file.

Every message has to be converted in a ~~~message~~~.
The structure of this struct is show here below:
    ~~~
    struct message {
        string topic;
        string payload;
    };
    ~~~

To generalize the connection type you have to implement a class that will contain all the things you'll need for the connection, such as the context and the socket. The class you're going to create must inherit from GeneralSocket class, a void class that's created in the connection.h file.
Once you've done so, you can create your connection class, that will inherit from Connection (the abstract class mentioned above).

## Variables list

Here's the list of all the Connection class's variables:
    - These variables are needed to set up the connection. You can only set them by calling the Connection's init function, by passing to it them all.
    ~~~
    string address; // FORMAT EXAMPLE: [127.0.0.1]
    string port;    // FORMAT EXAMPLE: [8080]
    int openMode;   // FORMAT EXAMPLE: [PUB]/[SUB] (it's an enum)
    ~~~

    - This is the object mentioned above, where should be stored your custom socket class.
    ~~~
    GeneralSocket* socket;
    ~~~

    - These variables are checked inside the Connection class, so you have to manage them.
    For example you have to set the ~~~open = true~~~ when the connection has started; the ~~~done = true~~~ when you close the connection; ~~~new_data~~~ should never be modify, because is managed by the Connection class.
    ~~~
    bool done;
    bool open;
    bool new_data;
    ~~~

    - These variables are used in the Connection class to manage the reading and the writing operation.
    The ~~~mtx~~~ is used to lock the variables, so you can't read or write them at the same time; the ~~~cv~~~ is used to wait for the other thread to finish the operation; the ~~~buff_send~~~ is used to store the data that will be sent.
    ~~~
    mutex mtx;
    condition_variable cv;
    queue<message> buff_send;
    ~~~

## Methods to implement

Here below the list of all the needed methods:
    - The constructor should only call the Connection's contrsuctor.
    - The destructor should only call the Connection's destructor.
    - The close connection function does not need parameter, because all connection items you'll need should be in the GeneralSocket item.
    In this function you have to manage the connection closing.
    ~~~
    void closeConnection();
    ~~~
    - The subscribe function should manage the subscriber's subscription to a spcified topic
    ~~~
    void subscribe(const string& topic);
    ~~~
    - The subscribe function should manage the subscriber's unsubscription to a specified topic.
    ~~~
    void unsubscribe(const string& topic);
    ~~~
    - The send message function should take a ~~~message~~~ type message and ONLY send it.
    ~~~
    void sendMessage(const message& msg);
    ~~~
    - The receive message function should call the receive function of your connection and save the arguments in a ~~~message~~~ type message.
    ~~~
    void receiveMessage(message& msg);
    ~~~

## Already implemented methods (in the Connection class)

    -
    ~~~
    init();
    ~~~

## Usage