#include "WinsockTelnetServer.h"
#include <stdexcept>
#include <thread>
#include <iostream>

#pragma region Public methods
WinsockTelnetServer::WinsockTelnetServer(unsigned short port):
    port(port)
{ }

void WinsockTelnetServer::Start()
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

        // Spin up new session to handle this client
        auto session = new WinsockTelnetSession(newClientSocket);
        std::thread(&WinsockTelnetSession::Start, session).detach();
        if (this->onClientConnectedCallback)
        {
            this->onClientConnectedCallback(session);
        }
        std::cout << "Accepted client connection." << std::endl;
    }
}

void WinsockTelnetServer::OnClientConnected(
    std::function<void(TelnetSession* session)> callback)
{
    this->onClientConnectedCallback = callback;
}
#pragma endregion