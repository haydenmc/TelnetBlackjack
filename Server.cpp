#include "Server.h"
#include <stdexcept>
#include <thread>
#include <iostream>

#pragma region Public methods
Server::Server(unsigned short port):
    port(port)
{ }

void Server::Start()
{
    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0)
    {
        throw std::runtime_error("WSAStartup failed: " + iResult);
    }

    // Create socket
    this->listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->listenSocket == INVALID_SOCKET)
    {
        WSACleanup();
        int socketError = WSAGetLastError();
        throw std::runtime_error("Error creating listen socket: " + socketError);
    }

    // Bind socket
    sockaddr_in sockaddr
    {
        AF_INET,
        htons(this->port),
        INADDR_ANY
    };

    iResult = bind(this->listenSocket, (SOCKADDR *) &sockaddr, sizeof (sockaddr));
    if (iResult == SOCKET_ERROR)
    {
        int socketError = WSAGetLastError();
        closesocket(this->listenSocket);
        WSACleanup();
        throw std::runtime_error("Error creating listen socket: " + socketError);
    }

    // Listen on socket
    if (listen(this->listenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        int socketError = WSAGetLastError();
        closesocket(this->listenSocket);
        WSACleanup();
        throw std::runtime_error("Listening on socket failed: " + socketError);
    }

    std::cout << "Server listening on port " << this->port << "..." << std::endl;

    // Accept client connections on a loop
    while (true)
    {
        // Accept a client socket
        SOCKET newClientSocket = accept(this->listenSocket, NULL, NULL);
        if (newClientSocket == INVALID_SOCKET)
        {
            std::cerr << "Accept socket failed with error '" << WSAGetLastError() << std::endl;
            closesocket(newClientSocket);
        }
        std::lock_guard<std::mutex> lockGuard(this->clientDataLock);
        this->clientSockets.push_back(newClientSocket);
        std::cout << "Accepted client connection." << std::endl;
    }
}
#pragma endregion

#pragma region Private methods
#pragma endregion