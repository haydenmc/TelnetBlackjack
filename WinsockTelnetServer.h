#pragma once

#include <functional>
#include <map>
#include <mutex>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "TelnetServer.h"
#include "WinsockTelnetSession.h"

#pragma comment(lib, "Ws2_32.lib")

class WinsockTelnetServer : 
    public TelnetServer
{
public:
    // Public methods
    WinsockTelnetServer(unsigned short port);

    // TelnetServer methods
    void Start() override;
    void OnClientConnected(
        std::function<void(TelnetSession* session)> callback) override;

private:
    // Private members
    unsigned short port;
    SOCKET listenSocket = INVALID_SOCKET;

    // Callbacks
    std::function<void(TelnetSession* session)> onClientConnectedCallback;
};
