#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <arpa/inet.h>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

struct Connection{
    typedef int SOCKET;
    typedef uint16_t PORT;
    typedef struct sockaddr_in SOCKADDR;
    typedef socklen_t SOCKLEN;

    PORT port;
    SOCKET endpoint;
    SOCKADDR sockaddr;
    SOCKLEN sockaddrLen;

    ~Connection(){
        close(endpoint);
    }

    int listen() const;
    std::shared_ptr<Connection> acceptConnection()const;
    template<int N=300>
    int sendMsg(const char[N])const;
    template<int N=300>
    std::pair<int, std::shared_ptr<char>> rcvMsg()const;
};

template<int N>
int Connection::sendMsg(const char msg[N])const{
    int len = N;
    int sent = 0;
    const char *str = msg;
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

template<int N>
std::pair<int, std::shared_ptr<char>> Connection::rcvMsg()const{
    char *buff = (char*)malloc(N*sizeof(char));

    int recvLen = 0;
    recvLen=recv(endpoint, (void*)buff,N,0);
   // if(recvLen == -1){
   //     return {-1, ""};
   // }
   std::shared_ptr<char> mg(buff, [](char*p){free(p);});
    return {recvLen, mg};
}

class ConnectionFactory{
public:
    using SOCKET = Connection::SOCKET;
    using PORT = Connection::PORT;
    using SOCKADDR = Connection::SOCKADDR;
    using SOCKLEN = Connection::SOCKLEN;
    std::shared_ptr<Connection> createServerConnection(PORT) const;
    std::shared_ptr<Connection> createClientConnection(PORT, const std::string&) const;
private:
};

#endif

