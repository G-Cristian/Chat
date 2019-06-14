#include "../../Include/connection.h"
#include <iostream>
#include <memory>
#include <string>


using namespace std;

int main(int argc, char *argv[]){
    if(argc < 3){
        cerr << "Not enough parameters." << endl;
        return 1;
    }

    ConnectionFactory connectionFactory;

    std::shared_ptr<Connection> connection = connectionFactory.createClientConnection(atoi(argv[2]), argv[1]);

    string msg;

    cin >> msg;

    if(!connection->sendMsg(msg)){
        cerr << "Error sending" << endl;
        return 1;
    }

    std::pair<int,std::string> rcvInfo;

    rcvInfo = connection->rcvMsg();
    cout << rcvInfo.second << endl;

    if(!connection->sendMsg("bye")){
        cerr << "Error sending" << endl;
        return 1;
    }

    rcvInfo = connection->rcvMsg();
    cout << rcvInfo.second << endl;

    if(rcvInfo.first == -1){
        cout << rcvInfo.second << endl;
        cerr << "Error receiving." << endl;
        return 1;
    }

    if(rcvInfo.first == 0){
        cout << rcvInfo.second << endl;
        cout << "Client closed connection." << endl;
    }

    cout << "Congrats" << endl;

    return 0;
}