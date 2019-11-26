#include "Server.h"
#include <stdexcept>
#include <thread>
#include <iostream>

#pragma region Constants
static const unsigned int DEFAULT_RECEIVE_BUFFER_LENGTH = 512u;
#pragma endregion

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
        // Spin up new thread to handle this client
        std::thread(&Server::receive, this, newClientSocket).detach();
        std::cout << "Accepted client connection. # connected: " << this->clientSockets.size() << std::endl;
    }
}
#pragma endregion

#pragma region Private methods
void Server::receive(SOCKET clientSocket)
{
    char recvbuf[DEFAULT_RECEIVE_BUFFER_LENGTH];
    int iResult, iSendResult;
    int recvbuflen = DEFAULT_RECEIVE_BUFFER_LENGTH;

    // Receive until the peer shuts down the connection
    while (true)
    {
        iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            std::cout << "Bytes received: " << iResult << std::endl;

            // Echo the buffer back to the sender
            iSendResult = send(clientSocket, recvbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR) {
                std::cerr << "Error sending to client: '" << WSAGetLastError() << std::endl;
                closesocket(clientSocket);
            }
            std::cout << "Bytes sent: " << iSendResult << std::endl;
        }
        else if (iResult == 0)
        {
            this->removeClientSocket(clientSocket);
            std::cout << "Client connection closing. # connected: " << this->clientSockets.size() << std::endl;
            break;
        }
        else
        {
            std::cerr << "Error receiving from client: '" << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            this->removeClientSocket(clientSocket);
            break;
        }
    }
}

void Server::removeClientSocket(SOCKET clientSocket)
{
    std::lock_guard<std::mutex> lockGuard(this->clientDataLock);
    auto it = std::find(
        this->clientSockets.begin(),
        this->clientSockets.end(),
        clientSocket
    );

    if (it != this->clientSockets.end())
    {
        std::swap(*it, this->clientSockets.back());
        this->clientSockets.pop_back();
    }
}
#pragma endregion