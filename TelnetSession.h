#pragma once
#include <functional>

enum BufferAction
{
    Keep,
    Clear
};

class TelnetSession
{
public:
    // Public methods
    virtual void Send(std::string message) = 0;
    virtual void Close() = 0;
    // Event/callback registrations
    virtual void OnReceive(
        std::function<BufferAction(char buffer[], unsigned int bufferFilled)> callback) = 0;
};
