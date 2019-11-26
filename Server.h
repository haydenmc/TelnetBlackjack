#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <vector>
#include <mutex>

#pragma comment(lib, "Ws2_32.lib")

class Server
{
public:
    // Public methods
    Server(unsigned short port);
    void Start();

private:
    // Private members
    unsigned short port;
    SOCKET listenSocket = INVALID_SOCKET;
    std::mutex clientDataLock;
    std::vector<SOCKET> clientSockets;

    // Private methods
    void receive(SOCKET clientSocket);
    void removeClientSocket(SOCKET clientSocket);
};
