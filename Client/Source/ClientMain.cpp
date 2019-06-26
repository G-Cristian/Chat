#include "../../Include/commands.h"
#include "../../Include/connection.h"
#include "../../Include/debugger.h"
#include "../../Include/semaphore.h"
#include <iostream>
#include <memory>
#include <pthread.h>
#include <queue>
#include <string>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>


using namespace std;

void *listenerThread(void*);
void *inputThread(void*);

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


    if(fork()!= 0){
        int *inputThreadOutput = (int*)inputThread((void*)connection.get());
        Debugger::getInstance()->print("inputThreadOutput");
        Debugger::getInstance()->print(*inputThreadOutput);
        free(inputThreadOutput);
    }
    else{
        int *listenerThreadOutput = (int*)listenerThread((void*)connection.get());
        Debugger::getInstance()->print("listenerThreadOutput");
        Debugger::getInstance()->print(*listenerThreadOutput);
        free(listenerThreadOutput);
    }
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

void *listenerThread(void *data){
    int *ret = (int*)malloc(sizeof(int));
    *ret = 0;
    Connection *connection = (Connection*)data;
    //send message
    char cmd[300];
    string msg = "";
    std::pair<int,std::shared_ptr<char>> rcvInfo;
    while(true){
        int remainigRetries=10;
        bool messageReceived = false;
        while(remainigRetries > 0 && !messageReceived){
            messageReceived = true;
            Debugger::getInstance()->print("Receiving ...");
            rcvInfo = connection->rcvMsg();
            if(!rcvInfo.first){
                Debugger::getInstance()->printErr("Error receiving ...");
                messageReceived = false;
                --remainigRetries;
            }
        }
        
        if(!messageReceived){
            Debugger::getInstance()->printErr("Didn't receive.");
            *ret = 1;
            return (void*)ret;
        }
        char commandReceived = rcvInfo.second.get()[0];
        if(commandReceived == SERVER_COMMAND_STATUS){
            Debugger::getInstance()->print("Receiving status.");
            char statusReceived = rcvInfo.second.get()[1];
            if(statusReceived == STATUS_MESSAGE_SENT_WITH_FAILS){
                Debugger::getInstance()->print("Message sent with fails.");
            }
            else if(statusReceived != STATUS_OK){
                Debugger::getInstance()->print("Unknown status");
                Debugger::getInstance()->print(statusReceived);
            }
            else{
                Debugger::getInstance()->print("Message sent.");
            }
        }
        else if(commandReceived == SERVER_CLIENT_SENT_MESSAGE){
            Debugger::getInstance()->print("Message received: ");
            Debugger::getInstance()->print(&(rcvInfo.second.get()[1]));
        }
    }

    return (void*)ret;
}

void *inputThread(void *data){
    int *ret = (int*)malloc(sizeof(int));
    *ret = 0;
    Connection *connection = (Connection*)data;
    //send message
    char cmd[300];
    string msg = "";
    do{
        cout << "Enter message: ";
        getline(cin,msg);
        bool messageSent = false;
        Debugger::getInstance()->print("Starting to send msg.");
        int remainigRetries = 10;
        while(remainigRetries > 0 && !messageSent){
            messageSent = true;
            cmd[0] = CLIENT_SENDS_MESSAGE;
            strcpy(&(cmd[1]),msg.c_str());
            
            cout << "Sending msg ( "<< &(cmd[1]) << ")." << endl;
            if(!connection->sendMsg(cmd)){
                Debugger::getInstance()->printErr("Error sending");
                messageSent = false;
                --remainigRetries;
            }
        }
        if(!messageSent){
            Debugger::getInstance()->printErr("Message not sent.");
            *ret=1;
            return (void*)ret;
        }
    }while(msg != "quit()");

    return (void*)ret;
}