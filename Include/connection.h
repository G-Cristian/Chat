#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#if defined(__linux__) || defined(__unix__)
//linux based
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#elif defined(_WIN32) || defined(WIN32)
//windows based
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iphlpapi.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#else
#error "OS not supperted."
#endif

#include <cstring>
#include <memory>
#include <string>
#include <utility>

struct Connection{
#if defined(__linux__) || defined(__unix__)
	//linux based
    typedef int SOCKET;
    typedef uint16_t PORT;
    typedef struct sockaddr_in SOCKADDR;
    typedef socklen_t SOCKLEN;
	using INVALID_SOCKET = -1;
#elif defined(_WIN32) || defined(WIN32)
	//windows based
	typedef SOCKET SOCKET;
	typedef uint16_t PORT;
	typedef sockaddr SOCKADDR;
	typedef size_t SOCKLEN;
	//using INVALID_SOCKET = INVALID_SOCKET;
#else
#error "OS not supperted."
#endif

#if defined(_WIN32) || defined(WIN32)
	WSADATA wsaData;
#endif
    PORT port;
    SOCKET endpoint;
    SOCKADDR sockaddr;
    SOCKLEN sockaddrLen;

    Connection():port(0),endpoint(INVALID_SOCKET),sockaddrLen(0){
        std::memset((void*)(&sockaddr), 0, sizeof(SOCKADDR));
    }

    ~Connection(){
#if defined(__linux__) || defined(__unix__)
		//linux based
        if(endpoint != INVALID_SOCKET){
            close(endpoint);
            endpoint = INVALID_SOCKET;
        }
#elif defined(_WIN32) || defined(WIN32)
		//windows based
		if (endpoint != INVALID_SOCKET)
		{
			shutdown(endpoint, SD_SEND);
			closesocket(endpoint);
			WSACleanup();
			endpoint = INVALID_SOCKET;
		}
#else
#error "OS not supperted."
#endif
    }

    int listen() const;
    std::shared_ptr<Connection> acceptConnection()const;
    template<int N=300>
    int sendMsg(const char[N])const;
    template<int N=300>
    std::pair<int, std::shared_ptr<char>> rcvMsg()const;
};

template<int N>
int Connection::sendMsg(const char msg[N])const {
	int len = N;
	int sent = 0;
	const char *str = msg;
#if defined(__linux__) || defined(__unix__)
	do {
		len -= sent;
		str += sent;
		sent = send(endpoint, str, len, 0);
		if (sent == -1) {
			return 0;
		}
	} while (sent != len);

	return 1;
#elif defined(_WIN32) || defined(WIN32)
	int iResult = send(endpoint, str, N, 0);
	if (iResult == SOCKET_ERROR) {
		return 0;
	}
	return 1;
#else
#error "OS not supperted."
#endif
}

template<int N>
std::pair<int, std::shared_ptr<char>> Connection::rcvMsg()const{
    char *buff = (char*)malloc(N*sizeof(char));
	int recvLen = 0;
#if defined(__linux__) || defined(__unix__)
    recvLen=recv(endpoint, (void*)buff,N,0);
   // if(recvLen == -1){
   //     return {-1, ""};
   // }
#elif defined(_WIN32) || defined(WIN32)
	recvLen = recv(endpoint, buff, N, 0);
	if (recvLen < 0) {
		recvLen = -1;
	}
#else
#error "OS not supperted."
#endif
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

