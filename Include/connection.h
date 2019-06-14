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
    int sendMsg(const std::string&)const;
    std::pair<int, std::string> rcvMsg()const;
};

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

