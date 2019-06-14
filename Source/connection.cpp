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

int Connection::sendMsg(const std::string &msg)const{
    int len = msg.length() + 1;
    int sent = 0;
    const char *str = msg.c_str();
    do{
        len -= sent;
        str += sent;
        sent = send(endpoint, str, len, 0);
        if(sent == -1){
            return 0;
        }
    }while(sent != len);

    return 1;
}

std::pair<int, std::string> Connection::rcvMsg()const{
    char buff[50];

    int recvLen = 0;
    recvLen=recv(endpoint, (void*)buff,50,0);
   // if(recvLen == -1){
   //     return {-1, ""};
   // }
    return {recvLen, buff};
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