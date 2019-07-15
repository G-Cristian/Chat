#ifndef _SERVER_H_
#define _SERVER_H_

#include "../../Include/connection.h"
#include "../../Include/semaphore.h"
#include <list>
#include <map>
#include <memory>
#if defined(__linux__) || defined(__unix__)
//linux based
#include <pthread.h>
#elif defined(_WIN32) || defined(WIN32)
//windows based
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#else
#error "OS not supperted."
#endif
#include <set>
#include <string>

class Server{
public:
    typedef int Result;
    Server(int port):
        _criticalError(false),
        _port(port),
        _groupsReadCount(0){
        ::initMutex(&_clientsMutex);
        ::initMutex(&_groupsMutex);
        ::initMutex(&_groupsReadCountMutex);
        ::initMutex(&_groupsServiceMutex);
    }

    ~Server(){
        ::destroyMutex(&_groupsServiceMutex);
        ::destroyMutex(&_groupsReadCountMutex);
        ::destroyMutex(&_groupsMutex);
        ::destroyMutex(&_clientsMutex);
    }
    Result start();
private:
    Server(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(const Server&)=delete;
    Server& operator=(Server&&)=delete;

    static const std::string GLOBAL_GROUP_NAME;

    struct ClientData{
        Server* server;
        std::string name;
        std::shared_ptr<Connection> connection;
        std::set<std::string> groups;

        ClientData():server(nullptr), name(""), connection(nullptr)
        {}
        ClientData(const ClientData&) = delete;
        ClientData(ClientData &&) = delete;
        ClientData& operator=(const ClientData&) = delete;
        ClientData& operator=(ClientData &&) = delete;
    };

    struct Group{
        std::string name;
        std::list<std::shared_ptr<ClientData>> clients;

        Group():name("")
        {}
        Group(const Group&) = delete;
        Group(Group &&) = delete;
        Group& operator=(const Group&) = delete;
        Group& operator=(Group &&) = delete;
    };

#if defined(__linux__) || defined(__unix__)
	//linux based
    pthread_t crerateListenerThread(std::shared_ptr<Connection>);
    static void* listen(void*);
#elif defined(_WIN32) || defined(WIN32)
	//windows based
	HANDLE crerateListenerThread(std::shared_ptr<Connection>);
	static DWORD WINAPI listen(LPVOID);
#else
#error "OS not supperted."
#endif
    Result listenClient(std::shared_ptr<ClientData>);
    Result connectClient(const std::string&, std::shared_ptr<ClientData>);
    Result removeClient(std::shared_ptr<ClientData>);
    Result sendMessage(const std::string&/*msg*/, std::shared_ptr<Connection> /*clientConnection*/);
    Result sendStatus(std::shared_ptr<Connection>, char, int retries=10);
    Result sendClientLeftCommand(const std::string&/*clientName*/, std::shared_ptr<Connection> /*clientConnection*/);

    bool _criticalError;
    int _port;
    std::shared_ptr<Connection> _serverConnection;
    Mutex _clientsMutex;
    Mutex _groupsMutex;
    Mutex _groupsReadCountMutex;
    Mutex _groupsServiceMutex;
    int _groupsReadCount;

    std::map<std::string, std::shared_ptr<ClientData>> _clients;
    std::map<std::string, std::shared_ptr<Group>> _groups;
};

#endif

