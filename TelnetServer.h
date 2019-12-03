#pragma once
#include <functional>
#include <string>

#include "TelnetSession.h"

class TelnetServer
{
public:
    virtual void Start() = 0;
    virtual void OnClientConnected(
        std::function<void(TelnetSession* session)> callback) = 0;
};
