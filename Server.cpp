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

        // Send a bunch of configuration
        // Set NVT mode to say that I will echo back characters.
        int iSendResult;
        unsigned char willEcho[3] = { 0xff, 0xfb, 0x01 };
        iSendResult = send(newClientSocket, (char *)willEcho, 3, 0);

        // Set NVT requesting that the remote system not/dont echo back characters
        unsigned char dontEcho[3] = { 0xff, 0xfe, 0x01 };
        iSendResult = send(newClientSocket, (char *)dontEcho, 3, 0);

        // Set NVT mode to say that I will supress go-ahead. Stops remote clients from doing local linemode.
        unsigned char willSGA[3] = { 0xff, 0xfb, 0x03 };
        iSendResult = send(newClientSocket, (char *)willSGA, 3, 0);

        // Send a banner
        iSendResult = send(newClientSocket, "Hello world!", 12, 0);

        // Spin up new thread to handle this client
        std::thread(&Server::receive, this, newClientSocket).detach();
        std::cout << "Accepted client connection. # connected: " << this->clientSockets.size() << std::endl;
    }
}
#pragma endregion

#pragma region Private methods
void Server::receive(SOCKET clientSocket)
{
    // Receive until the peer shuts down the connection
    std::string received;
    char recvbuf[DEFAULT_RECEIVE_BUFFER_LENGTH];
    int iResult, iSendResult, bufferFilledTo = 0;
    int recvbuflen = DEFAULT_RECEIVE_BUFFER_LENGTH;
    while (true)
    {
        iResult = recv(clientSocket, (char*)(recvbuf + bufferFilledTo), recvbuflen - bufferFilledTo, 0);
        if (iResult > 0)
        {
            // We've received data - process what we've received so far
            std::cout << "Bytes received: " << iResult << std::endl;
            bufferFilledTo += iResult;
            this->processBuffer(recvbuf, bufferFilledTo);
        }
        else if (iResult == 0)
        {
            // This means the client closed the connection.
            this->removeClientSocket(clientSocket);
            std::cout << "Client connection closing. # connected: " << this->clientSockets.size() << std::endl;
            break;
        }
        else
        {
            // Some error occurred.
            std::cerr << "Error receiving from client: '" << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            this->removeClientSocket(clientSocket);
            break;
        }
    }
}

void Server::processBuffer(char buffer[], int &bufferFilledTo)
{
    std::cout << "Processing buffer: " << bufferFilledTo << " chars." << std::endl;
    std::cout << "\tBuffer contents: ";
    for (unsigned int i = 0; i < bufferFilledTo; ++i)
    {
        std::cout << buffer[i];
    }
    std::cout << std::endl;
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