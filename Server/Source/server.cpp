#include "../../Include/commands.h"
#include "../Include/server.h"
#include <iostream>
#include <list>
#include <pthread.h>
#include <string>

const std::string Server::GLOBAL_GROUP_NAME = "_GLOBALGROUP";

int Server::start() {
    using namespace std;

    ConnectionFactory connectionFactory;

    _serverConnection = connectionFactory.createServerConnection(_port);

    if(_serverConnection == nullptr)
        return 0;

    if(!_serverConnection->listen()){
        return 0;
    }

    list <pthread_t> threads;
    Group *globalGroup = new Group();
    globalGroup->name = GLOBAL_GROUP_NAME;
    _groups[GLOBAL_GROUP_NAME] = shared_ptr<Group>(globalGroup);

    while(!_criticalError){
        std::shared_ptr<Connection> clientConnection = _serverConnection->acceptConnection();

        threads.push_back(crerateListenerThread(clientConnection));
    }

    for(auto it = threads.begin(); it != threads.end(); it++){
        pthread_join(*it, NULL);
    }

    return 1;
}

pthread_t Server::crerateListenerThread(std::shared_ptr<Connection> clientConnection){
    pthread_t threadId;
    ClientData *clientData = new ClientData();
    clientData->server = this;
    clientData->connection = clientConnection;

    int err = pthread_create(&threadId, NULL, Server::listen, (void*)clientData);

    return threadId;
}

void* Server::listen(void *data){
    using namespace std;

    shared_ptr<ClientData> clientData((ClientData*)data);
    Server *server = clientData->server;
    
    if(server->listenClient(clientData)){
        cout << "Congrats" << endl;
    }
    else{
        cout << "Error" << endl;
    }

    return NULL;
}

int Server::listenClient(std::shared_ptr<Server::ClientData> clientData){
    using namespace std;
    
    shared_ptr<Connection> clientConnection = clientData->connection;
    pair<int,shared_ptr<char>> rcvInfo;

    while((rcvInfo = clientConnection->rcvMsg()).first > 0){
        char command = rcvInfo.second.get()[0];
        switch(command){
            case CLIENT_CONNECTS:
                break;
            case CLIENT_SENDS_MESSAGE:
                    cout << &(rcvInfo.second.get()[1]) << endl;
                    clientConnection->sendMsg("Received ");
                break;
            case CLIENT_LEAVES_CHAT:
                break;
            default:
                throw "Should never enter this case option";
        }
    }

    if(rcvInfo.first == -1){
        cout << rcvInfo.second << endl;
        cerr << "Error receiving." << endl;
        return 0;
    }

    if(rcvInfo.first == 0){
        cout << rcvInfo.second << endl;
        cout << "Client closed connection." << endl;
    }

    return 1;
}