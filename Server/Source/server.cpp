#include "../../Include/commands.h"
#include "../../Include/debugger.h"
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
        Debugger::getInstance()->print("Waiting client connection.");
        std::shared_ptr<Connection> clientConnection = _serverConnection->acceptConnection();
        Debugger::getInstance()->print("Client connection created.");
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
    if(!err){
        Debugger::getInstance()->print("Created listener thread.");
    }
    else{
        Debugger::getInstance()->print("Error creating listener thread.");
    }

    return threadId;
}

void* Server::listen(void *data){
    using namespace std;

    shared_ptr<ClientData> clientData((ClientData*)data);
    Server *server = clientData->server;
    
    if(server->listenClient(clientData)){
        Debugger::getInstance()->print("Client disconnected successfully.");
    }
    else{
        Debugger::getInstance()->print("Error listening client.");
    }

    return NULL;
}

int Server::listenClient(std::shared_ptr<Server::ClientData> clientData){
    using namespace std;
    Debugger::getInstance()->print("Listening client.");
    
    shared_ptr<Connection> clientConnection = clientData->connection;
    pair<int,shared_ptr<char>> rcvInfo;
    Debugger::getInstance()->print("Receiving messages");
    while((rcvInfo = clientConnection->rcvMsg()).first > 0){
        char command = rcvInfo.second.get()[0];
        char *data = &(rcvInfo.second.get()[1]);
        size_t dataLen = strlen(data);
        switch(command){
            case CLIENT_CONNECTS:
            {
                char status = '\0';
                if(connectClient(string(data), clientData)){
                    Debugger::getInstance()->print("Client connected.");
                    status = STATUS_OK;
                }
                else{
                    Debugger::getInstance()->print("Error connecting client.");
                    status = STATUS_CLIENT_NAME_EXISTS;
                }
                if(sendStatus(clientConnection, status)){
                    Debugger::getInstance()->print("Status sent to client.");
                }
                else{
                    Debugger::getInstance()->print("Status not sent to client.");
                }
            }
            break;
            case CLIENT_SENDS_MESSAGE:
            {
                if(sendMessage(string(data), clientConnection)){
                    Debugger::getInstance()->print("No errors while sending message.");
                }
                else{
                    Debugger::getInstance()->print("Some errors occured while sending message.");
                }
            }
            break;
            case CLIENT_LEAVES_CHAT:
            {

            }
            break;
            default:{
                Debugger::getInstance()->print("Should never enter this case option");
                throw "Should never enter this case option";
            }
        }
        Debugger::getInstance()->print("Receiving messages");
    }

    Debugger::getInstance()->print("Exit receiving msg.");

    if(rcvInfo.first == -1){
        Debugger::getInstance()->print(rcvInfo.second.get());
        Debugger::getInstance()->print("Error receiving.");
        return 0;
    }

    if(rcvInfo.first == 0){
        Debugger::getInstance()->print(rcvInfo.second.get());
        Debugger::getInstance()->print("Client closed connection.");
    }

    return 1;
}

Server::Result Server::connectClient(const std::string &name, std::shared_ptr<Server::ClientData> clientData){
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

Server::Result Server::sendMessage(const std::string &msg, std::shared_ptr<Connection> clientConnection){
    Result res = 0;
    char status = STATUS_OK;
    ::lock(&_groupsServiceMutex);
    ::lock(&_groupsReadCountMutex);
    if(_groupsReadCount == 0){
        ::lock(&_groupsMutex);
    }
    ++_groupsReadCount;
    ::unlock(&_groupsReadCountMutex);
    ::unlock(&_groupsServiceMutex);

    auto groupIt = _groups.find(GLOBAL_GROUP_NAME);
    if(groupIt != _groups.end()){
        res = 1;
        char sendInfo[300];
        sendInfo[0] = SERVER_CLIENT_SENT_MESSAGE;
        strcpy(&(sendInfo[1]),msg.c_str());
        auto clientsIt = groupIt->second->clients.begin();
        auto end = groupIt->second->clients.end();
        for(;clientsIt != end; clientsIt++){
            std::shared_ptr<Connection> connection = (*clientsIt)->connection;
            if(clientConnection != connection){
                if(!connection->sendMsg(sendInfo)){
                    Debugger::getInstance()->print("Error sending message");
                    res = 0;
                    status = STATUS_MESSAGE_SENT_WITH_FAILS;
                }
                else{
                    Debugger::getInstance()->print("Message sent");
                }
            }
        }
    }
    else{
        res = 0;
        status = STATUS_GROUP_NAME_DOESNT_EXIST;
    }

    if(sendStatus(clientConnection, status)){
        Debugger::getInstance()->print("Status sent to client.");
    }
    else{
        Debugger::getInstance()->print("Status not sent to client.");
        res = 0;
    }

    ::lock(&_groupsReadCountMutex);
    --_groupsReadCount;
    if(_groupsReadCount == 0){
        ::unlock(&_groupsMutex);
    }
    ::unlock(&_groupsReadCountMutex);

    return res;
}

Server::Result Server::sendStatus(std::shared_ptr<Connection> clientConnection, char status, int retries/*=10*/){
    Result res = 0;
    char sendInfo[300];
    sendInfo[0] = SERVER_COMMAND_STATUS;
    sendInfo[1] = status;
    sendInfo[2] = '\0';
    
    int remainingRetries = retries;

    while(!clientConnection->sendMsg(sendInfo) && remainingRetries > 0){
        Debugger::getInstance()->print("Error sending status to client.");
        --remainingRetries;
    }
                
    if(remainingRetries > 0){
        res = 1;
    }
    else{
        res = 0;
    }

    return res;
}