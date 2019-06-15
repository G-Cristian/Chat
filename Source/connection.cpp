#include "../Include/connection.h"

int Connection::listen() const{
    if(::listen(endpoint, 50) == -1){
        throw "Cannot listen.";
    }

    return 1;
}

std::shared_ptr<Connection> Connection::acceptConnection()const{
    std::shared_ptr<Connection> connection = std::make_shared<Connection>();
    connection->port = port;
    connection->sockaddrLen = sizeof(struct sockaddr_in);
    connection->endpoint = accept(endpoint, (struct sockaddr*)&(connection->sockaddr), &(connection->sockaddrLen));
    if(connection->endpoint == -1){
        throw "Error accept.";
    }

    return connection;
}

std::shared_ptr<Connection> ConnectionFactory::createServerConnection(PORT port) const{
    struct in_addr inAddr;
    inAddr.s_addr = INADDR_ANY;

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr = inAddr;

    int endpoint = socket(AF_INET, SOCK_STREAM, 0);

    if(endpoint == -1){
        throw "Error creating endpoint.";
    }

    std::shared_ptr<Connection> connection = std::make_shared<Connection>();
    connection->port = port;
    connection->endpoint = endpoint;
    connection->sockaddr = server;
    connection->sockaddrLen = sizeof(connection->sockaddr);

    if(bind(endpoint, (struct sockaddr*)&(connection->sockaddr), connection->sockaddrLen) == -1){
        throw "Cannot bind.";
    }

    return connection;
}

std::shared_ptr<Connection> ConnectionFactory::createClientConnection(PORT port, const std::string &ip) const{
    struct in_addr inAddr;
    int ok = inet_aton(ip.c_str(), &inAddr);
    if(!ok){
        throw "Invalid ip address.";
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr = inAddr;

    SOCKET endpoint = socket(AF_INET, SOCK_STREAM, 0);

    if(endpoint == -1){
        throw "Error creating endpoint.";
    }

    std::shared_ptr<Connection> connection = std::make_shared<Connection>();
    connection->port = port;
    connection->endpoint = endpoint;
    connection->sockaddr = server;
    connection->sockaddrLen = sizeof(connection->sockaddr);


    if(connect(endpoint, (struct sockaddr*)&(connection->sockaddr), connection->sockaddrLen) == -1){
        throw "Error connecting to server.";
    }

    return connection;
}