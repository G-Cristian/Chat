#include "../../Include/commands.h"
#include "../../Include/connection.h"
#include <iostream>
#include <memory>
#include <string>
#include <string.h>


using namespace std;

int main(int argc, char *argv[]){
    if(argc < 3){
        cerr << "Not enough parameters." << endl;
        return 1;
    }

    ConnectionFactory connectionFactory;

    std::shared_ptr<Connection> connection = connectionFactory.createClientConnection(atoi(argv[2]), argv[1]);

    string msg;

   // cin >> msg;

    //if(!connection->sendMsg(msg.c_str())){
     //   cerr << "Error sending" << endl;
      //  return 1;
    //}

    std::pair<int,std::shared_ptr<char>> rcvInfo;

    //rcvInfo = connection->rcvMsg();
    //cout << rcvInfo.second << endl;

    char cmd[300];
    string name;
    cin >> name;
    cmd[0] = CLIENT_CONNECTS;
    strcpy(&(cmd[1]),name.c_str());
    if(!connection->sendMsg(cmd)){
        cerr << "Error sending" << endl;
        return 1;
    }

    //rcvInfo = connection->rcvMsg();
    //cout << rcvInfo.second << endl;

    //if(rcvInfo.first == -1){
    //    cout << rcvInfo.second << endl;
    //    cerr << "Error receiving." << endl;
    //    return 1;
    //}

    //if(rcvInfo.first == 0){
    //    cout << rcvInfo.second << endl;
    //    cout << "Client closed connection." << endl;
    //}

    cout << "End" << endl;

    return 0;
}