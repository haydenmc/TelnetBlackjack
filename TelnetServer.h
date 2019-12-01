#pragma once
#include <functional>
#include <string>

class TelnetServer
{
public:
    virtual void OnClientConnected(
        std::function<void(unsigned long long socket)> callback) = 0;
    virtual void OnClientDisconnected(
        std::function<void(unsigned long long socket, std::string reason)> callback) = 0;
};
