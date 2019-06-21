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
    bool connectedOk = false;
    do{
        bool messageSent = false;
        cout << "Starting to send name." << endl;
        int remainigRetries = 10;
        while(remainigRetries > 0 && !messageSent){
            name = "";
            messageSent = true;
            getline(cin,name);
            cout << "Name ( "<< name << ")." << endl;
            cmd[0] = CLIENT_CONNECTS;
            strcpy(&(cmd[1]),name.c_str());
            cout << "Sending name ( "<< *(&(cmd[1])) << ")." << endl;
            if(!connection->sendMsg(cmd)){
                cerr << "Error sending" << endl;
                messageSent = false;
                --remainigRetries;
            }
        }
        if(!messageSent){
            cerr << "Message not sent." << endl;
            return 1;
        }

        remainigRetries=10;
        bool messageReceived = false;
        while(remainigRetries > 0 && !messageReceived){
            messageReceived = true;
            cout << "Receiving status" << endl;
            rcvInfo = connection->rcvMsg();
            if(!rcvInfo.first){
                cerr << "Error receiving status." << endl;
                messageReceived = false;
                --remainigRetries;
            }
        }
        
        if(!messageReceived){
            cerr << "Status not received." << endl;
            return 1;
        }
        char commandReceived = rcvInfo.second.get()[0];
        if(commandReceived != SERVER_COMMAND_STATUS){
            cerr << "SERVER_COMMAND_STATUS (" << SERVER_COMMAND_STATUS <<") expected. Received " << commandReceived << endl;
            return 1;
        }
        char statusReceived = rcvInfo.second.get()[1];
        if(statusReceived == STATUS_CLIENT_NAME_EXISTS){
            cout << "Name already exist." << endl;
        }
        else if(statusReceived != STATUS_OK){
            cout << "Unknown status (" << statusReceived << ")." << endl;
        }
        else{
            cout << "Connected with name " << name << endl;
            connectedOk = true;
        }
    }while(!connectedOk);

    //send message
    msg = "";
    do{
        cout << "Enter message: ";
        getline(cin,msg);
        bool messageSent = false;
        cout << "Starting to send msg." << endl;
        int remainigRetries = 10;
        while(remainigRetries > 0 && !messageSent){
            messageSent = true;
            cmd[0] = CLIENT_SENDS_MESSAGE;
            strcpy(&(cmd[1]),msg.c_str());
            cout << "Sending msg ( "<< &(cmd[1]) << ")." << endl;
            if(!connection->sendMsg(cmd)){
                cerr << "Error sending" << endl;
                messageSent = false;
                --remainigRetries;
            }
        }
        if(!messageSent){
            cerr << "Message not sent." << endl;
            return 1;
        }

        remainigRetries=10;
        bool messageReceived = false;
        while(remainigRetries > 0 && !messageReceived){
            messageReceived = true;
            cout << "Receiving ..." << endl;
            rcvInfo = connection->rcvMsg();
            if(!rcvInfo.first){
                cerr << "Error receiving ..." << endl;
                messageReceived = false;
                --remainigRetries;
            }
        }
        
        if(!messageReceived){
            cerr << "Didn't receive." << endl;
            return 1;
        }
        char commandReceived = rcvInfo.second.get()[0];
        if(commandReceived == SERVER_COMMAND_STATUS){
            cout << "Receiving status." << endl;
            char statusReceived = rcvInfo.second.get()[1];
            if(statusReceived == STATUS_MESSAGE_SENT_WITH_FAILS){
                cout << "Message sent with fails." << endl;
            }
            else if(statusReceived != STATUS_OK){
                cout << "Unknown status (" << statusReceived << ")." << endl;
            }
            else{
                cout << "Message sent." << endl;
            }
        }
        else if(commandReceived == SERVER_CLIENT_SENT_MESSAGE){
            cout << "Message received: ";
            cout << &(rcvInfo.second.get()[1]) << endl;
        }
    }while(msg != "quit()");
/*
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

    cout << "End" << endl;
*/
    return 0;
}