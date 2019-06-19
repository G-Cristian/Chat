#include "../../Include/commands.h"
#include "../Include/server.h"
#include <iostream>
#include <list>
#include <pthread.h>
#include <string>
#include <string.h>

#define LOCK_WRITE_GROUP() do{ ::lock(&_groupsServiceMutex); ::lock(&_groupsMutex); ::unlock(&_groupsServiceMutex); }while(false)
#define UNLOCK_WRITE_GROUP() do{ ::unlock(&_groupsMutex); }while(false)

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
        cout << "Client disconnected successfully." << endl;
    }
    else{
        cout << "Error listening client." << endl;
    }

    return NULL;
}

int Server::listenClient(std::shared_ptr<Server::ClientData> clientData){
    using namespace std;
    
    shared_ptr<Connection> clientConnection = clientData->connection;
    pair<int,shared_ptr<char>> rcvInfo;

    while((rcvInfo = clientConnection->rcvMsg()).first > 0){
        char command = rcvInfo.second.get()[0];
        char *data = &(rcvInfo.second.get()[1]);
        size_t dataLen = strlen(data);
        switch(command){
            case CLIENT_CONNECTS:
                if(connectClient(string(data), clientData)){
                    cout << "Client connected." << endl;
                    //TODO: Send message to client that connection was a success.
                }
                else{
                    cout << "Error connecting client." << endl;
                    //TODO: Send message to client that name already exist.
                }
                break;
            case CLIENT_SENDS_MESSAGE:
                    if(sendMessage(string(data))){
                        cout << "Message sent." << endl;
                    }
                    else{
                        cout << "Error sending message." << endl;
                    }
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

Server::Result Server::connectClient(const std::string &name, std::shared_ptr<ClientData> clientData){
    using namespace std;
    Result result = 1;
    LOCK_WRITE_GROUP();
    ::lock(&_clientsMutex);
    
    auto server = clientData->server;
    auto clientsIt = _clients.find(name);
    if(clientsIt == _clients.end()){
        _clients.insert({name, clientData});
        _groups[GLOBAL_GROUP_NAME]->clients.push_back(clientData);
    }
    else{
        result = 0;
    }
    
    ::unlock(&_clientsMutex);
    UNLOCK_WRITE_GROUP();
    return result;
}

Server::Result Server::sendMessage(const std::string&){
    ::lock(&_groupsServiceMutex);
    ::lock(&_groupsReadCountMutex);
    if(_groupsReadCount == 0){
        ::lock(&_groupsMutex);
    }
    ++_groupsReadCount;
    ::unlock(&_groupsReadCountMutex);
    ::unlock(&_groupsServiceMutex);

    //TODO: Send message to all member of group

    ::lock(&_groupsReadCountMutex);
    --_groupsReadCount;
    if(_groupsReadCount == 0){
        ::unlock(&_groupsMutex);
    }
    ::unlock(&_groupsReadCountMutex);

    return 1;
}