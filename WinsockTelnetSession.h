#pragma once

#include <functional>
#include <string>

#include "TelnetSession.h"
#include "WinSock2.h"

class WinsockTelnetSession : 
    public TelnetSession
{
public:
    // Public methods
    WinsockTelnetSession(SOCKET clientSocket);

    // TelnetSession
    void Start() override;
    void Send(std::string message) override;
    void Close() override;
    // Event/callback registrations
    void OnReceive(
        std::function<BufferAction(char buffer[], unsigned int bufferFilled)> callback) override;
    void OnClosed(
        std::function<void(std::string reason)> callback) override;

private:
    // Private members
    SOCKET socket;

    // Callbacks
    std::function<BufferAction(char buffer[], unsigned int bufferFilled)> onReceiveCallback;
    std::function<void(std::string reason)> onClosedCallback;
};