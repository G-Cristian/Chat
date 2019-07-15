#include "../Include/connection.h"
#include <sstream>

int Connection::listen() const{
#if defined(__linux__) || defined(__unix__)
    if(::listen(endpoint, 50) == -1){
        throw "Cannot listen.";
    }
#elif defined(_WIN32) || defined(WIN32)
	if (::listen(endpoint, SOMAXCONN) == SOCKET_ERROR) {
		std::stringstream ss;
		ss << "listen failed with error: " << WSAGetLastError() << std::endl;
		throw ss.str();
	}
#else
#error "OS not supperted."
#endif
    return 1;
}

std::shared_ptr<Connection> Connection::acceptConnection()const{
#if defined(__linux__) || defined(__unix__)
    std::shared_ptr<Connection> connection = std::make_shared<Connection>();
    connection->port = port;
    connection->sockaddrLen = sizeof(struct sockaddr_in);
    connection->endpoint = accept(endpoint, (struct sockaddr*)&(connection->sockaddr), &(connection->sockaddrLen));
    if(connection->endpoint == -1){
        throw "Error accept.";
    }
#elif defined(_WIN32) || defined(WIN32)
	//windows based
	std::shared_ptr<Connection> connection = std::make_shared<Connection>();
	connection->port = port;
	connection->endpoint = INVALID_SOCKET;

	int iResult = WSAStartup(MAKEWORD(2, 2), &(connection->wsaData));
	if (iResult != 0) {
		std::stringstream ss;
		ss << "WSAStartup failed with error: " << iResult << std::endl;
		throw ss.str();
	}

	connection->sockaddrLen = sizeof(struct sockaddr_in);
	connection->endpoint = accept(endpoint, &(connection->sockaddr), (int*)&(connection->sockaddrLen));
	// Accept a client socket
	if (connection->endpoint == INVALID_SOCKET) {
		WSACleanup();
		std::stringstream ss;
		ss << "accept failed with error: " << WSAGetLastError() << std::endl;
		throw ss.str();
	}
#else
#error "OS not supperted."
#endif
    return connection;
}

std::shared_ptr<Connection> ConnectionFactory::createServerConnection(PORT port) const{
#if defined(__linux__) || defined(__unix__)
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
#elif defined(_WIN32) || defined(WIN32)
	//windows based
	std::shared_ptr<Connection> connection = std::make_shared<Connection>();
	connection->port = port;
	connection->endpoint = INVALID_SOCKET;
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &(connection->wsaData));
	if (iResult != 0) {
		std::stringstream ss;
		ss << "WSAStartup failed with error: " << iResult << std::endl;
		throw ss.str();
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	std::stringstream ss;
	ss << port;
	iResult = getaddrinfo(NULL, ss.str().c_str(), &hints, &result);
	if (iResult != 0) {
		WSACleanup();
		std::stringstream ss;
		ss << "getaddrinfo failed with error: " << iResult << std::endl;
		throw ss.str();
	}

	// Create a SOCKET for connecting to server
	connection->endpoint = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (connection->endpoint == INVALID_SOCKET) {
		freeaddrinfo(result);
		WSACleanup();
		std::stringstream ss;
		ss << "socket failed with error: " << WSAGetLastError() << std::endl;
		throw ss.str();
	}

	connection->sockaddr = *(result->ai_addr);
	connection->sockaddrLen = result->ai_addrlen;

	// Setup the TCP listening socket
	iResult = bind(connection->endpoint, (sockaddr*)&(connection->sockaddr), (int)connection->sockaddrLen);
	if (iResult == SOCKET_ERROR) {
		freeaddrinfo(result);
		closesocket(connection->endpoint);
		connection->endpoint = INVALID_SOCKET;
		WSACleanup();
		std::stringstream ss;
		ss << "bind failed with error: " << WSAGetLastError() << std::endl;
		throw ss.str();
	}

	freeaddrinfo(result);

#else
#error "OS not supperted."
#endif
    return connection;
}

std::shared_ptr<Connection> ConnectionFactory::createClientConnection(PORT port, const std::string &ip) const{
#if defined(__linux__) || defined(__unix__)
	//linux based
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
#elif defined(_WIN32) || defined(WIN32)
	//windows based
	std::shared_ptr<Connection> connection = std::make_shared<Connection>();
	connection->port = port;
	connection->endpoint = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &(connection->wsaData));
	if (iResult != 0) {
		std::stringstream ss;
		ss << "WSAStartup failed with error: " << iResult << std::endl;
		throw ss.str();
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	std::stringstream ss;
	ss << port;
	iResult = getaddrinfo(ip.c_str(), ss.str().c_str(), &hints, &result);
	if (iResult != 0) {
		WSACleanup();
		std::stringstream ss;
		ss << "getaddrinfo failed with error: " << iResult << std::endl;
		throw ss.str();
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		connection->endpoint = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (connection->endpoint == INVALID_SOCKET) {
			freeaddrinfo(result);
			WSACleanup();
			std::stringstream ss;
			ss << "socket failed with error: " << WSAGetLastError() << std::endl;
			throw ss.str();
		}

		connection->sockaddr = *(ptr->ai_addr);
		connection->sockaddrLen = ptr->ai_addrlen;

		// Connect to server.
		iResult = connect(connection->endpoint, (struct sockaddr*)&(connection->sockaddr), (int)connection->sockaddrLen);
		if (iResult == SOCKET_ERROR) {
			closesocket(connection->endpoint);
			connection->endpoint = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (connection->endpoint == INVALID_SOCKET) {
		WSACleanup();
		throw "Unable to connect to server!";
	}

#else
#error "OS not supperted."
#endif

    return connection;
}